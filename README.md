# rayx-py

## building
- Release: `pipx run cibuildwheel`
- Development (2 options): 
    1. `pip wheel .` and install the resulting wheel
    2. build the project with CMake and either (both options require rayx to already be present in your environment as there are data files included in the full release that are not part of the .so file):
        1. import the package like this `import build.src.rayx as rayx` (using the project root as CWD)
        2. copy the built shared library (in build/src) to your CWD