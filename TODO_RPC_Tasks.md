# Completed RPC JSON Dispatch Refactor

- Reusable JSON RPC runtime files live under `Source/Library/RpcJson`.
- Channel-backed default dispatchers are `WfLibraryRpcJsonDispatcherClient`, `WfLibraryRpcJsonDispatcherServer`, and `WfLibraryRpcJsonDispatcherShared`.
- ChatBot-specific generated setup remains under `Test/UnitTest/ChatBotServer/Shared` in the `ChatBot*` files.
- `TODO_RPC_JsonRequest.md` documents the reusable setup and JSON request routing.
- `Release/Rpc.d.ts` is the canonical shared TypeScript dispatcher schema. `Test/TypeScript/prepare.ps1` copies it into the TypeScript test package as an ignored working file.
