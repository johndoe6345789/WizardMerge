#!/usr/bin/env sh
# Launch the WizardMerge GUI using the local virtual environment when present.
set -eu

VENV_DIR=${VENV_DIR:-.venv}
PYTHON_BIN=${PYTHON_BIN:-python3}

if [ -d "$VENV_DIR" ]; then
  # shellcheck disable=SC1090
  . "$VENV_DIR/bin/activate"
fi

exec "$PYTHON_BIN" -m wizardmerge.app "$@"
