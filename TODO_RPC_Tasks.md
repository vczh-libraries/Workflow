investigate repro

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
