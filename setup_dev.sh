#!/bin/bash
# setup_dev.sh - Development setup with virtual environment

set -e

# Ensure we're in the right directory
cd "$(dirname "$0")"

# Check if venv exists
if [ ! -d ".venv" ]; then
    echo "Creating virtual environment with uv..."
    uv venv
    source .venv/bin/activate
    echo "Installing dependencies..."
    uv pip install numpy pytest matplotlib
else
    source .venv/bin/activate
fi

# Build C++ extension
echo "Building C++ extension..."
mkdir -p build
cd build

# Configure with local install prefix
cmake -DCMAKE_INSTALL_PREFIX=./install ..

# Build
make -j$(nproc)

# Install to local directory
make install

cd ..

# Create symlink
BUILD_DIR="build/install/rayx"
PYTHON_DIR="python/rayx"

SO_FILE=$(find "$BUILD_DIR" -name "_core.cpython-*.so" 2>/dev/null | head -n 1)

if [ -f "$SO_FILE" ]; then
    ln -sf "../../$SO_FILE" "$PYTHON_DIR/_core.so"
    echo "✓ Created symlink: $PYTHON_DIR/_core.so"
else
    echo "✗ Error: Could not find _core.cpython-*.so in $BUILD_DIR"
    echo "Files in build/install:"
    find build/install -type f 2>/dev/null || echo "  (directory doesn't exist)"
    exit 1
fi

# Add python directory to PYTHONPATH for development
export PYTHONPATH="$(pwd)/python:$PYTHONPATH"

source .venv/bin/activate
python3 -c "
import sys
sys.path.insert(0, 'python')
import rayx._core as m
with open('python/rayx/_core.pyi', 'w') as f:
    f.write('# Auto-generated stub\n')
    for name in dir(m):
        if name.startswith('_'): continue
        obj = getattr(m, name)
        if callable(obj):
            f.write(f'def {name}(*args, **kwargs): ...\n')
        else:
            f.write(f'{name}: object\n')
"
deactivate

echo "✓ Development environment ready!"
echo ""
echo "To use:"
echo "  source .venv/bin/activate"
echo "  cd python && python3 -c 'import rayx; print(rayx.__version__)'"
