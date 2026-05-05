# !!!INVESTIGATE!!!

# PROBLEM DESCRIPTION

Update `WfAnalyzer_GenerateRpc.cpp` so that generated RPC metadata changes:
- `rpcNameToId` is not used, remove it
- `rpcIdToName` is unnecessary, the only usage is `RequestService`, use switch-case with constants created in the same script, and default case throws error. `InvokeMethod` and `InvokeEvent` should also use switch-case instead of if-else.
- `SyncIds` should leave blank bodies (they receive input, not output the map).
- Add `rpc_GetIds()` function returning `int[string]` map.

Then update `WfLibraryRpc.h`:
- Use `Dictionary<WString, vint>` in `IRpcIdSync::SyncIds`
- `IRpcController::Register` returns `void`
- Fix reflection registration accordingly.

# UPDATES

# TEST

N/A

# PROPOSALS
