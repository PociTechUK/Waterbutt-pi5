#!/usr/bin/env zsh
set -euo pipefail
HERE="$(cd "$(dirname "$0")"; pwd)"
source "$HERE/_wb_common.sh"
LOG="$STATE_DIR/ssh_diag.log"
: > "$LOG"
set +e
echo "ATTEMPT id_ed25519 publickey" >> "$LOG"
ssh -vv -o ConnectTimeout=4 -o BatchMode=yes -o PreferredAuthentications=publickey -o IdentitiesOnly=yes -i "${HOME}/.ssh/id_ed25519" "$TARGET_SSH" "echo ok-ed25519" >>"$LOG" 2>&1
echo "RC=$?" >> "$LOG"
echo "ATTEMPT id_rsa publickey" >> "$LOG"
ssh -vv -o ConnectTimeout=4 -o BatchMode=yes -o PreferredAuthentications=publickey -o IdentitiesOnly=yes -i "${HOME}/.ssh/id_rsa" "$TARGET_SSH" "echo ok-rsa" >>"$LOG" 2>&1
echo "RC=$?" >> "$LOG"
echo "ATTEMPT password only" >> "$LOG"
ssh -vv -o ConnectTimeout=4 -o PubkeyAuthentication=no -o PreferredAuthentications=password "$TARGET_SSH" "echo ok-pass" >>"$LOG" 2>&1
echo "RC=$?" >> "$LOG"
set -e
echo "$LOG"
