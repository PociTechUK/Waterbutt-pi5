#!/usr/bin/env zsh
set -euo pipefail
REPO_ROOT="$(cd "$(dirname "$0")/../.."; pwd)"
STATE_DIR="$REPO_ROOT/.wb"
HOST_FILE="$STATE_DIR/host"
mkdir -p "$STATE_DIR"
if [[ ! -f "$HOST_FILE" ]]; then
  echo "unset" > "$HOST_FILE"
fi
TARGET_IP="$(cat "$HOST_FILE" | tr -d '[:space:]')"
if [[ "$TARGET_IP" == "unset" || -z "$TARGET_IP" ]]; then
  echo "No target selected. Run: scripts/mac/wb_login.sh"
  exit 1
fi
WB_USER_DEFAULT="pi"
WB_USER="${WB_USER:-$WB_USER_DEFAULT}"
TARGET_SSH="$WB_USER@$TARGET_IP"
SSH_KEY="${HOME}/.ssh/id_ed25519"
[[ -f "$SSH_KEY" ]] || SSH_KEY="${HOME}/.ssh/id_rsa"
SSH_OPTS=(-o ConnectTimeout=4 -o BatchMode=yes -o PreferredAuthentications=publickey -o IdentitiesOnly=yes -o StrictHostKeyChecking=accept-new -i "$SSH_KEY")
export REPO_ROOT STATE_DIR HOST_FILE TARGET_IP TARGET_SSH WB_USER SSH_KEY SSH_OPTS
