#!/bin/bash

cmake -S . -B build
cmake --build build -j
./build/glfw_template