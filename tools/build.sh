#!/bin/bash

# Move to the current working directory.
cd $(dirname ${BASH_SOURCE[ ${#BASH_SOURCE[@]} - 1 ]})

# Clean up any previous build artifacts.
if [ -d ../build ]; then
    rm -rf ../build
fi
if [ -d ../bin ]; then
    rm -rf ../bin
fi

# Build the application
mkdir ../build
cd ../build
qmake ..
make -j4 install

