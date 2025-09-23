#!/usr/bin/env zsh
set -euo pipefail
REPO_ROOT="$(cd "$(dirname "$0")/../.."; pwd)"
TARGET_IP="$(cat "$REPO_ROOT/.wb/host" | tr -d '[:space:]')"
WB_USER="${WB_USER:-poci}"
ssh "$WB_USER@$TARGET_IP" "hostname; uptime; hostname -I"
