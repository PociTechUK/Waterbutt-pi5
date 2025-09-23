#!/usr/bin/env zsh
set -euo pipefail
REPO_ROOT="$(cd "$(dirname "$0")/../.."; pwd)"
STATE_DIR="$REPO_ROOT/.wb"
HOST_FILE="$STATE_DIR/host"
mkdir -p "$STATE_DIR"
IP55="192.168.9.55"
IP56="192.168.9.56"
echo
echo "Choose target:"
echo "1) $IP55"
echo "2) $IP56"
read "?Select [1/2 or .55/.56]: " choice
case "$choice" in
  1|".55"|$IP55) sel="$IP55" ;;
  2|".56"|$IP56) sel="$IP56" ;;
  *) echo "Invalid"; exit 1 ;;
esac
echo "$sel" > "$HOST_FILE"
sync
echo "Selected target: $sel"
