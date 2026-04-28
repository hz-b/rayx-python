# rayx-python

Python bindings for [RAYX](https://github.com/hz-b/rayx), the ray tracing engine for synchrotron optics.

## Installation

```bash
pip install rayx
```

> **Note:** The package includes a compiled C++ extension and is distributed as a pre-built wheel. Source builds are not supported via pip.

## Development

To develop the package you need to:
1. build the core module with cmake
2. 

### Running tests

```bash
uv run pytest tests
```

## Release

Wheels are built using [cibuildwheel](https://cibuildwheel.readthedocs.io/) inside a custom manylinux container with CUDA support:

```bash
pipx run cibuildwheel
```

Built wheels are placed in `wheelhouse/`. See `pyproject.toml` for the build configuration.
