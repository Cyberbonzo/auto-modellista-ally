#!/usr/bin/env bash
set -euo pipefail
root="$(cd -- "$(dirname -- "$0")/.." && pwd -P)"
exec podman run --rm --pull=never --userns=keep-id --security-opt label=disable -v "$root:$root" -w "$PWD" localhost/automodellista-builder:0.1.0 /usr/bin/clang++ "$@"
