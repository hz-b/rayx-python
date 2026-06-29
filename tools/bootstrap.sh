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

have_nvidia_smi=0
have_nvcc=0

if command -v nvidia-smi >/dev/null 2>&1; then
  have_nvidia_smi=1
fi

if command -v nvcc >/dev/null 2>&1; then
  have_nvcc=1
fi

if [ "$have_nvidia_smi" -ne 1 ] || [ "$have_nvcc" -ne 1 ]; then
  echo "Error: CUDA prerequisites are missing." >&2

  if [ "$have_nvidia_smi" -ne 1 ]; then
    echo "  - NVIDIA driver not found: install the driver so nvidia-smi works." >&2
  fi

  if [ "$have_nvcc" -ne 1 ]; then
    echo "  - CUDA toolkit not found: install the CUDA toolkit so nvcc works." >&2
  fi

  echo "See: install_nvidia.md" >&2
  exit 1
fi

echo "Updating git submodules"
git submodule update --init --recursive

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
