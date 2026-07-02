#!/usr/bin/env bash
set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
ROOT_DIR="$(cd "${SCRIPT_DIR}/.." && pwd)"
cd "$ROOT_DIR"

usage() {
  cat <<'EOF'
Usage: tools/bootstrap.sh [--with-nvidia|--without-nvidia]

Options:
  --with-nvidia     Build with NVIDIA/CUDA support.
  --without-nvidia  Build without NVIDIA/CUDA support.
  -h, --help        Show this help message.
EOF
}

if ! command -v uv >/dev/null 2>&1; then
  echo "Error: uv is not installed or not in PATH." >&2
  echo "Install uv from https://docs.astral.sh/uv/getting-started/installation/" >&2
  exit 1
fi

PYTHON_SPEC="${PYTHON_SPEC:-3.12}"
USE_NVIDIA="with"
NVIDIA_MODE_SET=0

while [ "$#" -gt 0 ]; do
  case "$1" in
    --with-nvidia|--nvidia)
      USE_NVIDIA="with"
      NVIDIA_MODE_SET=1
      ;;
    --without-nvidia|--no-nvidia)
      USE_NVIDIA="without"
      NVIDIA_MODE_SET=1
      ;;
    -h|--help)
      usage
      exit 0
      ;;
    *)
      echo "Error: unknown argument: $1" >&2
      usage >&2
      exit 1
      ;;
  esac
  shift
done

if [ "$NVIDIA_MODE_SET" -eq 0 ] && [ -t 0 ] && [ -t 1 ]; then
  read -r -p "Build with NVIDIA/CUDA support? [Y/n] " response
  case "$response" in
    [Nn]|[Nn][Oo])
      USE_NVIDIA="without"
      ;;
    *)
      USE_NVIDIA="with"
      ;;
  esac
fi

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
nvcc_path_hint=0

if command -v nvidia-smi >/dev/null 2>&1; then
  have_nvidia_smi=1
fi

if command -v nvcc >/dev/null 2>&1; then
  have_nvcc=1
elif [ -x "/usr/local/cuda/bin/nvcc" ]; then
  nvcc_path_hint=1
fi

if [ "$USE_NVIDIA" = "with" ] && { [ "$have_nvidia_smi" -ne 1 ] || [ "$have_nvcc" -ne 1 ]; }; then
  echo "Error: CUDA prerequisites are missing." >&2

  if [ "$have_nvidia_smi" -ne 1 ]; then
    echo "  - NVIDIA driver not found: install the driver so nvidia-smi works." >&2
  fi

  if [ "$have_nvcc" -ne 1 ]; then
    if [ "$nvcc_path_hint" -eq 1 ]; then
      echo "  - CUDA toolkit found at /usr/local/cuda/bin but not in PATH." >&2
      echo "    See README.md for how to add it to your PATH." >&2
    else
      echo "  - CUDA toolkit not found: install the CUDA toolkit so nvcc works." >&2
    fi
  fi

  echo "See: README.md" >&2
  exit 1
fi

if [ "$USE_NVIDIA" = "with" ]; then
  echo "Bootstrapping with NVIDIA/CUDA support enabled."
  BOOTSTRAP_CMAKE_ARGS="-DRAYX_ENABLE_CUDA=ON -DRAYX_REQUIRE_CUDA=ON"
else
  echo "Bootstrapping with NVIDIA/CUDA support disabled."
  BOOTSTRAP_CMAKE_ARGS="-DRAYX_ENABLE_CUDA=OFF -DRAYX_REQUIRE_CUDA=OFF"
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
CMAKE_ARGS="${CMAKE_ARGS:+${CMAKE_ARGS} }${BOOTSTRAP_CMAKE_ARGS}" \
  uv pip install --python .venv/bin/python -e .

echo "Bootstrap complete."
echo "Activate with: source .venv/bin/activate"
