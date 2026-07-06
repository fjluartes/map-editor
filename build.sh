#!/bin/bash
cmake --preset unix-default
cmake --build --preset unix-default

if [ $# -ne 2 ]; then
    echo "Usage: $0 {-edit|-map} <map_name>"
    exit 1
fi

MODE="$1"
MAP_NAME="$2"

case "$MODE" in
    -edit|-map)
        ./build/map-editor "$MODE" "$MAP_NAME"
        ;;
    *)
        echo "Error: first argument must be -edit or -map"
        echo "Usage: $0 {-edit|-map} <map_name>"
        exit 1
        ;;
esac