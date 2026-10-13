#!/bin/bash
# Compile Emma 02 for the right architecture (guest-side, runs on the Linux builder).
# Usage: ./build_emma02.sh --arch amd64|arm64 [--src REPO_ROOT] [--jobs N] [--clean] [--allow-wx30]
#   amd64 -> Release_x86/    arm64 -> Release/
# Verifies toolchain + wx/SDL/curl/serialport/X11 before running make, then checks ldd.
set -e
ARCH=""; SRCROOT=""; JOBS=""; CLEAN=0; ALLOW_WX30=0
while [ $# -gt 0 ]; do
  case "$1" in
    --arch) ARCH="$2"; shift 2;;
    --src) SRCROOT="$2"; shift 2;;
    --jobs) JOBS="$2"; shift 2;;
    --clean) CLEAN=1; shift;;
    --allow-wx30) ALLOW_WX30=1; shift;;
    *) echo "unknown arg: $1" >&2; exit 1;;
  esac
done
SCRIPTDIR="$(cd "$(dirname "$0")" && pwd)"
[ -n "$SRCROOT" ] || SRCROOT="$(cd "$SCRIPTDIR/.." && pwd)"
[ "$ARCH" = "amd64" ] || [ "$ARCH" = "arm64" ] || { echo "--arch amd64|arm64 required" >&2; exit 1; }
if [ "$ARCH" = "amd64" ]; then BINDIR="Release_x86"; EXPECT_UNAME="x86_64";
else BINDIR="Release"; EXPECT_UNAME="aarch64 arm64"; fi
[ -n "$JOBS" ] || JOBS="$(nproc 2>/dev/null || echo 6)"

# 1. host arch must match target (no cross-compile; use the matching builder VM)
UNAME_M="$(uname -m)"
case " $EXPECT_UNAME " in
  *" $UNAME_M "*) ;;
  *) echo "arch mismatch: target $ARCH needs ($EXPECT_UNAME) but builder is $UNAME_M" >&2
     echo "run amd64 on emma02-x64, arm64 on the ARM builder" >&2; exit 1;;
esac

# 2. writable source tree? (Lima host ~ mount is read-only from the guest)
if ! touch "$SRCROOT/.writetest" 2>/dev/null; then
  echo "source tree not writable: $SRCROOT" >&2
  echo "copy to a writable path first, e.g.:" >&2
  echo "  rsync -a --delete /Users/etxmato/workspace/emma_02/ ~/emma02-build/ --exclude=.git" >&2
  exit 1
fi
rm -f "$SRCROOT/.writetest"

# 3. toolchain
for t in g++ gcc make wx-config strip; do
  command -v "$t" >/dev/null 2>&1 || { echo "missing tool: $t" >&2; exit 1; }
done

# 4. wx version (3.2+ required; 3.0 only with --allow-wx30 for the future 22.04 cell)
WXVER="$(wx-config --release 2>/dev/null || wx-config --version 2>/dev/null || echo 0)"
WXMAJ="$(echo "$WXVER" | cut -d. -f1)"; WXMIN="$(echo "$WXVER" | cut -d. -f2)"
WXMAJ="${WXMAJ:-0}"; WXMIN="${WXMIN:-0}"
if [ "$WXMAJ" -lt 3 ] || { [ "$WXMAJ" -eq 3 ] && [ "$WXMIN" -lt 2 ] && [ "$ALLOW_WX30" = "0" ]; }; then
  echo "wxWidgets $WXVER found, need >= 3.2 for this target" >&2
  echo "install libwxgtk3.2-dev (24.04+) or rebuild wx from source" >&2; exit 1
fi
if [ "$WXMIN" -lt 2 ] && [ "$ALLOW_WX30" = "1" ]; then
  echo "WARNING: wx $WXVER build — needs GetAppearance() fallback (see packaging skill §3)" >&2
fi
echo "wx: $WXVER  arch-target: $ARCH ($BINDIR)  jobs: $JOBS"

# 5. runtime/dev libraries the link needs (actionable message on miss)
MISS=""
ldconfig -p 2>/dev/null | grep -q libSDL- >/dev/null 2>&1 || MISS="$MISS libsdl1.2-dev"
ldconfig -p 2>/dev/null | grep -q libcurl >/dev/null 2>&1 || MISS="$MISS libcurl4-openssl-dev"
ldconfig -p 2>/dev/null | grep -q libserialport >/dev/null 2>&1 || MISS="$MISS libserialport-dev"
ldconfig -p 2>/dev/null | grep -q libX11 >/dev/null 2>&1 || MISS="$MISS libx11-dev"
[ -d /usr/include/SDL ] || MISS="$MISS libsdl1.2-dev(headers)"
if [ -n "$MISS" ]; then
  echo "missing libraries:$MISS" >&2
  echo "try: sudo apt-get install -y libwxgtk3.2-dev libsdl1.2-dev libcurl4-openssl-dev libserialport-dev libx11-dev" >&2
  exit 1
fi

# 6. build
if [ "$CLEAN" = "1" ]; then
  make -C "$SRCROOT/$BINDIR" clean
fi
make -C "$SRCROOT/$BINDIR" all -j"$JOBS"

# 7. verify binary
BIN="$SRCROOT/$BINDIR/emma_02"
[ -x "$BIN" ] || { echo "build produced no binary: $BIN" >&2; exit 1; }
file "$BIN"
echo "--- ldd (wx lines) ---"
ldd "$BIN" | grep -i wx || true
if ldd "$BIN" | grep -q "not found"; then
  echo "ERROR: unresolved shared libs:" >&2
  ldd "$BIN" | grep "not found" >&2
  exit 1
fi
echo "build OK: $BIN"
