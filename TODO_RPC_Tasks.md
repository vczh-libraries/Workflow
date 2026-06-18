# Completed RPC JSON Dispatch Refactor

- Reusable JSON RPC runtime files live under `Source/Library/RpcJson`.
- Channel-backed default dispatchers are `WfLibraryRpcJsonDispatcherClient`, `WfLibraryRpcJsonDispatcherServer`, and `WfLibraryRpcJsonDispatcherShared`.
- ChatBot-specific generated setup remains under `Test/UnitTest/ChatBotServer/Shared` in the `ChatBot*` files.
- [Workflow JSON Request Routing](.github/KnowledgeBase/KB_Workflow_JsonRequestRouting.md) documents the reusable setup and JSON request routing.
- `Release/Rpc.d.ts` is the canonical shared TypeScript dispatcher schema. `Test/TypeScript/prepare.ps1` copies it into the TypeScript test package as an ignored working file.
