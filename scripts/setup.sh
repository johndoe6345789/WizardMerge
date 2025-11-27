#!/usr/bin/env sh
# Prepare a local virtual environment and install dependencies.
set -eu

VENV_DIR=${VENV_DIR:-.venv}
PYTHON_BIN=${PYTHON_BIN:-python3}

if [ ! -d "$VENV_DIR" ]; then
  "$PYTHON_BIN" -m venv "$VENV_DIR"
fi

# shellcheck disable=SC1090
. "$VENV_DIR/bin/activate"

pip install --upgrade pip
pip install -r requirements.txt

echo "Environment ready. Activate with: . $VENV_DIR/bin/activate"
