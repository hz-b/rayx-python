# rayx-python

Python bindings for [RAYX](https://github.com/hz-b/rayx), the ray tracing engine for synchrotron optics.

## Installation

```bash
pip install rayx
```

> **Note:** Standard installations use a pre-built wheel because the package includes a compiled C++ extension. The supported development source-build workflow is described below.

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

Use an editable install for development; it builds the native extension and makes
the Python wrapper importable from its normal package location. `uv build` creates
a distributable wheel.

```bash
git submodule update --init --recursive
uv sync --locked
uv pip install --no-build-isolation -e .
```

Re-run the editable-install command after changing the native bindings. The
persistent `build/<wheel-tag>` directory and non-isolated development toolchain
retain CMake's build cache, so Ninja recompiles only affected targets in parallel.

Python-only changes are available to the next Python process. After changing
C++ or nanobind code, re-run the editable-install command above, then restart
the Python process or test run; extension modules cannot be safely replaced in
an already-running interpreter.

After the editable install has configured the build directory, `cmake --build
build/<wheel-tag> -j` remains available for a compile-only check. Re-run the
editable-install command to also copy the rebuilt extension into the active Python
environment.

A `tools/bootstrap.sh` helper script is also available, wrapping the steps above with CUDA on/off prompting.

### Running tests

Tests require the editable install:

```bash
uv run pytest tests
```

## Releases

Release artifacts are built, tested, and published only by the wheel CI workflow.
It installs every produced wheel outside the checkout and runs the full test suite
before publication; do not publish a workstation-built wheel.
