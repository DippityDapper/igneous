#!/usr/bin/env bash
# Bootstrap Igneous: submodules, configure, and build (debug preset).
set -euo pipefail

root="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
cd "$root"

echo "==> Initializing git submodules..."
git submodule update --init --recursive

echo "==> Configuring (debug preset)..."
cmake --preset debug

echo "==> Building..."
cmake --build --preset debug

echo ""
echo "Done. Try:"
echo "  ./build/examples/hello/example_hello"
echo "  ./build/tests/igneous_tests"
