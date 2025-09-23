#!/bin/bash
set -euo pipefail
if [[ -f Makefile ]]; then
  make || true
fi
if [[ -x ./bin/wb_app ]]; then
  exec ./bin/wb_app
fi
if [[ -x ./run ]]; then
  exec ./run
fi
echo "Nothing to execute (looked for ./bin/wb_app, then ./run)"; exit 1
