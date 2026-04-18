# !!!INVESTIGATE!!!

# PROBLEM DESCRIPTION

Complete `RuntimeTest` project to make `Rpc` category unit test work.

Task 1. Complete `IndexRpc.txt` to add expected execution result, just like how `IndexCodegen.txt` does.

Task 2. Add `Test\Source\RpcDualLifecycleMock.(h|cpp)`, inheriting from `RpcLifecycleMock.h`, implementing dual lifecycle mock for RPC testing. The mock simulates two separate execution domains (lc1 and lc2) that communicate through cross-registered ops implementations.

Key design:
- Two `RpcDualLifecycleMock` instances with different client IDs (1 and 2)
- Each instance has a peer pointer to the other instance
- Ops created from one lc's assembly context are registered into the other lc
- When `RefToPtr` encounters a reference with a different `clientId`, it creates a wrapper proxy using the loaded Workflow wrapper function (`rpcwrapper_<interface-name>`)
- The `rpc_GetIds` function provides the id map, and the id map is used to build type-to-wrapper-name mappings
- `Register` override passes the id map to `SyncIds` on registered ops

# UPDATES

# TEST

N/A

# PROPOSALS

# PROPOSALS
