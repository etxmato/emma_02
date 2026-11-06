#!/bin/bash
# Create the Emma 02 RPM signing key A ONCE, in a dedicated keyring that the
# pipeline can use without touching the user's personal ~/.gnupg.
#
#   ./Pipeline/create_rpm_signing_key.sh [--uid "Name <email>"] [--bits 3072] [--passphrase FILE]
#
# Outputs:
#   Pipeline/signing/gnupg/                       private keyring (gitignored)
#   Pipeline/signing/emma_02_rpm_signing.asc      public key to publish for users
#
# The key is unprotected by default so an unattended pipeline can sign without a
# passphrase. Pass --passphrase FILE (a file containing the passphrase) to protect
# it; build_rpm.sh then needs the same passphrase via EMMA02_RPM_GPG_PASSPHRASE,
# which makes unattended signing harder. A release signing key usually wants
# protection, so choose deliberately. Back up Pipeline/signing/gnupg: without it
# you cannot sign updates with the same key and users would have to import a new
# one.
set -e

SCRIPTDIR="$(cd "$(dirname "$0")" && pwd)"
UID_NAME="Emma 02 RPM Signing (openSUSE/Fedora packages) <rpm@emma02.hobby-site.com>"
BITS="3072"
PASSPHRASE_FILE=""
KEYCHAIN_SERVICE=""
while [ $# -gt 0 ]; do
  case "$1" in
    --uid) UID_NAME="$2"; shift 2;;
    --bits) BITS="$2"; shift 2;;
    --passphrase) PASSPHRASE_FILE="$2"; shift 2;;
    --passphrase-keychain) KEYCHAIN_SERVICE="$2"; shift 2;;
    *) echo "unknown arg: $1" >&2; exit 1;;
  esac
done
[ -z "$PASSPHRASE_FILE" ] || [ -z "$KEYCHAIN_SERVICE" ] || { echo "use --passphrase or --passphrase-keychain, not both" >&2; exit 1; }


command -v gpg >/dev/null || { echo "gpg not found" >&2; exit 1; }

SIGN_DIR="$SCRIPTDIR/signing"
GNUPGHOME_DIR="$SIGN_DIR/gnupg"
PUBKEY="$SIGN_DIR/emma_02_rpm_signing.asc"
mkdir -p "$SIGN_DIR"
chmod 700 "$SIGN_DIR"

if [ -d "$GNUPGHOME_DIR" ] && GNUPGHOME="$GNUPGHOME_DIR" gpg --list-secret-keys 2>/dev/null | grep -q '^sec'; then
  echo "ERROR: $GNUPGHOME_DIR already holds a secret key - refusing to overwrite." >&2
  echo "Delete it deliberately, or keep using the existing key." >&2
  exit 1
fi

mkdir -p "$GNUPGHOME_DIR"
chmod 700 "$GNUPGHOME_DIR"
export GNUPGHOME="$GNUPGHOME_DIR"

# A passphrase can come from a file or from a macOS Keychain item. Reading it
# from the Keychain keeps it out of the repo and out of shell history; it is
# materialised into a 600 temp file only for the duration of this script.
TEMP_PASS=""
cleanup() { [ -z "$TEMP_PASS" ] || rm -f "$TEMP_PASS"; }
trap cleanup EXIT
if [ -n "$KEYCHAIN_SERVICE" ]; then
  command -v security >/dev/null || { echo "security (macOS Keychain) not found" >&2; exit 1; }
  PASS="$(security find-generic-password -s "$KEYCHAIN_SERVICE" -w 2>/dev/null)" || {
    echo "no Keychain item for service '$KEYCHAIN_SERVICE' (add it with security add-generic-password)" >&2; exit 1; }
  [ -n "$PASS" ] || { echo "Keychain item '$KEYCHAIN_SERVICE' is empty" >&2; exit 1; }
  TEMP_PASS="$(mktemp)"
  chmod 600 "$TEMP_PASS"
  printf '%s' "$PASS" > "$TEMP_PASS"
  unset PASS
  PASSPHRASE_FILE="$TEMP_PASS"
fi

GPG_ARGS=(--batch --pinentry-mode loopback)
if [ -n "$PASSPHRASE_FILE" ]; then
  [ -f "$PASSPHRASE_FILE" ] || { echo "passphrase file not found: $PASSPHRASE_FILE" >&2; exit 1; }
  GPG_ARGS+=(--passphrase-file "$PASSPHRASE_FILE")
else
  GPG_ARGS+=(--passphrase '')
fi

gpg "${GPG_ARGS[@]}" --quick-gen-key "$UID_NAME" rsa"$BITS" sign 0

gpg --armor --export "$UID_NAME" > "$PUBKEY"
chmod 644 "$PUBKEY"

KEYID="$(gpg --list-secret-keys --with-colons "$UID_NAME" 2>/dev/null | awk -F: '/^sec:/{print $5; exit}')"
echo "Created signing key."
echo "  uid:     $UID_NAME"
echo "  key id:  $KEYID"
echo "  keyring: $GNUPGHOME_DIR"
echo "  pubkey:  $PUBKEY"
echo ""
echo "Next: set RPM_SIGN_KEY_ID=$KEYID in Pipeline/emma02_package.conf."
echo "Publish $PUBKEY so users can verify packages (rpm --import <pubkey>)."
