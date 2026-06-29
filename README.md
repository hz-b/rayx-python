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

### Running tests

To run the tests you need to build the package with cmake.

```bash
uv run pytest tests
```

## Release Process

`develop` is the integration branch for release preparation. Feature work should land in `develop` first, and `main` should only be updated through a release promotion pull request.

Record release notes in [CHANGELOG.md](./CHANGELOG.md) under `## [Unreleased]`. When the branch is ready to ship, run:

```bash
bash tools/release.sh
```

The release script must be run from `develop` with a clean working tree. It:

1. selects the next semantic version
2. moves `Unreleased` notes into a dated release section in `CHANGELOG.md`
3. creates a release commit
4. creates and pushes the git tag `vX.Y.Z`
5. opens a pull request from `develop` to `main`

The package version is derived from the git tag via `setuptools_scm`. GitHub Actions builds wheels for `develop`, `main`, and release tags, and publishes to PyPI only when a `vX.Y.Z` tag is pushed.
