#!/bin/bash
# Host-side Emma 02 packaging pipeline (runs on macOS, drives a Lima builder VM).
# ONE command from the Mac: sync -> compile (right arch/libs) -> package -> copy .deb back.
#
# Usage:
#   ./pipeline.sh --arch amd64|arm64 [--wx 3.0|3.2] [--format deb] [--jobs N] [--clean] [--no-build] [--out DIR]
#
#   --arch    amd64 -> Release_x86 | arm64 -> Release
#   --wx      3.2 (default, Ubuntu 24.04+ builds) or 3.0 (Ubuntu 20.04/22.04 builds)
#             wx 3.0 selects the *-2204 builder VMs and the _wx30 artifact suffix
#   --format  deb (default); rpm reserved for later (same conf, no data.tar)
#   --no-build  skip compile, package the binary already in the VM
#   --clean   clean rebuild (make clean first)
#   --out DIR where to place the artifact on the Mac
#             (default: the installation repo at $INSTALL_ROOT/v<version>)
#
# Reads Linux build/emma02_package.conf as the single source of truth (PACKAGE/VERSION).
set -e

ARCH=""; WXVER="3.2"; FORMAT="deb"; JOBS=""; CLEAN=0; BUILD=1; OUTDIR=""
while [ $# -gt 0 ]; do
  case "$1" in
    --arch) ARCH="$2"; shift 2;;
    --wx) WXVER="$2"; shift 2;;
    --format) FORMAT="$2"; shift 2;;
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
  amd64) VM="emma02-x64"; OTHER_DIR="Release";;
  arm64) VM="emma02-arm"; OTHER_DIR="Release_x86";;
  *) echo "--arch amd64|arm64 required" >&2; exit 1;;
esac
case "$WXVER" in
  3.0) VM="$VM-2204"; SUFFIX="_wx30";;
  3.2) SUFFIX="";;
  *) echo "--wx 3.0|3.2 required" >&2; exit 1;;
esac
[ "$FORMAT" = "deb" ] || { echo "format '$FORMAT' not implemented yet (deb only)" >&2; exit 1; }

command -v limactl >/dev/null || { echo "limactl not found (install Lima)" >&2; exit 1; }
command -v rsync   >/dev/null || { echo "rsync not found" >&2; exit 1; }

PKG="$(grep -E '^PACKAGE=' "$CONF" | head -n1 | cut -d= -f2-)"
VER="$(grep -E '^VERSION=' "$CONF" | head -n1 | cut -d= -f2-)"
[ -n "$PKG" ] && [ -n "$VER" ] || { echo "cannot read PACKAGE/VERSION from $CONF" >&2; exit 1; }
# Artifact name is version-free: the release version lives in the v<version> folder
# (and in the package's Version: field), so download links stay stable across releases.
ARTIFACT="${PKG}_${ARCH}${SUFFIX}.deb"
[ -n "$OUTDIR" ] || OUTDIR="$INSTALL_ROOT/v$VER"

echo "== Emma 02 pipeline =="
echo "  arch:    $ARCH   VM: $VM   wx: $WXVER"
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
  --exclude='*.o' --exclude='*.d' --exclude='emma_02' --exclude='*.deb' \
  -e "ssh -F $SSHCFG" \
  "$REPO/" "lima-$VM:$GUEST_DIR/"

# 3. build + package inside the guest via the guest-side scripts
DEB_ARGS="--arch $ARCH --wx $WXVER --src $GUEST_DIR --out $GUEST_DIR"
[ "$BUILD" = "1" ] && DEB_ARGS="$DEB_ARGS --build"
[ "$CLEAN" = "1" ] && DEB_ARGS="$DEB_ARGS --clean"
[ -n "$JOBS" ] && DEB_ARGS="$DEB_ARGS --jobs $JOBS"

echo "-- building + packaging in guest ..."
limactl shell "$VM" -- sh -lc "cd '$GUEST_DIR' && './Linux build/build_deb.sh' $DEB_ARGS" 2>&1 | tail -25

# 4. copy the artifact back to the Mac
mkdir -p "$OUTDIR"
limactl copy "$VM:$GUEST_DIR/$ARTIFACT" "$OUTDIR/$ARTIFACT"
echo "== done =="
ls -lh "$OUTDIR/$ARTIFACT"
