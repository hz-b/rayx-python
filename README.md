# rayx-python

Python bindings for [RAYX](https://github.com/hz-b/rayx), the ray tracing engine for synchrotron optics.

## Installation

```bash
pip install rayx
```

> **Note:** The package includes a compiled C++ extension and is distributed as a pre-built wheel. Source builds are not supported via pip.

## Development

The package can be built in two ways:
1. with `cmake`
2. with `uv build`

The 1. option supports build cashing and the build result can be used immediatly without any installation steps. See [example](./examples/metrix.ipynb)
The 2. option builds the package as a wheel. The resulting wheel can be installed in your python environment.

## Developer tools

Set up a local `.venv` with all dependencies:

```bash
bash tools/bootstrap.sh
```

Build the documentation locally:

```bash
bash tools/build_docs.sh --html --open
```

### Running tests

To run the tests you need to build the package with cmake.

```bash
uv run pytest tests
```
