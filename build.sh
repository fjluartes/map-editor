#!/bin/bash
cmake --preset unix-default
cmake --build --preset unix-default
while read arg; do
  ./build/map-editor "$arg"
done