#!/usr/bin/env bash

set -e

ROOT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
cd "$ROOT_DIR"

echo "[Tiwut API] Cleaning project build artifacts..."

rm -rf "$ROOT_DIR/bin"
rm -rf "$ROOT_DIR/lib"
rm -rf "$ROOT_DIR/sample/bin"
rm -f "$ROOT_DIR/sample_snapshot.json"
rm -f "$ROOT_DIR/sample_output.json"
rm -f "/tmp/tiwut_server.pid"

find "$ROOT_DIR/src" -type f -name "*.o" -delete 2>/dev/null || true
find "$ROOT_DIR/sample" -type f -name "*.o" -delete 2>/dev/null || true
find "$ROOT_DIR/sdk" -type f -name "*.o" -delete 2>/dev/null || true

find "$ROOT_DIR" -type f -name "*.dSYM" -exec rm -rf {} + 2>/dev/null || true
find "$ROOT_DIR" -type d -name "*.dSYM" -exec rm -rf {} + 2>/dev/null || true
find "$ROOT_DIR" -type f -name "*.dylib" -delete 2>/dev/null || true
find "$ROOT_DIR" -type f -name "*.a" -delete 2>/dev/null || true
find "$ROOT_DIR" -type f -name "*.gch" -delete 2>/dev/null || true
find "$ROOT_DIR" -type f -name ".DS_Store" -delete 2>/dev/null || true

if [ -d "$ROOT_DIR/external/cfeel" ]; then
    rm -f "$ROOT_DIR/external/cfeel/cfeelc"
    find "$ROOT_DIR/external/cfeel" -type f -name "*.o" -delete 2>/dev/null || true
fi

if [ -d "$ROOT_DIR/external/TCF/target" ]; then
    rm -rf "$ROOT_DIR/external/TCF/target"
fi

echo "[Tiwut API] Clean completed successfully. Repository is ready for git."
