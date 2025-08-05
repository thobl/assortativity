#!/bin/bash

git submodule init
git submodule update

mkdir -p cpp/release
cd cpp/release
cmake ..
make
