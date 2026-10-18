#!/bin/bash
# Host-side Emma 02 packaging pipeline (runs on macOS, drives a Lima builder VM).
# ONE command from the Mac: sync -> compile (right arch/libs) -> package -> copy artifact back.
#
# Usage:
#   ./pipeline.sh --arch amd64|arm64 [--format deb|rpm] [--wx 3.0|3.1|3.2] [--distro fedora|suse] [--jobs N] [--clean] [--no-build] [--out DIR]
#   ./pipeline.sh --arch arm|x64|x86 --format exe [--wx 3.3|3.2] [--no-build] [--out DIR]
#   ./pipeline.sh --format dmg [--wx 3.3] [--deploy 12.0|10.13] [--jobs N] [--clean] [--no-build] [--out DIR]
#
#   Linux: --arch amd64 -> Release_x86 | arm64 -> Release
#   Windows (exe): --arch arm -> ARM64 | x64 -> x64 | x86 -> Win32 (all from one ARM64 Win11 VM)
#     --wx 3.3: vc17 modern line (arm/x64/x86) -> emma_02_<arch>_windows.exe
#     --wx 3.2: vc9 XP line (x64/x86 only) -> emma_02_<arch>_windows_xp.exe
#     --wx 2.8: vc9 w2k line (x86 only) -> emma_02_x86_windows_w2k.exe
#   macOS (dmg): no --arch (one universal arm64+x86_64 binary, built natively on the Mac, no VM)
#     --deploy 12.0 (default, Monterey+) | 10.13 (High Sierra, needs 10.13-built wx3.3 tree)
#   --format  deb (default) or rpm or exe or dmg
#   --wx      wx ABI of the target:
#             deb  3.2 (default) | 3.0  (Ubuntu 22.04, *-2204 VMs, _legacy)
#             rpm  3.2 (default) | 3.1  (openSUSE Leap 15, *-15 VMs, _leap15)
#             exe  3.3 (default, vc17 modern line) | 3.2 (vc9 XP line, x64/x86 -> _xp)
#                  | 2.8 (vc9 w2k line, x86 only -> _w2k)
#             dmg  3.3 (default, Emma 02 Xcode target)
#             Fedora is wx 3.2 only (it ships no 3.0/3.1)
#   --distro  fedora|suse — required with --format rpm; picks the builder VM and the
#             DEPENDS_RPM_* dependency set (RPM package names differ per distro)
#   --no-build  skip compile, package the binary already in the VM
#   --clean   clean rebuild (make clean first; ignored for exe for now)
#   --out DIR where to place the artifact on the Mac
#             (default: the installation repo at $INSTALL_ROOT/v<version>)
#
# Reads Pipeline/emma02_package.conf as the single source of truth (PACKAGE/VERSION).
set -e

ARCH=""; WXVER=""; FORMAT="deb"; DISTRO=""; DEPLOY=""; JOBS=""; CLEAN=0; BUILD=1; OUTDIR=""
while [ $# -gt 0 ]; do
  case "$1" in
    --arch) ARCH="$2"; shift 2;;
    --wx) WXVER="$2"; shift 2;;
    --format) FORMAT="$2"; shift 2;;
    --distro) DISTRO="$2"; shift 2;;
    --deploy) DEPLOY="$2"; shift 2;;
    --jobs) JOBS="$2"; shift 2;;
    --clean) CLEAN=1; shift;;
    --no-build) BUILD=0; shift;;
    --out) OUTDIR="$2"; shift 2;;
    *) echo "unknown arg: $1" >&2; exit 1;;
  esac
done
# default wx per format (so --wx can be omitted)
if [ -z "$WXVER" ]; then
  case "$FORMAT" in
    exe|dmg) WXVER="3.3";;
    *) WXVER="3.2";;
  esac
fi

SCRIPTDIR="$(cd "$(dirname "$0")" && pwd)"
REPO="$(cd "$SCRIPTDIR/.." && pwd)"
CONF="$SCRIPTDIR/emma02_package.conf"
# Published artifacts land in a version-named subfolder of the installation repo
# (e.g. .../emma_02_installation/v2.01/), matching the existing folder convention.
INSTALL_ROOT="${INSTALL_ROOT:-/Users/etxmato/workspace/emma_02_installation}"

case "$ARCH" in
  amd64) OTHER_DIR="Release";;
  arm64) OTHER_DIR="Release_x86";;
  arm|x64|x86) OTHER_DIR="";;
  "") [ "$FORMAT" = "dmg" ] || { echo "--arch amd64|arm64 required (deb/rpm) or arm|x64|x86 (exe)" >&2; exit 1; }
     OTHER_DIR="";;
  *) echo "--arch amd64|arm64 required (deb/rpm) or arm|x64|x86 (exe)" >&2; exit 1;;
esac
case "$FORMAT" in
  deb)
    case "$ARCH" in
      amd64) VM="emma02-x64";;
      arm64) VM="emma02-arm";;
      *) echo "--arch amd64|arm64 required with --format deb" >&2; exit 1;;
    esac
    case "$WXVER" in
      3.0) VM="$VM-2204"; SUFFIX="_legacy";;
      3.2) SUFFIX="";;
      *) echo "--wx 3.0|3.2 required for deb" >&2; exit 1;;
    esac
    EXT="deb"; GUEST_SCRIPT="build_deb.sh"; GUEST_ARGS="--arch $ARCH --wx $WXVER"
    ;;
  rpm)
    case "$ARCH" in
      amd64|arm64) ;;
      *) echo "--arch amd64|arm64 required with --format rpm" >&2; exit 1;;
    esac
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
        # wx 3.2 -> Leap 16 (base VMs, _leap16 suffix); wx 3.1 -> Leap 15 (*-15 VMs, _leap15)
        case "$WXVER" in
          3.2) VM_SUF="";  SUFFIX="_leap16";;
          3.1) VM_SUF="-15"; SUFFIX="_leap15";;
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
  exe)
    # All arches from one ARM64 Win11 VM (build_win.bat maps
    # arm->ARM64, x64->x64, x86->Win32; MSVC cross-builds x64/Win32).
    # --wx 3.3: vc17 modern line (arm/x64/x86); --wx 3.2: vc9 XP line (x64/x86 -> _xp);
    # --wx 2.8: vc9 w2k line (x86 only -> _w2k).
    case "$ARCH" in
      arm|x64|x86) ;;
      *) echo "--arch arm|x64|x86 required with --format exe" >&2; exit 1;;
    esac
    case "$WXVER" in
      3.3) SUFFIX="";;
      3.2) [ "$ARCH" = "arm" ] && { echo "--wx 3.2 XP line is x64/x86 only (no ARM64 in VC9)" >&2; exit 1; }
           SUFFIX="_xp";;
      2.8) [ "$ARCH" = "x86" ] || { echo "--wx 2.8 w2k line is x86 only" >&2; exit 1; }
           SUFFIX="_w2k";;
      *) echo "--wx 3.3|3.2|2.8 required for exe (3.3 vc17 modern, 3.2 vc9 XP, 2.8 vc9 w2k)" >&2; exit 1;;
    esac
    [ -z "$DISTRO" ] || { echo "--distro not used with --format exe" >&2; exit 1; }
    VM="emma02-win"; EXT="exe"
    GUEST_SCRIPT="build_win.bat"; GUEST_ARGS="--arch $ARCH --wx $WXVER"
    ;;
  dmg)
    # Native macOS build: no VM, no --arch (one universal binary covers
    # both Apple Silicon and Intel). Runs build_mac.sh locally.
    [ -z "$ARCH" ] || { echo "--arch not used with --format dmg (universal binary)" >&2; exit 1; }
    [ "$WXVER" = "3.3" ] || { echo "--wx 3.3 required for dmg (Emma 02 Xcode target)" >&2; exit 1; }
    [ -z "$DISTRO" ] || { echo "--distro not used with --format dmg" >&2; exit 1; }
    [ -z "$DEPLOY" ] && DEPLOY="12.0"
    case "$DEPLOY" in
      12.0) SUFFIX="";;
      10.13) SUFFIX="_highsierra";;
      *) echo "--deploy 12.0|10.13 required for dmg" >&2; exit 1;;
    esac
    VM=""; EXT="dmg"
    GUEST_SCRIPT="build_mac.sh"; GUEST_ARGS="--wx $WXVER --deploy $DEPLOY"
    ;;
  *) echo "unknown --format '$FORMAT' (deb|rpm|exe|dmg)" >&2; exit 1;;
esac

command -v limactl >/dev/null || { echo "limactl not found (install Lima)" >&2; exit 1; }
if [ "$FORMAT" = "exe" ]; then
  command -v tar >/dev/null || { echo "tar not found" >&2; exit 1; }
elif [ "$FORMAT" = "dmg" ]; then
  : # native macOS build: no VM tooling needed
else
  command -v rsync >/dev/null || { echo "rsync not found" >&2; exit 1; }
fi

PKG="$(grep -E '^PACKAGE=' "$CONF" | head -n1 | cut -d= -f2-)"
VER="$(grep -E '^VERSION=' "$CONF" | head -n1 | cut -d= -f2-)"
[ -n "$PKG" ] && [ -n "$VER" ] || { echo "cannot read PACKAGE/VERSION from $CONF" >&2; exit 1; }
# Artifact name is version-free: the release version lives in the v<version> folder
# (and in the package's Version:/AppVersion field), so download links stay stable.
# Windows keeps its historical underscore form (emma_02_<arch>_windows[_xp|_w2k].exe,
# matching the .iss OutputBaseFilename); deb/rpm use the dashed PACKAGE form;
# macOS is a universal DMG (emma_02_mac.dmg, or emma_02_mac_highsierra.dmg for 10.13).
if [ "$FORMAT" = "exe" ]; then
  ARTIFACT="emma_02_${ARCH}_windows${SUFFIX}.exe"
elif [ "$FORMAT" = "dmg" ]; then
  ARTIFACT="emma_02_mac${SUFFIX}.dmg"
else
  ARTIFACT="${PKG}_${ARCH}${SUFFIX}.$EXT"
fi
[ -n "$OUTDIR" ] || OUTDIR="$INSTALL_ROOT/v$VER"

echo "== Emma 02 pipeline =="
echo "  arch:    $ARCH   VM: $VM   wx: $WXVER   distro: ${DISTRO:-ubuntu}"
echo "  format:  $FORMAT"
echo "  package: $PKG $VER -> $ARTIFACT"
echo "  repo:    $REPO"
echo "  install: $OUTDIR"

# ---------------------------------------------------------------------------
# macOS dmg flow (native: no VM, no sync — builds in place on the Mac).
# ---------------------------------------------------------------------------
if [ "$FORMAT" = "dmg" ]; then
  echo "  guest:   native (no VM)"
  ARGS="$GUEST_ARGS --src $REPO --out $OUTDIR"
  [ "$BUILD" = "1" ] && ARGS="$ARGS --build"
  [ "$CLEAN" = "1" ] && ARGS="$ARGS --clean"
  [ -n "$JOBS" ] && ARGS="$ARGS --jobs $JOBS"

  echo "-- building + packaging (native) ..."
  LOG_BASE="${ARTIFACT%.dmg}"
  GUEST_LOG="$OUTDIR/${LOG_BASE}.build.log"
  mkdir -p "$OUTDIR"
  # Drop any previous artifact so a failed build can never leave a stale file
  # that looks like a fresh one.
  rm -f "$OUTDIR/$ARTIFACT"
  if ! "$SCRIPTDIR/$GUEST_SCRIPT" $ARGS > "$GUEST_LOG" 2>&1; then
    tail -n 40 "$GUEST_LOG"
    echo "ERROR: mac build failed - full log: $GUEST_LOG" >&2
    exit 1
  fi
  tail -n 25 "$GUEST_LOG"
  echo "build log: $GUEST_LOG"

  echo "== done =="
  ls -lh "$OUTDIR/$ARTIFACT"
  exit 0
fi

# ---------------------------------------------------------------------------
# Windows exe flow (plain-mode VM: no mounts, no guest rsync).
# Sync via tarball + limactl copy (scp backend), build via build_win.bat.
# Guest path is fixed (no spaces, no user lookup): C:\emma02-build.
# ---------------------------------------------------------------------------
if [ "$FORMAT" = "exe" ]; then
  if ! limactl list --json 2>/dev/null | grep -q "\"name\":\"$VM\""; then
    echo "ERROR: builder VM '$VM' not found. Create it first (see packaging skill: template:windows-11, ARM64, bake VS Build Tools 2022 + Inno 6 + wx 3.3.3)." >&2
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
  # NOTE: `limactl shell` does not pass arguments correctly to this Windows guest
  # (the guest's login shell is cmd and the quoting is consumed), so drive the
  # guest over ssh directly and use cmd (not powershell) for the simple steps.
  SSHCFG="$HOME/.lima/$VM/ssh.config"
  [ -f "$SSHCFG" ] || { echo "missing ssh config: $SSHCFG" >&2; exit 1; }
  win_guest() { ssh -F "$SSHCFG" "lima-$VM" "$1"; }
  if [ "$CLEAN" = "1" ]; then
    echo "(--clean ignored for exe for now: MSBuild incremental; delete guest ARM64/Release manually for a clean build)" >&2
  fi
  if [ -n "$JOBS" ]; then
    echo "(--jobs ignored for exe for now: MSBuild /m is always parallel)" >&2
  fi

  # 1. sync the tree via tarball (host ~ mount does not exist on plain-mode Windows;
  #    guest has no rsync, so no incremental sync yet — full tar each run, TODO rsync in guest).
  #    Excludes: build dirs/binaries, mac-only sources (images_osx, SDL2.framework),
  #    and the 104M IsTool/Emma 02 mac-app staging dir (dead weight — no .iss references it).
  echo "-- syncing tree (tar + scp) ..."
  TARBALL="$(mktemp -t emma02-win-sync-XXXXXX.tar.gz)"
  tar -czf "$TARBALL" \
    --exclude='.git' \
    --exclude='Win32' --exclude='x64' --exclude='ARM64' \
    --exclude='Release' --exclude='Release_x86' --exclude='Debug' --exclude='Debug_x86' \
    --exclude='images_osx' --exclude='SDL2.framework' --exclude='IsTool/Emma 02' \
    --exclude='*.o' --exclude='*.d' --exclude='*.deb' --exclude='*.rpm' \
    --exclude='emma_02_arm' --exclude='emma_02_amd64' \
    --exclude='IsTool/x86/*.exe' --exclude='IsTool/x64/*.exe' --exclude='IsTool/arm64/*.exe' \
    --exclude='.DS_Store' --exclude='.AppleDouble' --exclude='.LSOverride' --exclude='._*' \
    -C "$REPO" .
  win_guest 'if not exist C:\emma02-build mkdir C:\emma02-build'
  limactl copy --backend=scp "$TARBALL" "$VM:C:/emma02-build.tar.gz"
  win_guest 'tar -xzf C:/emma02-build.tar.gz -C C:/emma02-build'
  # tar does not delete files that are absent from the archive, so stale
  # AppleDouble / metadata files left in the guest tree survive between runs
  # (they polluted the first installers). Purge them every run (guest-side script).
  # One-time: remove the pre-rename "Linux build" dir (renamed to Pipeline/
  # 2026-10-18); rmdir via cmd because Remove-Item on that path proved unreliable
  # through the ssh/cmd/powershell chain. 2>nul makes it a no-op once gone.
  win_guest 'rmdir /s /q "C:\emma02-build\Linux build" 2>nul & exit 0'
  win_guest 'powershell -NoProfile -ExecutionPolicy Bypass -File "C:\emma02-build\Pipeline\purge_win_metadata.ps1"'
  rm -f "$TARBALL"

  # 2. build + package inside the guest via build_win.bat
  ARGS="$GUEST_ARGS --src C:\emma02-build --out C:\emma02-build"
  [ "$BUILD" = "1" ] || ARGS="$ARGS --no-build"

  echo "-- building + packaging in guest ..."
  LOG_BASE="${ARTIFACT%.exe}"
  GUEST_LOG="$OUTDIR/${LOG_BASE}.build.log"
  mkdir -p "$OUTDIR"
  # Drop any previous artifact so a failed build can never leave a stale file
  # that looks like a fresh one.
  rm -f "$OUTDIR/$ARTIFACT"
  if ! win_guest "cd /d C:\\emma02-build && \"Pipeline\\build_win.bat\" $ARGS" > "$GUEST_LOG" 2>&1; then
    tail -n 40 "$GUEST_LOG"
    echo "ERROR: build failed in guest - full log: $GUEST_LOG" >&2
    exit 1
  fi
  tail -n 25 "$GUEST_LOG"
  echo "guest log: $GUEST_LOG"

  # 3. copy the installer back to the Mac
  limactl copy --backend=scp "$VM:C:/emma02-build/$ARTIFACT" "$OUTDIR/$ARTIFACT"
  echo "== done =="
  ls -lh "$OUTDIR/$ARTIFACT"
  exit 0
fi

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
  --exclude='/build/' \
  --exclude='Debug' --exclude='Debug_x86' \
  --exclude='*.o' --exclude='*.d' --exclude='emma_02' --exclude='*.deb' --exclude='*.rpm' \
  --exclude='.DS_Store' --exclude='**/.DS_Store' --exclude='.AppleDouble' --exclude='**/.AppleDouble' --exclude='.LSOverride' --exclude='**/.LSOverride' \
  --exclude='._*' --exclude='**/._*' \
  -e "ssh -F $SSHCFG" \
  "$REPO/" "lima-$VM:$GUEST_DIR/"

# 3. build + package inside the guest via the guest-side scripts
ARGS="$GUEST_ARGS --src $GUEST_DIR --out $GUEST_DIR"
[ "$BUILD" = "1" ] && ARGS="$ARGS --build"
[ "$CLEAN" = "1" ] && ARGS="$ARGS --clean"
[ -n "$JOBS" ] && ARGS="$ARGS --jobs $JOBS"

echo "-- building + packaging in guest ..."
GUEST_LOG="$OUTDIR/${PKG}_${ARCH}${SUFFIX}.$EXT.build.log"
mkdir -p "$OUTDIR"
# Drop any previous artifact so a failed build can never leave a stale file that
# looks like a fresh one.
rm -f "$OUTDIR/$ARTIFACT"
if ! limactl shell "$VM" -- sh -lc "cd '$GUEST_DIR' && './Pipeline/$GUEST_SCRIPT' $ARGS" > "$GUEST_LOG" 2>&1; then
  tail -n 40 "$GUEST_LOG"
  echo "ERROR: build failed in guest - full log: $GUEST_LOG" >&2
  exit 1
fi
tail -n 25 "$GUEST_LOG"
echo "guest log: $GUEST_LOG"

# 4. copy the artifact back to the Mac
mkdir -p "$OUTDIR"
limactl copy "$VM:$GUEST_DIR/$ARTIFACT" "$OUTDIR/$ARTIFACT"
echo "== done =="
ls -lh "$OUTDIR/$ARTIFACT"
