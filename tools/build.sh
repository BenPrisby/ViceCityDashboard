#!/bin/bash

set -e

# Ensure Qt is available in the PATH.
if ! command -v qmake &> /dev/null; then
    echo "ERROR: Failed to find qmake. Ensure Qt is accessible in the PATH and try again."
    exit 1
fi

# Move to the repository root.
cd "$(dirname "$0")/.."

# Clean up any previous build artifacts.
rm -rf bin build

# Build the application.
mkdir build
cd build
qmake ..
if [[ "${OSTYPE}" == "darwin"* ]]; then
    cpu_count=$(sysctl -n hw.physicalcpu)
else
    cpu_count=$(nproc)
fi
make -j${cpu_count} install
