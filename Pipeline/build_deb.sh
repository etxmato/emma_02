#!/bin/bash
# Build Emma 02 .deb from Pipeline/emma02_package.conf (single source of truth)
# Runs on the Linux builder (Lima VM), not on macOS.
# Usage: ./build_deb.sh --arch amd64|arm64 [--wx 3.0|3.2] [--src REPO_ROOT] [--out DIR] [--build] [--clean] [--jobs N]
#   --build: compile first via build_emma02.sh (correct libs + Release dir per arch), then package
#   --wx 3.0: build/package the wxWidgets 3.0 variant (Ubuntu 22.04); Depends from DEPENDS_WX30
set -e
ARCH=""
WXVER="3.2"
SRCROOT=""
OUTDIR=""
BUILD=0
CLEAN=0
JOBS=""
while [ $# -gt 0 ]; do
  case "$1" in
    --arch) ARCH="$2"; shift 2;;
    --wx) WXVER="$2"; shift 2;;
    --src) SRCROOT="$2"; shift 2;;
    --out) OUTDIR="$2"; shift 2;;
    --build) BUILD=1; shift;;
    --clean) CLEAN=1; shift;;
    --jobs) JOBS="$2"; shift 2;;
    *) echo "unknown arg: $1" >&2; exit 1;;
  esac
done
SCRIPTDIR="$(cd "$(dirname "$0")" && pwd)"
[ -n "$SRCROOT" ] || SRCROOT="$(cd "$SCRIPTDIR/.." && pwd)"
[ -n "$OUTDIR" ] || OUTDIR="$SRCROOT"
[ "$ARCH" = "amd64" ] || [ "$ARCH" = "arm64" ] || { echo "--arch amd64|arm64 required" >&2; exit 1; }
[ "$WXVER" = "3.0" ] || [ "$WXVER" = "3.2" ] || { echo "--wx 3.0|3.2 required" >&2; exit 1; }
if [ "$BUILD" = "1" ]; then
  BUILD_ARGS="--arch $ARCH --src $SRCROOT"
  [ -n "$JOBS" ] && BUILD_ARGS="$BUILD_ARGS --jobs $JOBS"
  [ "$CLEAN" = "1" ] && BUILD_ARGS="$BUILD_ARGS --clean"
  [ "$WXVER" = "3.0" ] && BUILD_ARGS="$BUILD_ARGS --allow-wx 3.0"
  "$SCRIPTDIR/build_emma02.sh" $BUILD_ARGS
fi

CONF="$SCRIPTDIR/emma02_package.conf"
get() { grep -E "^$1=" "$CONF" | head -n1 | cut -d= -f2-; }
PACKAGE="$(get PACKAGE)"
VERSION="$(get VERSION)"
MAINTAINER="$(get MAINTAINER)"
HOMEPAGE="$(get HOMEPAGE)"
if [ "$WXVER" = "3.0" ]; then
  DEPENDS="$(get DEPENDS_WX30)"
  SUFFIX="-legacy"
else
  DEPENDS="$(get DEPENDS)"
  SUFFIX=""
fi
DESC_SHORT="$(get DESC_SHORT)"
DESC_LONG="$(get DESC_LONG)"
if [ "$ARCH" = "amd64" ]; then BIN="Release_x86"; DEBARCH="amd64";
else BIN="Release"; DEBARCH="arm64"; fi

[ -x "$SRCROOT/$BIN/emma_02" ] || { echo "missing binary: $SRCROOT/$BIN/emma_02 (build first: make -C $BIN)" >&2; exit 1; }

STAGE="$(mktemp -d)"
trap 'rm -rf "$STAGE"' EXIT
mkdir -p "$STAGE/DEBIAN"

# control
{
  echo "Package: $PACKAGE"
  echo "Version: $VERSION"
  echo "Architecture: $DEBARCH"
  echo "Maintainer: $MAINTAINER"
  echo "Homepage: $HOMEPAGE"
  echo "Priority: standard"
  echo "Depends: $DEPENDS"
  echo "Description: $DESC_SHORT"
  echo " $DESC_LONG"
} > "$STAGE/DEBIAN/control"

# preinst (parity with ubuntu_build_*.dbp <<PREINST>>)
cat > "$STAGE/DEBIAN/preinst" <<'EOF'
#!/bin/sh
set -e
if test -d /usr/share/emma_02/data; then rm -r /usr/share/emma_02/data; fi
if test -d /usr/share/emma_02/Configurations; then rm -r /usr/share/emma_02/Configurations; fi
EOF
chmod 755 "$STAGE/DEBIAN/preinst"

# payload from [files] section
infiles=0
while IFS= read -r line; do
  case "$line" in
    '[files]') infiles=1; continue;;
    '['*) infiles=0; continue;;
  esac
  [ "$infiles" = "1" ] || continue
  case "$line" in ''|'#'*) continue;; esac
  src="${line%% -> *}"; dst="${line##* -> }"
  src="$(echo "$src" | sed 's/^ *//;s/ *$//')"
  dst="$(echo "$dst" | sed 's/^ *//;s/ *$//')"
  # expand {BIN}
  case "$src" in '{BIN}/'*) src="$BIN/${src#'{BIN}'/}";; esac
  full="$SRCROOT/$src"
  [ -e "$full" ] || { echo "missing payload source: $full" >&2; exit 1; }
  if [ -d "$full" ]; then
    mkdir -p "$STAGE$dst"
    cp -a "$full" "$STAGE$dst/"
  else
    mkdir -p "$STAGE$dst"
    cp -a "$full" "$STAGE$dst/"
  fi
done < "$CONF"

# desktop entry (parity with .dbp <<MENU>>)
mkdir -p "$STAGE/usr/share/applications"
cp -a "$SRCROOT/Pipeline/emma_02.desktop" "$STAGE/usr/share/applications/emma_02.desktop"

# --- one-release compatibility shim: CyberVision ROM filename case change ---
# See the identical block in build_rpm.sh for the full rationale: the lower-case
# name is what the XMLs ask for, the capital-C name is kept in the payload for one
# release so an upgrade of an older package can still remove its recorded entry.
# Generated here (not as a second repo file) because a case-only duplicate cannot
# exist in the macOS checkout; the payload is staged on the case-sensitive builder.
CV_CAP="$STAGE/usr/share/emma_02/data/Cybervision/Cybervision2001.bin"
CV_LOW="$STAGE/usr/share/emma_02/data/Cybervision/cybervision2001.bin"
if [ -f "$CV_LOW" ] && [ ! -e "$CV_CAP" ]; then
  install -D -m 644 "$CV_LOW" "$CV_CAP"
fi

# strip binary like Debreate <<BUILD>> strip option
strip --strip-unneeded "$STAGE/usr/bin/emma_02" 2>/dev/null || true
chmod 755 "$STAGE/usr/bin/emma_02"

# Version-free artifact name; the version lives in the v<version> folder and the Version: field.
# normalise staged modes: the source checkout can carry restrictive modes (600/700)
# that rsync -a copies through, which would ship unreadable data. git only tracks the
# executable bit, so a polluted checkout is invisible in the commit.
chmod -R a+rX "$STAGE"

# Naming (2026-10-25): emma_02-<os>-<arch>[-<variant>].<ext>
DEB="$OUTDIR/emma_02-linux-${DEBARCH}${SUFFIX}.deb"
dpkg-deb --build "$STAGE" "$DEB"
echo "built: $DEB"
dpkg-deb -f "$DEB" Package Version Architecture Depends
