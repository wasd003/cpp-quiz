#!/bin/bash

BUILD_LEVEL=$1

cmake -B build -DBUILD_LEVEL=$BUILD_LEVEL && cmake --build build --parallel
