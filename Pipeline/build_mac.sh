#!/bin/bash
# Build Emma 02 macOS DMG (runs on the Mac, no VM).
# Usage: ./build_mac.sh [--wx 3.3] [--deploy 12.0|10.13] [--src REPO_ROOT] [--out DIR] [--build] [--clean] [--jobs N] [--no-dmg]
#   --build: compile first via xcodebuild (universal arm64+x86_64), then package
#   --clean: clean the Xcode build folder first
#   --jobs N: parallel build jobs (default: nproc)
#   --no-dmg: stop after staging the .app (skip hdiutil), for debugging
#   --wx 3.3: only wx 3.3 supported (matches the "Emma 02" target, WXVER 3.3)
#   --deploy: macOS deployment floor, 12.0 (default, Monterey+) or 10.13
#     (High Sierra, covers 2009+ hardware; needs a wx3.3 tree built with
#     --with-macosx-version-min=10.13 - wx 3.3's floor is 10.13, no second
#     wx tree needed; launch-test on real 10.13 hardware before shipping,
#     weak-linked symbols from the modern SDK are the classic trap)
set -e
ARCH=""
WXVER="3.3"
DEPLOY="12.0"
SRCROOT=""
OUTDIR=""
BUILD=0
CLEAN=0
JOBS=""
NODMG=0
while [ $# -gt 0 ]; do
  case "$1" in
    --arch) ARCH="$2"; shift 2;;
    --wx) WXVER="$2"; shift 2;;
    --deploy) DEPLOY="$2"; shift 2;;
    --src) SRCROOT="$2"; shift 2;;
    --out) OUTDIR="$2"; shift 2;;
    --build) BUILD=1; shift;;
    --clean) CLEAN=1; shift;;
    --jobs) JOBS="$2"; shift 2;;
    --no-dmg) NODMG=1; shift;;
    *) echo "unknown arg: $1" >&2; exit 1;;
  esac
done
SCRIPTDIR="$(cd "$(dirname "$0")" && pwd)"
[ -n "$SRCROOT" ] || SRCROOT="$(cd "$SCRIPTDIR/.." && pwd)"
[ -n "$OUTDIR" ] || OUTDIR="$SRCROOT"
[ "$WXVER" = "3.3" ] || { echo "--wx 3.3 required (Emma 02 target is wx 3.3)" >&2; exit 1; }
case "$DEPLOY" in
  12.0|10.13) ;;
  *) echo "--deploy 12.0|10.13 required" >&2; exit 1;;
esac
[ -n "$JOBS" ] || JOBS="$(sysctl -n hw.ncpu 2>/dev/null || echo 8)"

CONF="$SCRIPTDIR/emma02_package.conf"
get() { grep -E "^$1=" "$CONF" | head -n1 | cut -d= -f2-; }
VERSION="$(get VERSION)"
# Artifact mirrors pipeline.sh naming (version-free, deploy suffix for 10.13).
if [ "$DEPLOY" = "10.13" ]; then
  ARTIFACT="emma_02_mac_highsierra.dmg"
else
  ARTIFACT="emma_02_mac.dmg"
fi

# Xcode needs the real Xcode.app, not the CLT stub
if ! xcodebuild -version >/dev/null 2>&1; then
  export DEVELOPER_DIR=/Applications/Xcode.app/Contents/Developer
fi
command -v xcodebuild >/dev/null || { echo "xcodebuild not found (install Xcode)" >&2; exit 1; }
xcodebuild -version >/dev/null 2>&1 || { echo "xcodebuild requires Xcode (CLT stub active)" >&2; exit 1; }

# Deployment floor: override the project's 12.0 when targeting older macOS.
# 10.13 needs a wx3.3 tree built with --with-macosx-version-min=10.13
# (WX28-style separate WXROOT is NOT needed - same wx 3.3 sources, different
# min flag); point WXPICK at that tree when DEPLOY=10.13.
if [ "$DEPLOY" = "10.13" ]; then
  echo "-- deploy floor: macOS 10.13 High Sierra (x86_64 slice; arm64 rides along)"
else
  echo "-- deploy floor: macOS 12.0 Monterey+"
fi

APP_OUT="$SRCROOT/build/Release/Emma 02.app"
STAGE="$(mktemp -d -t emma02-mac-XXXXXX)"
trap 'rm -rf "$STAGE"' EXIT

if [ "$BUILD" = "1" ]; then
  echo "-- building (xcodebuild, universal arm64+x86_64, deploy $DEPLOY) ..."
  if [ "$CLEAN" = "1" ]; then
    xcodebuild -project "$SRCROOT/Emma 02.xcodeproj" -target "Emma 02" \
      -configuration Release clean >/dev/null
  fi
  xcodebuild -project "$SRCROOT/Emma 02.xcodeproj" -target "Emma 02" \
    -configuration Release MACOSX_DEPLOYMENT_TARGET="$DEPLOY" \
    -jobs "$JOBS" build || {
    echo "ERROR: xcodebuild failed" >&2; exit 1
  }
fi

[ -d "$APP_OUT" ] || { echo "missing app bundle: $APP_OUT (build first: $0 --build)" >&2; exit 1; }

# The link line passes -lserialport with -L/usr/local/lib/serialport, but that
# dir holds both libserialport.a and libserialport.0.dylib — and the linker
# prefers the dylib. So the binary references /usr/local/lib/libserialport.0.dylib
# (absolute path, not @rpath). Embed it like the old manual bundle did:
echo "-- embedding libserialport ..."
SERIAL_SRC="/usr/local/lib/serialport/libserialport.0.dylib"
[ -f "$SERIAL_SRC" ] || { echo "missing $SERIAL_SRC" >&2; exit 1; }
FW="$APP_OUT/Contents/Frameworks"
mkdir -p "$FW"
ditto "$SERIAL_SRC" "$FW/libserialport.0.dylib"
chmod 755 "$FW/libserialport.0.dylib"
# Strip the stale i386 slice (matches the old bundle shape; keeps the DMG lean)
lipo -remove i386 "$FW/libserialport.0.dylib" -output "$FW/libserialport.0.dylib" 2>/dev/null || true
# Rewire only if still pointing at the system path (idempotent across reruns)
if otool -L "$APP_OUT/Contents/MacOS/Emma 02" | grep -q "/usr/local/lib/libserialport"; then
  install_name_tool -change /usr/local/lib/libserialport.0.dylib \
    @rpath/libserialport.0.dylib "$APP_OUT/Contents/MacOS/Emma 02"
fi
# Re-seal after rewiring (ad-hoc, matching CODE_SIGN_IDENTITY = "-")
codesign --force --deep --sign - "$APP_OUT" >/dev/null 2>&1 || true

echo "-- verifying bundle ..."
lipo -info "$APP_OUT/Contents/MacOS/Emma 02" | grep -q "x86_64" || { echo "binary missing x86_64 slice" >&2; exit 1; }
lipo -info "$APP_OUT/Contents/MacOS/Emma 02" | grep -q "arm64" || { echo "binary missing arm64 slice" >&2; exit 1; }
# Confirm the deploy floor landed in the binary (both slices carry LC_BUILD_VERSION minos)
MINOS="$(otool -l "$APP_OUT/Contents/MacOS/Emma 02" | grep -A3 "LC_BUILD_VERSION" | grep minos | sort -u)"
echo "-- minos: $MINOS"
MISSING="$(otool -L "$APP_OUT/Contents/MacOS/Emma 02" | sed 's/^[[:space:]]*//' | grep -v -e ":$" -e "^/System/" -e "^/usr/lib/" -e "@rpath" -e "@executable_path" -e "^$" || true)"
[ -z "$MISSING" ] || { echo "ERROR: non-system dylib linkage (not bundled): $MISSING" >&2; exit 1; }
codesign --verify --deep --strict "$APP_OUT" 2>&1 | head -5 || true

if [ "$NODMG" = "1" ]; then
  echo "-- staged (no dmg): $APP_OUT"
  exit 0
fi

echo "-- staging DMG ..."
DMGSTAGE="$STAGE/dmg"
mkdir -p "$DMGSTAGE"
# Copy the bundle (ditto preserves symlinks, resource forks, code signature)
ditto "$APP_OUT" "$DMGSTAGE/Emma 02.app"
# Drag-install target: symlink to /Applications so Finder shows the
# "drag Emma 02 to Applications" affordance
ln -s /Applications "$DMGSTAGE/Applications"
# Stamp the bundle version from the package config (the Xcode project still
# hardcodes the last release; the DMG volume name carries the live VERSION)
PLIST="$DMGSTAGE/Emma 02.app/Contents/Info.plist"
if command -v plutil >/dev/null; then
  plutil -replace CFBundleShortVersionString -string "$VERSION" "$PLIST" >/dev/null
  plutil -replace CFBundleGetInfoString -string "Emma 02 version $VERSION, (c) 2007-2025 Marcel van Tongeren" "$PLIST" >/dev/null
  plutil -replace CFBundleLongVersionString -string "$VERSION, (c) 2007-2025 Marcel van Tongeren" "$PLIST" >/dev/null
fi
# Re-seal the bundle after the plist edit (ad-hoc, matching the project setting)
codesign --force --deep --sign - "$DMGSTAGE/Emma 02.app" >/dev/null 2>&1 || true
# Purge macOS metadata that must not ship
find "$DMGSTAGE" \( -name ".DS_Store" -o -name ".AppleDouble" -o -name ".LSOverride" -o -name "._*" \) -delete

DMG="$OUTDIR/$ARTIFACT"
mkdir -p "$OUTDIR"
# Drop any previous artifact so a failed build can never leave a stale file
# that looks like a fresh one.
rm -f "$DMG"
hdiutil create -volname "Emma 02 $VERSION" -srcfolder "$DMGSTAGE" \
  -ov -format UDZO "$DMG" >/dev/null
echo "== done =="
ls -lh "$DMG"
