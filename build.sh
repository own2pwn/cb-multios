#!/usr/bin/env bash

echo "Creating build directory"
mkdir build
pushd build

echo "Creating Makefiles"
cmake -DCMAKE_EXPORT_COMPILE_COMMANDS=On ..

make -j8

