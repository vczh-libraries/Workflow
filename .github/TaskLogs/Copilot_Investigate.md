# !!!INVESTIGATE!!!

# PROBLEM DESCRIPTION

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

Correction received during implementation: `## Setup` and its `### Implementation` subsection belong in `TODO_RPC_JsonRequest.md`; `TODO_RPC_JsonDispatch.md` is the wrong file name and should not be created or referenced.

# TEST [CONFIRMED]

The request is a refactor and documentation update. I will confirm the work with static checks and builds:

- Verify old C++ paths are gone from project files, source includes, docs, and non-generated build metadata.
- Verify new `Source/Library/RpcJson` files are included in shared library project metadata and consumed by ChatBot projects.
- Verify `Release/Rpc.d.ts` is the documented source of the shared TypeScript schema, while `Test/TypeScript/prepare.ps1` restores the working copy under `Test/TypeScript`.
- Verify the TypeScript package still builds after running `Test/TypeScript/prepare.ps1`.
- Because `*.h` and `*.cpp` files are moved, run the required Workflow unit-test/build flow as far as the repository scripts permit and record any blocker.

# PROPOSALS

- No.1 [CONFIRMED] Move reusable RPC JSON files and shared schema to their requested production/release locations

## No.1 [CONFIRMED] Move reusable RPC JSON files and shared schema to their requested production/release locations

Move `WfLibraryRpcJson` into `Source/Library/RpcJson`, promote the reusable ChatBot RPC JSON dispatcher files into the same folder with the `WfLibraryRpcJsonDispatcher*` names, update includes and project metadata, document the reusable setup in `TODO_RPC_JsonRequest.md`, and make `Release/Rpc.d.ts` the canonical shared TypeScript schema copied by `Test/TypeScript/prepare.ps1`.

### CODE CHANGE

- Moved `WfLibraryRpcJson.(h|cpp)` from `Source/Library/Rpc` to `Source/Library/RpcJson`.
- Moved reusable `RpcJsonDispatcher*` files from `Test/UnitTest/ChatBotServer/Shared` to `Source/Library/RpcJson` as `WfLibraryRpcJsonDispatcher*`; ChatBot-specific setup remains in `ChatBot*` files.
- Updated source includes, `VlppWorkflow_Library.vcxitems`, ChatBot project files, and generated release library outputs to use the new `Source/Library/RpcJson` location.
- Added `## Setup` and `### Implementation` to `TODO_RPC_JsonRequest.md`; no `TODO_RPC_JsonDispatch.md` remains.
- Moved the shared TypeScript schema to `Release/Rpc.d.ts`; `Test/TypeScript/prepare.ps1` now copies it back to the ignored `Test/TypeScript/Rpc.d.ts` working file.
- Updated documentation and learning/rule notes that referred to `Test/TypeScript/Rpc.d.ts`.

### CONFIRMED

- Static scan found no remaining ordinary references to `TODO_RPC_JsonDispatch.md`, `Test/TypeScript/Rpc.d.ts`, `Source/Library/Rpc/WfLibraryRpcJson`, or `Test/UnitTest/ChatBotServer/Shared/RpcJsonDispatcher`.
- XML parsing and project-item existence checks passed for touched `.vcxitems`, `.filters`, and `.vcxproj` files.
- `Test/TypeScript/prepare.ps1` succeeded; `npm run build` succeeded; standalone `npx tsc --ignoreConfig --strict --noEmit --moduleResolution node16 --module node16 --target ES2020 Rpc.d.ts` succeeded.
- `.github/Scripts/copilotBuild.ps1 -Configuration Debug -Platform Win32` succeeded with `0 Error(s)`.
- `.github/Scripts/copilotBuild.ps1 -Configuration Debug -Platform x64` succeeded with `0 Error(s)`.
- Debug unit tests passed: `LibraryTest` Win32/x64, `CompilerTest_GenerateMetadata` Win32/x64, `CompilerTest_LoadAndCompile` x64, `RuntimeTest` Win32/x64, `CppTest` Win32/x64, `CppTest_Metaonly` Win32/x64, and `CppTest_Reflection` Win32/x64.
- `C:\Code\VczhLibraries\Tools\Tools\Build.ps1 Workflow` succeeded, including Release Win32/x64 rebuild/test flow, TypeScript package build, and release generation.
