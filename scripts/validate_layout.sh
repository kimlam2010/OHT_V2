#!/bin/bash
# Validate OHT-50 repository layout (no nested OHT-50 directories, canonical tree)

set -e

fail() {
  echo "[FAIL] $1" >&2
  exit 1
}

warn() {
  echo "[WARN] $1" >&2
}

echo "[INFO] Validating repository layout..."

# 1) No nested OHT-50 directories (physical directories), allow mentions in docs/examples
if find . -type d -path "*/OHT-50/OHT-50*" | grep -q .; then
  echo "[ERROR] Detected nested directories:"
  find . -type d -path "*/OHT-50/OHT-50*"
  fail "Nested OHT-50 directories detected (OHT-50/OHT-50). Please remove duplicates."
fi

# 2) Canonical top-level directories should exist
for d in backend firmware deploy docs tools frontend; do
  if [ ! -d "$d" ]; then
    warn "Missing expected top-level directory: $d"
  fi
done

# 3) No nginx configs allowed in deploy/
if grep -R "nginx" deploy/ -n >/dev/null 2>&1; then
  echo "[ERROR] Nginx references detected in deploy/ but policy forbids nginx"
  fail "Remove nginx related files/references from deploy/"
fi

echo "[OK] Repository layout validation passed."

