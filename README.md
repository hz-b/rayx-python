# rayx-python

Python bindings for [RAYX](https://github.com/hz-b/rayx), the ray tracing engine for synchrotron optics.

## Installation

```bash
pip install rayx
```

> **Note:** The package includes a compiled C++ extension and is distributed as a pre-built wheel. Source builds are not supported via pip.

## Requirements

For development builds (not needed for the `pip install` above):

- **uv** — package/environment manager used for development builds. [Install guide](https://docs.astral.sh/uv/getting-started/installation/)
- **Python** — 3.12 is recommended [Download](https://www.python.org/downloads/)
- **HDF5** — required by `rayx-core` for compilation. Install via your system package manager (e.g. `apt install libhdf5-dev` on Debian/Ubuntu, `brew install hdf5` on macOS). See the [HDF5 docs](https://docs.hdfgroup.org/hdf5/develop/_getting_started.html) if you need a different install method.
- **NVIDIA GPU driver** — only required for CUDA-accelerated builds. Verify with `nvidia-smi`. [Download](https://www.nvidia.com/Download/index.aspx)
- **CUDA Toolkit** — only required for CUDA-accelerated builds. Verify with `nvcc --version`. [Download](https://developer.nvidia.com/cuda-downloads)

If the toolkit is installed but `nvcc` isn't found, its `bin` directory is missing from `PATH`:

```bash
export PATH=/usr/local/cuda/bin:$PATH
```

Make it permanent by adding that line to `~/.bashrc` (or `~/.zshrc`) and reloading.

## Development

The package can be built in two ways:

1. **CMake** — supports build caching; the result is usable immediately with no install step. See the [example notebook](./examples/metrix.ipynb).
2. **`uv build`** — builds the package as a wheel, which can then be installed into any Python environment.

```bash
git submodule update --init --recursive

# Option 1: build in place with CMake (uv creates/manages the environment)
uv run cmake -S . -B build
uv run cmake --build build -j

# Option 2: editable install via uv
uv pip install -e .
```

`uv run` puts the managed environment's Python first on `PATH`, so CMake's `find_package(Python)` resolves the right interpreter without pinning it manually.

`uv pip install -e .` only re-links the Python wrapper files. It does **not** rebuild the compiled bindings — if you change `rayx-core` or the nanobind glue in `rayxpy`, rerun the CMake commands (Option 1) to recompile, or the install will keep serving the stale extension.

A `tools/bootstrap.sh` helper script is also available, wrapping the steps above with CUDA on/off prompting.

### Building the docs

Use `bash tools/build_docs.sh --html --open` to build and open the documentation. The script creates a dedicated docs virtual environment, installs the docs dependencies, and writes the HTML output to `docs/_build/html/`.

### Running tests

Tests require a CMake build:

```bash
uv run pytest tests
```
