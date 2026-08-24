#!/usr/bin/env bash

set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
DRIVER="$SCRIPT_DIR/Linux/RpcStdioTest_Driver/Bin/RpcStdioTest_Driver"
SERVICE="$SCRIPT_DIR/Linux/RpcStdioTest_Service/Bin/RpcStdioTest_Service"

usage() {
    echo "Usage: ./StartRpcStdio.sh [path-to-SkippedTestCaseListFile]" >&2
}

if [[ "$#" -gt 1 ]]; then
    usage
    exit 1
fi

if [[ ! -x "$DRIVER" ]]; then
    echo "RpcStdioTest_Driver is not built in Debug x64: $DRIVER" >&2
    exit 1
fi
if [[ ! -x "$SERVICE" ]]; then
    echo "RpcStdioTest_Service is not built in Debug x64: $SERVICE" >&2
    exit 1
fi

printf -v SERVICE_COMMAND '%q' "$SERVICE"

if [[ "$#" -eq 1 ]]; then
    SKIP_FILE="$1"
    if [[ ! -f "$SKIP_FILE" ]]; then
        echo "Skipped test case list not found: $SKIP_FILE" >&2
        exit 1
    fi
    exec "$DRIVER" "$SERVICE_COMMAND" "$SKIP_FILE"
fi

exec "$DRIVER" "$SERVICE_COMMAND"
