# Installation

## Requirements

- **Platform**: Linux only. Pre-built wheels are provided for `manylinux x86_64`. macOS and Windows are not currently supported.
- **Python**: 3.9 or later
- **Compute**: CPU only. Unlike the original [RAYX](https://pypi.org/project/rayx/) C++ program, this Python package does not expose GPU acceleration at this time.

## Install

```bash
pip install rayx
```

The following dependencies are installed automatically:

| Package | Minimum version |
| --- | --- |
| `numpy` | 2.0 |
| `pandas` | 2.3 |
| `matplotlib` | 3.9 |

## Note on source builds

The package includes a compiled C++ extension and is distributed as a pre-built wheel. Building from source requires CMake, a C++ compiler, and the RAYX C++ dependencies; it is not supported via a plain `pip install` from source.
