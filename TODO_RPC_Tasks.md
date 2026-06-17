investigate repro

DO NOT ASK ME ANY QUESTION, I will not be watching you, you must make your best decision and run through the end.
Follow `REPO-ROOT/.github/Rules/document-and-commit.md` to finish the work.
This is a multi-task request, follow the investigation instructions precisely.

## Task 1

Move `Source/Library/Rpc/WfLibraryRpcJson.(h|cpp)` to `Source/Library/RpcJson/WfLibraryRpcJson.(h|cpp)`.
Move `Test/UnitTest/ChatBotServer/Shared/RpcJsonDispatcher*.*` to `Source/Library/RpcJson/WfLibraryRpcJsonDispatcher*.*`. `ChatBot*.*` remain in that shared folder.
Now `WfLibraryRpcJsonDispatch*.*` become a sharable/default setup for executing `TODO_RPC_JsonDispatch.md`. Add a new section `## Setup` describing how `Source/Library/RpcJson` classes and functions are used:
- Focus on things that user really need to touch.
- Something in the header file are actually internal implementation, skip them in `## Setup`, but describe them in `### Explanation of Implementation` at the end of `## Setup`.
- You can check out how `Test/UnitTest/ChatBotServer/Shared` and `Test/UnitTest/ChatBot*/Main.cpp` are using them.

Move `Test/TypeScript/Rpc.d.ts` to `Release/Rpc.d.ts`:
- In documents where mentioning `Test/TypeScript/Rpc.d.ts`, update them to reference `Release/Rpc.d.ts`.
- `Test/TypeScript/prepare.ps1` copy `Release/Rpc.d.ts` to `Test/TypeScript/Rpc.d.ts`.
- Delete `Test/TypeScript/Rpc.d.ts`, now this file only available by running `prepare.ps1`.
  - Add it to `Test/TypeScript/.gitignore`.
