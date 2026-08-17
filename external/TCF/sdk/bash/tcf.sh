#!/usr/bin/env bash

TCF_BIN="${TCF_BIN:-tcf}"

if ! command -v "$TCF_BIN" &> /dev/null; then
    if [ -f "$(dirname "${BASH_SOURCE[0]}")/../../target/debug/tcf-rs" ]; then
        TCF_BIN="$(dirname "${BASH_SOURCE[0]}")/../../target/debug/tcf-rs"
    fi
fi

tcf_get() {
    local key="$1"
    local file="${2:-build.tcf}"
    
    if [ -z "$key" ]; then
        echo "Error: Key is required" >&2
        return 1
    fi

    "$TCF_BIN" --file "$file" get "$key"
}

tcf_run() {
    local task="$1"
    local file="${2:-build.tcf}"

    if [ -z "$task" ]; then
        echo "Error: Task name is required" >&2
        return 1
    fi

    "$TCF_BIN" --file "$file" run "$task"
}

tcf_list() {
    local file="${1:-build.tcf}"
    "$TCF_BIN" --file "$file" list
}
