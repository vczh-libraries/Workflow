# !!!INVESTIGATE!!!

# PROBLEM DESCRIPTION

DO NOT ASK ME ANY QUESTION, I will not be watching you, you must make your best decision and run through the end.
Follow `REPO-ROOT/.github/Rules/document-and-commit.md` to finish the work.
This is a multi-task request, follow the investigation instructions precisely.

## Task 1

I would like you to refactor API creation according to `Copilot_KB.md`'s `## Improvement Report` but limit to scope to RPC related code:
- Move RPC related source files from `Source/Analyzer` to `Source/Analyzer/Rpc`.
- New file `Source/Analyzer/Rpc/WfAnalyzer_RpcAstHelpers.(h|cpp)`, moving workflow AST helper functions to de-duplicate, and other RPC source files should rely on this file.
- Update `Copilot_KB.md` accordingly, removing oudated information after executing this task.
  - `WfAnalyer_RpcAstHelper.h` is a header file, but do not count this file into "ast building functions in header files" in `Copilot_KB.md`.

Verifying:
This refactoring should not affect generated code in any way.
You are required to:
- Run the complete test process
- Make sure the test compiler does not bring any change to generated Workflow or C++ code, otherwise it is highly possibly a refactoring defect.

# TEST [CONFIRMED]

This is a pure RPC analyzer refactor. The confirmation criteria are:

- RPC analyzer source files are moved under `Source/Analyzer/Rpc` and project/build metadata references the new paths.
- Shared Workflow AST construction helpers used by RPC generation live in `Source/Analyzer/Rpc/WfAnalyzer_RpcAstHelpers.(h|cpp)`, with RPC generator files using that header instead of repeated local declarations or duplicate helper definitions.
- `Copilot_KB.md` is updated to describe the new state and no longer reports the resolved declaration/helper duplication as outstanding. The new RPC AST helper header is not counted as a public header-level AST-building API.
- Full required verification passes: Debug Win32/x64 build, all required unit tests in the required order, TypeScript RPC verification, and `..\Tools\Tools\Build.ps1 Workflow`.
- After `CompilerTest_LoadAndCompile` and the full build/test process, no generated Workflow or generated C++ files have changed.

# PROPOSALS

- No.1 [CONFIRMED] Move RPC analyzer code into an RPC subfolder and extract shared RPC AST helpers

## No.1 Move RPC analyzer code into an RPC subfolder and extract shared RPC AST helpers

Move the RPC analyzer implementation files from `Source/Analyzer` to `Source/Analyzer/Rpc`, update all project and build metadata, and add a private RPC AST helper header/source pair for common Workflow AST factory helpers. Keep domain-specific RPC generation logic in the existing generator files, but move common helper declarations and implementations needed by multiple RPC generator files into `WfAnalyzer_RpcAstHelpers.(h|cpp)`.

### CODE CHANGE

- Moved RPC analyzer sources and private RPC generator headers from `Source/Analyzer` to `Source/Analyzer/Rpc`:
  - `WfAnalyzer_GenerateRpc.(h|cpp)`
  - `WfAnalyzer_GenerateRpc_JsonDts.cpp`
  - `WfAnalyzer_GenerateRpc_JsonSerialization.cpp`
  - `WfAnalyzer_ValidateRPC.cpp`
- Added `Source/Analyzer/Rpc/WfAnalyzer_RpcAstHelpers.(h|cpp)` for shared RPC Workflow AST factories.
- Moved reusable RPC AST helpers into `WfAnalyzer_RpcAstHelpers`, including type factories, expression factories, statement/declaration factories, `CreateTypeFromCpp<T>`, `CreateCall`, RPC exception helpers, `CreateWhile`, and block append helpers.
- Kept RPC-domain generators and dispatch helpers in `WfAnalyzer_GenerateRpc.(h|cpp)`, with JSON generation including the shared headers instead of repeating local forward declarations.
- Updated RPC metadata reconstruction to use the shared `CreateString` and `CreateInt` helpers instead of local literal factories.
- Updated `VlppWorkflow_Compiler.vcxitems` and `.filters` for the new source layout and helper files.
- Updated `Copilot_KB.md` so `WfAnalyzer_RpcAstHelpers.h` is documented as a private RPC implementation helper, not counted as a public header-level AST-building API.
- Regenerated release packaging through `..\Tools\Tools\Build.ps1 Workflow`, which updated `Release/VlppWorkflowCompiler*`.

### CONFIRMED

- `copilotBuild.ps1 -Configuration Debug -Platform Win32` passed.
- `copilotBuild.ps1 -Configuration Debug -Platform x64` passed.
- `LibraryTest` passed on Win32 and x64: 15/15 cases each.
- `CompilerTest_GenerateMetadata` passed on Win32 and x64: 2/2 cases each.
- `CompilerTest_LoadAndCompile` passed on x64.
- After `CompilerTest_LoadAndCompile`, `git diff --name-only -- Test/Generated Test/SourceCppGen Test/SourceCppGenRpc Test/UnitTest/Generated_Metaonly Test/UnitTest/Generated_Reflection Test/UnitTest/Generated_ReflectionRpc Test/UnitTest/Generated_Cpp Test/UnitTest/Generated_CppRpc Source/Parser/Generated` returned no files.
- `RuntimeTest` passed on Win32 and x64: 261/261 cases each.
- `CppTest`, `CppTest_Metaonly`, and `CppTest_Reflection` passed on Win32 and x64: 229/229 cases for each run.
- `Test/TypeScript/prepare.ps1` passed.
- `npm run build` under `Test/TypeScript` passed with `tsc --noEmit`.
- `..\Tools\Tools\Build.ps1 Workflow` was rerun at the end and passed, including release builds, unit tests, TypeScript build, and release packaging.
- After the full process, generated Workflow and generated C++ test output folders still had no diff. Only release packaging files changed as expected.
