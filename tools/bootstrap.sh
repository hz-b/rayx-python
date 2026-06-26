#!/usr/bin/env bash
set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
ROOT_DIR="$(cd "${SCRIPT_DIR}/.." && pwd)"
cd "$ROOT_DIR"

if ! command -v uv >/dev/null 2>&1; then
  echo "Error: uv is not installed or not in PATH." >&2
  echo "Install uv from https://docs.astral.sh/uv/getting-started/installation/" >&2
  exit 1
fi

PYTHON_SPEC="${PYTHON_SPEC:-3.12}"

venv_matches_python_spec() {
  if [ ! -x ".venv/bin/python" ]; then
    return 1
  fi
  local version
  version="$(".venv/bin/python" -c 'import sys; print(f"{sys.version_info.major}.{sys.version_info.minor}")')"
  [ "$version" = "$PYTHON_SPEC" ]
}

if [ -d ".venv" ]; then
  if venv_matches_python_spec; then
    echo "Using existing virtual environment at .venv (Python ${PYTHON_SPEC})"
  else
    echo "Existing .venv does not use Python ${PYTHON_SPEC}; recreating."
    rm -rf .venv
    echo "Creating virtual environment at .venv with Python ${PYTHON_SPEC}"
    uv venv --python "${PYTHON_SPEC}" .venv
  fi
else
  echo "Creating virtual environment at .venv with Python ${PYTHON_SPEC}"
  uv venv --python "${PYTHON_SPEC}" .venv
fi

echo "Installing rayx in editable mode (-e)"
echo "Note: this triggers the CMake/scikit-build-core build — cmake and a C++ compiler must be available."
uv pip install --python .venv/bin/python -e .

echo "Bootstrap complete."
echo "Activate with: source .venv/bin/activate"
