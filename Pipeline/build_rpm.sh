#!/bin/bash
# Build Emma 02 .rpm from Pipeline/emma02_package.conf (single source of truth)
# Runs on the Linux builder (Lima VM), not on macOS.
#
# Same approach as build_deb.sh: the [files] payload is staged and shipped directly.
# The legacy epm path (data.tar + mv_files/extract_files %postinstall) is retired
# and must not be brought back — this is a plain rpmbuild of the staged tree.
#
# Usage: ./build_rpm.sh --arch amd64|arm64 --distro fedora|suse [--wx 3.2|3.1] [--src REPO_ROOT] [--out DIR] [--build] [--clean] [--jobs N]
#   --build: compile first via build_emma02.sh (correct libs + Release dir per arch), then package
#   --distro fedora|suse: selects the builder distro family and the artifact name
#   --wx: wx ABI of the target distro — fedora is 3.2 only; suse 3.2 (Leap 16,
#         DEPENDS_RPM_SUSE) or 3.1 (Leap 15.6, DEPENDS_RPM_SUSE_WX31, _leap156 suffix)
#   --sign-key KEYID / --no-sign: override the conf's RPM_SIGN_KEY_ID for this run
#   --gnupghome DIR: keyring to sign with (default <repo>/Pipeline/signing/gnupg)
#   --passphrase-file FILE: passphrase for a protected key; the file is deleted
#         after signing. rpm 4.14 ignores %_gpg_passphrase, so the passphrase is
#         fed to gpg through %_gpg_sign_cmd_extra_args instead.
set -e
ARCH=""
DISTRO=""
WXVER="3.2"
SRCROOT=""
OUTDIR=""
BUILD=0
CLEAN=0
JOBS=""
SIGN_KEY=""
SIGN=1
GNUPGHOME_ARG=""
PASS_FILE=""
while [ $# -gt 0 ]; do
  case "$1" in
    --arch) ARCH="$2"; shift 2;;
    --distro) DISTRO="$2"; shift 2;;
    --wx) WXVER="$2"; shift 2;;
    --src) SRCROOT="$2"; shift 2;;
    --out) OUTDIR="$2"; shift 2;;
    --build) BUILD=1; shift;;
    --clean) CLEAN=1; shift;;
    --jobs) JOBS="$2"; shift 2;;
    --sign-key) SIGN_KEY="$2"; shift 2;;
    --gnupghome) GNUPGHOME_ARG="$2"; shift 2;;
    --passphrase-file) PASS_FILE="$2"; shift 2;;
    --no-sign) SIGN=0; shift;;
    *) echo "unknown arg: $1" >&2; exit 1;;
  esac
done
cleanup() { [ -z "$PASS_FILE" ] || rm -f "$PASS_FILE"; }
# NOTE: bash keeps only one EXIT trap, so the WORK cleanup below must call this
# function too rather than install a second trap.
SCRIPTDIR="$(cd "$(dirname "$0")" && pwd)"
[ -n "$SRCROOT" ] || SRCROOT="$(cd "$SCRIPTDIR/.." && pwd)"
[ -n "$OUTDIR" ] || OUTDIR="$SRCROOT"
[ "$ARCH" = "amd64" ] || [ "$ARCH" = "arm64" ] || { echo "--arch amd64|arm64 required" >&2; exit 1; }
WX_SUFFIX=""
case "$DISTRO" in
  fedora)
    [ "$WXVER" = "3.2" ] || { echo "fedora rpm is wx 3.2 only (Fedora ships no wx 3.0/3.1)" >&2; exit 1; }
    DEPS_KEY="DEPENDS_RPM_FEDORA"; DISTRO_SUFFIX="-fedora"; OBSOLETES="emma02_fedora emma02_fedora_x86";;
  suse)
    case "$WXVER" in
      3.2) DEPS_KEY="DEPENDS_RPM_SUSE";      WX_SUFFIX="16";;
      3.1) DEPS_KEY="DEPENDS_RPM_SUSE_WX31"; WX_SUFFIX="156";;
      *) echo "--wx 3.1|3.2 required for suse rpm" >&2; exit 1;;
    esac
    DISTRO_SUFFIX="-leap"; OBSOLETES="emma02_suse emma02_suse_x86";;
  *) echo "--distro fedora|suse required" >&2; exit 1;;
esac
SUFFIX="${DISTRO_SUFFIX}${WX_SUFFIX}"
if [ "$ARCH" = "amd64" ]; then BIN="Release_x86"; RPMARCH="x86_64"; NAMEARCH="amd64"; else BIN="Release"; RPMARCH="aarch64"; NAMEARCH="aarch64"; fi

command -v rpmbuild >/dev/null || { echo "rpmbuild not found (install rpm-build)" >&2; exit 1; }

if [ "$BUILD" = "1" ]; then
  BUILD_ARGS="--arch $ARCH --src $SRCROOT"
  [ -n "$JOBS" ] && BUILD_ARGS="$BUILD_ARGS --jobs $JOBS"
  [ "$CLEAN" = "1" ] && BUILD_ARGS="$BUILD_ARGS --clean"
  [ "$WXVER" != "3.2" ] && BUILD_ARGS="$BUILD_ARGS --allow-wx $WXVER"
  "$SCRIPTDIR/build_emma02.sh" $BUILD_ARGS
fi

CONF="$SCRIPTDIR/emma02_package.conf"
get() { grep -E "^$1=" "$CONF" | head -n1 | cut -d= -f2-; }
PACKAGE="$(get PACKAGE)"
VERSION="$(get VERSION)"
MAINTAINER="$(get MAINTAINER)"
HOMEPAGE="$(get HOMEPAGE)"
LICENSE="$(get LICENSE)"
SIGN_CONF="$(get RPM_SIGN_KEY_ID)"
[ -n "$SIGN_KEY" ] || SIGN_KEY="$SIGN_CONF"
REQUIRES="$(get $DEPS_KEY)"
DESC_SHORT="$(get DESC_SHORT)"
DESC_LONG="$(get DESC_LONG)"
[ -n "$REQUIRES" ] || { echo "missing $DEPS_KEY in $CONF" >&2; exit 1; }

[ -x "$SRCROOT/$BIN/emma_02" ] || { echo "missing binary: $SRCROOT/$BIN/emma_02 (build first: make -C $BIN)" >&2; exit 1; }

WORK="$(mktemp -d)"
trap 'cleanup; rm -rf "$WORK"' EXIT
TOP="$WORK/rpmbuild"
STAGE="$TOP/stage"
mkdir -p "$TOP/BUILD" "$TOP/BUILDROOT" "$TOP/RPMS" "$TOP/SOURCES" "$TOP/SPECS" "$TOP/SRPMS" "$STAGE"

# payload from [files] section (identical staging as build_deb.sh)
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
  mkdir -p "$STAGE$dst"
  cp -a "$full" "$STAGE$dst/"
done < "$CONF"

# desktop entry (parity with .dbp <<MENU>> and build_deb.sh)
mkdir -p "$STAGE/usr/share/applications"
cp -a "$SRCROOT/Pipeline/emma_02.desktop" "$STAGE/usr/share/applications/emma_02.desktop"

# --- one-release compatibility shim: CyberVision ROM filename case change ---
# The previous payload shipped .../data/Cybervision/Cybervision2001.bin (capital
# C) while the XMLs ask for cybervision2001.bin (lowercase). Both names must be in
# the payload for one release so that an upgrade of the OLD package can still
# remove its recorded capital-C entry: that entry is deleted AFTER the new files
# are laid down, and it would otherwise fail with
#   "file /usr/share/emma_02/data/Cybervision/Cybervision2001.bin: remove failed:
#    No such file or directory".
# Generated here (not as a second repo file) because a case-only duplicate cannot
# exist in the macOS checkout - the payload is staged on the case-sensitive
# builder VM. Guarded so a future re-sync of the real uppercase name is a no-op;
# remove this block once no install predating the rename is being upgraded.
CV_CAP="$STAGE/usr/share/emma_02/data/Cybervision/Cybervision2001.bin"
CV_LOW="$STAGE/usr/share/emma_02/data/Cybervision/cybervision2001.bin"
if [ -f "$CV_LOW" ] && [ ! -e "$CV_CAP" ]; then
  install -D -m 644 "$CV_LOW" "$CV_CAP"
fi

# strip binary like Debreate <<BUILD>> strip option
strip --strip-unneeded "$STAGE/usr/bin/emma_02" 2>/dev/null || true
chmod 755 "$STAGE/usr/bin/emma_02"

esc() { printf '%s' "$1" | sed 's/%/%%/g'; }

# spec generated from the conf + the staged tree; payload ships directly
# (no data.tar, no mv_files/extract_files %postinstall stage)
SPEC="$TOP/SPECS/$PACKAGE.spec"
{
  # prebuilt binary: no debuginfo subpackage, no brp strip/post scripts
  echo '%global debug_package %{nil}'
  echo '%global __os_install_post %{nil}'
  echo "Name: $PACKAGE"
  echo "Version: $VERSION"
  echo "Release: 1"
  echo "Summary: $(esc "$DESC_SHORT")"
  echo "License: $(esc "$LICENSE")"
  echo "Vendor: $(esc "$MAINTAINER")"
  echo "URL: $HOMEPAGE"
  echo "BuildArch: $RPMARCH"
  echo "Obsoletes: $OBSOLETES"
  echo ""
  # explicit runtime deps; rpmbuild adds soname-level requires from the binary
  echo "$REQUIRES" | tr ',' '\n' | sed 's/^ *//;s/ *$//' | grep -v '^$' | while read -r r; do
    echo "Requires: $r"
  done
  echo ""
  echo "%description"
  esc "$DESC_LONG"
  echo ""
  echo "%install"
  echo 'rm -rf %{buildroot}'
  echo 'mkdir -p %{buildroot}'
  echo 'cp -a %{_topdir}/stage/. %{buildroot}/'
  echo ""
  echo '%pre'
  # parity with the .deb preinst: clear legacy epm-era leftovers
  echo 'if test -d /usr/share/emma_02/data; then rm -r /usr/share/emma_02/data; fi'
  echo 'if test -d /usr/share/emma_02/Configurations; then rm -r /usr/share/emma_02/Configurations; fi'
  echo ""
  echo '%files'
  echo '%defattr(-,root,root,-)'
  # Claim our own tree by directory: rpm enumerates its contents itself, so payload
  # names containing spaces, brackets, commas or parentheses never appear in the spec.
  # That matters because rpm 4.14 (openSUSE Leap 15) cannot reference such names in a
  # per-file entry at all: quoted entries are still split on whitespace, backslash
  # escaping is rejected, and `%files -f` splits too (verified on rpm 4.14.3 and 4.20.1).
  # Everything we install outside that tree is listed per file, and must be name-safe.
  echo '/usr/share/emma_02'
  ( cd "$STAGE" && find . -mindepth 1 \( -type f -o -type l \) -not -path './usr/share/emma_02/*' -printf '/%P\n' | sort )
} > "$SPEC"

# guard: a non-rpm-safe name outside our own tree would break rpm 4.14 builds
UNSAFE="$(cd "$STAGE" && find . -mindepth 1 \( -type f -o -type l \) -not -path './usr/share/emma_02/*' -printf '%P\n' | grep -E '[][ ,(){}*?\\"]' || true)"
if [ -n "$UNSAFE" ]; then
  echo "payload outside /usr/share/emma_02 needs rpm-safe names (spaces/brackets/commas" >&2
  echo "break rpm 4.14 %files parsing) - move them under /usr/share/emma_02 or rename:" >&2
  echo "$UNSAFE" >&2
  exit 1
fi

# normalise staged modes (see build_deb.sh): a checkout with restrictive modes would
# otherwise ship unreadable data, since git only tracks the executable bit.
chmod -R a+rX "$STAGE"

# keep the generated spec next to the artifact: cheap reproducibility/debug aid
cp -a "$SPEC" "$OUTDIR/emma_02-linux-${NAMEARCH}${SUFFIX}.spec"

rpmbuild -bb --define "_topdir $TOP" "$SPEC"

RPM="$(find "$TOP/RPMS" -type f -name '*.rpm' | head -n1)"
[ -n "$RPM" ] || { echo "rpmbuild produced no rpm" >&2; exit 1; }

# --- optional GPG signing (rpm --addsign) ---------------------------------
# Without a signature openSUSE YaST reports the package as broken:
#   "Package emma-02 is broken, integrity check has failed."
# Signing turns that into a one-time "new signing key" prompt for users who
# import the public key (rpm --import emma_02_rpm_signing.asc).
if [ "$SIGN" = "1" ] && [ -n "$SIGN_KEY" ]; then
  GNUPGHOME="${GNUPGHOME_ARG:-$SCRIPTDIR/signing/gnupg}"
  [ -d "$GNUPGHOME" ] || { echo "signing keyring not found: $GNUPGHOME (create it with Pipeline/create_rpm_signing_key.sh, or pass --no-sign)" >&2; exit 1; }
  # rpm >= 6 (Fedora 44) no longer ships rpmsign in the rpm package; --addsign
  # execs /usr/bin/rpmsign, so without the rpm-sign package it fails with
  # "rpm: /usr/bin/rpmsign: No such file or directory".
  command -v rpmsign >/dev/null 2>&1 || {
    echo "ERROR: rpmsign not found (Fedora 6.x rpm needs: dnf install rpm-sign)" >&2
    exit 1
  }
  export GNUPGHOME
  SIGN_ARGS=(--define "_gpg_name $SIGN_KEY")
  if [ -n "$PASS_FILE" ]; then
    [ -f "$PASS_FILE" ] || { echo "passphrase file not found: $PASS_FILE" >&2; exit 1; }
    # rpm 4.14's %__gpg_sign_cmd has no passphrase support; appending these
    # flags makes gpg read the passphrase from the file (loopback pinentry).
    SIGN_ARGS+=(--define "_gpg_sign_cmd_extra_args --batch --pinentry-mode loopback --passphrase-file $PASS_FILE")
  fi
  echo "--- signing $PACKAGE with $SIGN_KEY ..."
  SIGN_LOG="$WORK/sign.log"
  if ! rpm "${SIGN_ARGS[@]}" --addsign "$RPM" > "$SIGN_LOG" 2>&1; then
    sed 's/^/    /' "$SIGN_LOG" >&2
    echo "ERROR: rpm --addsign failed (keyring $GNUPGHOME, passphrase source ${PASS_FILE:-none})" >&2
    exit 1
  fi
elif [ "$SIGN" = "0" ] || [ -z "$SIGN_KEY" ]; then
  echo "--- signing disabled (unsigned rpm) ..."
fi

# Version-free artifact name; the version lives in the v<version> folder and the Version: tag.
# Naming (2026-10-25): emma_02-<os>-<arch>[-<variant>].<ext> (rpm uses aarch64)
DEST="$OUTDIR/emma_02-linux-${NAMEARCH}${SUFFIX}.rpm"
cp -a "$RPM" "$DEST"
echo "built: $DEST"
rpm -qp --qf 'Package: %{NAME}\nVersion: %{VERSION}-%{RELEASE}\nArchitecture: %{ARCH}\n' "$DEST"
echo "Requires:"
rpm -qp --requires "$DEST" | sed 's/^/  /'
if [ "$SIGN" = "1" ] && [ -n "$SIGN_KEY" ]; then
  echo "Signature:"
  rpm -Kv "$DEST" | sed 's/^/  /'
  # The builder normally does not import our public key, so rpm reports the
  # signature as "NOKEY" - that still means the signature is present. Only
  # require a signature line plus intact digests; a missing signature line is
  # the real failure.
  # rpm 4.x prints "Header V4 RSA/SHA256 Signature, key ID", rpm 6.x prints
  # "Header OpenPGP V4 RSA/SHA512 signature, key ID" - match case-insensitively.
  rpm -Kv "$DEST" | grep -qi 'signature, key ID' || { echo "ERROR: $DEST is not signed" >&2; exit 1; }
  rpm -Kv "$DEST" | grep -q 'digest: OK' || { echo "ERROR: $DEST failed its digest check" >&2; exit 1; }
fi
