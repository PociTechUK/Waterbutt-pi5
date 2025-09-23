#!/usr/bin/env zsh
set -euo pipefail
REPO_ROOT="$(cd "$(dirname "$0")/../.."; pwd)"
STATE_DIR="$REPO_ROOT/.wb"
HOST_FILE="$STATE_DIR/host"
mkdir -p "$STATE_DIR"
[[ -f "$HOST_FILE" ]] || { echo "Run scripts/mac/wb_login.sh first"; exit 1; }
TARGET_IP="$(cat "$HOST_FILE" | tr -d '[:space:]')"
[[ -n "$TARGET_IP" && "$TARGET_IP" != "unset" ]] || { echo "Run scripts/mac/wb_login.sh first"; exit 1; }
DEST_USER="${WB_USER:-pi}"
DEST="$DEST_USER@$TARGET_IP"
KEY="${HOME}/.ssh/id_ed25519.pub"
[[ -f "$KEY" ]] || KEY="${HOME}/.ssh/id_rsa.pub"
if [[ ! -f "$KEY" ]]; then
  ssh-keygen -t ed25519 -N "" -f "${HOME}/.ssh/id_ed25519"
  KEY="${HOME}/.ssh/id_ed25519.pub"
fi
ssh -o ConnectTimeout=6 -o StrictHostKeyChecking=accept-new "$DEST" 'mkdir -p ~/.ssh && chmod 700 ~/.ssh && touch ~/.ssh/authorized_keys && chmod 600 ~/.ssh/authorized_keys'
ssh -o ConnectTimeout=6 "$DEST" 'cat >> ~/.ssh/authorized_keys' < "$KEY"
ssh -o ConnectTimeout=6 "$DEST" 'sort -u ~/.ssh/authorized_keys -o ~/.ssh/authorized_keys'
