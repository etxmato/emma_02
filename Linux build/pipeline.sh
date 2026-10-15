#!/bin/bash
# Host-side Emma 02 packaging pipeline (runs on macOS, drives a Lima builder VM).
# ONE command from the Mac: sync -> compile (right arch/libs) -> package -> copy .deb back.
#
# Usage:
#   ./pipeline.sh --arch amd64|arm64 [--format deb|rpm] [--wx 3.0|3.1|3.2] [--distro fedora|suse] [--jobs N] [--clean] [--no-build] [--out DIR]
#
#   --arch    amd64 -> Release_x86 | arm64 -> Release
#   --format  deb (default) or rpm
#   --wx      wx ABI of the target distro (sets the builder VM and artifact suffix):
#             deb  3.2 (default) | 3.0  (Ubuntu 20.04/22.04, *-2204 VMs, _wx30)
#             rpm  3.2 (default) | 3.1  (openSUSE Leap 15, *-15 VMs, _wx31)
#             Fedora is wx 3.2 only (it ships no 3.0/3.1)
#   --distro  fedora|suse — required with --format rpm; picks the builder VM and the
#             DEPENDS_RPM_* dependency set (RPM package names differ per distro)
#   --no-build  skip compile, package the binary already in the VM
#   --clean   clean rebuild (make clean first)
#   --out DIR where to place the artifact on the Mac
#             (default: the installation repo at $INSTALL_ROOT/v<version>)
#
# Reads Linux build/emma02_package.conf as the single source of truth (PACKAGE/VERSION).
set -e

ARCH=""; WXVER="3.2"; FORMAT="deb"; DISTRO=""; JOBS=""; CLEAN=0; BUILD=1; OUTDIR=""
while [ $# -gt 0 ]; do
  case "$1" in
    --arch) ARCH="$2"; shift 2;;
    --wx) WXVER="$2"; shift 2;;
    --format) FORMAT="$2"; shift 2;;
    --distro) DISTRO="$2"; shift 2;;
    --jobs) JOBS="$2"; shift 2;;
    --clean) CLEAN=1; shift;;
    --no-build) BUILD=0; shift;;
    --out) OUTDIR="$2"; shift 2;;
    *) echo "unknown arg: $1" >&2; exit 1;;
  esac
done

SCRIPTDIR="$(cd "$(dirname "$0")" && pwd)"
REPO="$(cd "$SCRIPTDIR/.." && pwd)"
CONF="$SCRIPTDIR/emma02_package.conf"
# Published artifacts land in a version-named subfolder of the installation repo
# (e.g. .../emma_02_installation/v2.01/), matching the existing folder convention.
INSTALL_ROOT="${INSTALL_ROOT:-/Users/etxmato/workspace/emma_02_installation}"

case "$ARCH" in
  amd64) OTHER_DIR="Release";;
  arm64) OTHER_DIR="Release_x86";;
  *) echo "--arch amd64|arm64 required" >&2; exit 1;;
esac
case "$FORMAT" in
  deb)
    case "$ARCH" in
      amd64) VM="emma02-x64";;
      arm64) VM="emma02-arm";;
    esac
    case "$WXVER" in
      3.0) VM="$VM-2204"; SUFFIX="_wx30";;
      3.2) SUFFIX="";;
      *) echo "--wx 3.0|3.2 required for deb" >&2; exit 1;;
    esac
    EXT="deb"; GUEST_SCRIPT="build_deb.sh"; GUEST_ARGS="--arch $ARCH --wx $WXVER"
    ;;
  rpm)
    case "$DISTRO" in
      fedora)
        [ "$WXVER" = "3.2" ] || { echo "fedora rpm is wx 3.2 only (Fedora ships no wx 3.0/3.1)" >&2; exit 1; }
        case "$ARCH" in
          amd64) VM="emma02-fedora-x64";;
          arm64) VM="emma02-fedora";;
        esac
        SUFFIX="_fedora"
        ;;
      suse)
        # wx 3.2 -> Leap 16 (base VMs, _suse suffix); wx 3.1 -> Leap 15 (*-15 VMs, _wx31)
        case "$WXVER" in
          3.2) VM_SUF="";  SUFFIX="_suse";;
          3.1) VM_SUF="-15"; SUFFIX="_suse_wx31";;
          *) echo "--wx 3.1|3.2 required for suse rpm" >&2; exit 1;;
        esac
        case "$ARCH" in
          amd64) VM="emma02-suse$VM_SUF-x64";;
          arm64) VM="emma02-suse$VM_SUF";;
        esac
        ;;
      *) echo "--distro fedora|suse required with --format rpm" >&2; exit 1;;
    esac
    EXT="rpm"; GUEST_SCRIPT="build_rpm.sh"; GUEST_ARGS="--arch $ARCH --distro $DISTRO --wx $WXVER"
    ;;
  *) echo "unknown --format '$FORMAT' (deb|rpm)" >&2; exit 1;;
esac

command -v limactl >/dev/null || { echo "limactl not found (install Lima)" >&2; exit 1; }
command -v rsync   >/dev/null || { echo "rsync not found" >&2; exit 1; }

PKG="$(grep -E '^PACKAGE=' "$CONF" | head -n1 | cut -d= -f2-)"
VER="$(grep -E '^VERSION=' "$CONF" | head -n1 | cut -d= -f2-)"
[ -n "$PKG" ] && [ -n "$VER" ] || { echo "cannot read PACKAGE/VERSION from $CONF" >&2; exit 1; }
# Artifact name is version-free: the release version lives in the v<version> folder
# (and in the package's Version: field), so download links stay stable across releases.
ARTIFACT="${PKG}_${ARCH}${SUFFIX}.$EXT"
[ -n "$OUTDIR" ] || OUTDIR="$INSTALL_ROOT/v$VER"

echo "== Emma 02 pipeline =="
echo "  arch:    $ARCH   VM: $VM   wx: $WXVER   distro: ${DISTRO:-ubuntu}"
echo "  format:  $FORMAT"
echo "  package: $PKG $VER -> $ARTIFACT"
echo "  repo:    $REPO"
echo "  install: $OUTDIR"

# 1. ensure the builder VM exists and is running
if ! limactl list --json 2>/dev/null | grep -q "\"name\":\"$VM\""; then
  echo "ERROR: builder VM '$VM' not found. Create it first (see packaging skill)." >&2
  exit 1
fi
STATUS="$(limactl list --json 2>/dev/null | python3 -c "import sys,json
for l in sys.stdin:
    if l.strip():
        v=json.loads(l)
        if v['name']=='$VM': print(v['status'])")"
if [ "$STATUS" != "Running" ]; then
  echo "-- starting VM $VM ..."
  limactl start "$VM" --tty=false >/dev/null
fi

SSHCFG="$HOME/.lima/$VM/ssh.config"
[ -f "$SSHCFG" ] || { echo "missing ssh config: $SSHCFG" >&2; exit 1; }
GUEST_HOME="$(limactl shell "$VM" -- sh -c 'cd ~ && pwd' 2>/dev/null | tr -d '\r')"
[ -n "$GUEST_HOME" ] || { echo "cannot determine guest home on $VM" >&2; exit 1; }
GUEST_DIR="$GUEST_HOME/emma02-build"
echo "  guest:   $VM:$GUEST_DIR"

# 2. sync the tree into the guest's writable disk (host ~ mount is read-only)
#    --delete + excludes so guest build outputs survive; exclude stray artifacts.
echo "-- syncing tree ..."
rsync -a --delete \
  --exclude='.git' \
  --exclude="$OTHER_DIR" \
  --exclude='Debug' --exclude='Debug_x86' \
  --exclude='*.o' --exclude='*.d' --exclude='emma_02' --exclude='*.deb' --exclude='*.rpm' \
  --exclude='.DS_Store' --exclude='**/.DS_Store' --exclude='.AppleDouble' --exclude='**/.AppleDouble' --exclude='.LSOverride' --exclude='**/.LSOverride' \
  -e "ssh -F $SSHCFG" \
  "$REPO/" "lima-$VM:$GUEST_DIR/"

# 3. build + package inside the guest via the guest-side scripts
ARGS="$GUEST_ARGS --src $GUEST_DIR --out $GUEST_DIR"
[ "$BUILD" = "1" ] && ARGS="$ARGS --build"
[ "$CLEAN" = "1" ] && ARGS="$ARGS --clean"
[ -n "$JOBS" ] && ARGS="$ARGS --jobs $JOBS"

echo "-- building + packaging in guest ..."
GUEST_LOG="$OUTDIR/${PKG}_${ARCH}${SUFFIX}.$EXT.build.log"
limactl shell "$VM" -- sh -lc "cd '$GUEST_DIR' && './Linux build/$GUEST_SCRIPT' $ARGS" 2>&1 | tee "$GUEST_LOG" | tail -25
echo "guest log: $GUEST_LOG"

# 4. copy the artifact back to the Mac
mkdir -p "$OUTDIR"
limactl copy "$VM:$GUEST_DIR/$ARTIFACT" "$OUTDIR/$ARTIFACT"
echo "== done =="
ls -lh "$OUTDIR/$ARTIFACT"
