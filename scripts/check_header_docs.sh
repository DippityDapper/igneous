#!/usr/bin/env bash
# Verify every public header has a // Doc: line (or is listed in code-reference.md exceptions).
set -euo pipefail

ROOT="$(cd "$(dirname "$0")/.." && pwd)"
cd "$ROOT"

declare -A DOC_OVERRIDES=(
    ["PerlinNoise.hpp"]="Perlin.md"
    ["NetworkProtocol.hpp"]="Serializer.md"
)

EXCEPTIONS=(
    # None currently — every include/igneous/**/*.hpp must have // Doc:
)

is_exception() {
    local base="$1"
    for ex in "${EXCEPTIONS[@]}"; do
        [[ "$base" == "$ex" ]] && return 0
    done
    return 1
}

fail=0

while IFS= read -r header; do
    base="$(basename "$header")"

    if is_exception "$base"; then
        continue
    fi

    first_line="$(head -n 1 "$header")"
    if [[ "$first_line" != //\ Doc:\ docs/classes/* ]]; then
        echo "Missing // Doc: line: $header"
        fail=1
        continue
    fi

    doc_rel="${first_line#// Doc: }"
    doc_path="$ROOT/$doc_rel"
    if [[ ! -f "$doc_path" ]]; then
        echo "Doc file not found: $header -> $doc_rel"
        fail=1
    fi
done < <(find include/igneous -name '*.hpp' | sort)

if [[ "$fail" -ne 0 ]]; then
    exit 1
fi

echo "Header doc check passed ($(find include/igneous -name '*.hpp' | wc -l) headers)."
