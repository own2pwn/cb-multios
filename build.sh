#!/usr/bin/env bash

echo "Creating build directory"
mkdir build
pushd build

echo "Creating Makefiles"
cmake -DCMAKE_GENERATE_COMPILE_COMMANDS=On ..

make -j8

