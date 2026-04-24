#!/bin/bash

# Ensure Eigen3.4 is downloaded
if [[ ! -d external/eigen ]]; then
    git clone --branch 3.4.0 --depth 1 https://gitlab.com/libeigen/eigen.git external/eigen
fi

# Run build process
cmake -S . -B build &&
cmake --build build -j &&
./build/hopfield_networks