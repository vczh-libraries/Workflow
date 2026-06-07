# Git Commit Maintenance

## Git Commit Structure

Multiple commits are strongly recommended to separate manual edited files and generated files.
- First commit only has all modified files and files you created directly.
- Followed by a commit with all new files that not created by you (aka auto generated), focus on *.txt files. The commit title would be the first commit appending ` [tx`.
  - You can skip it if no such file is edited.
- Followed by a commit with all new files that not created by you (aka auto generated), focus on the rest. The commit title would be the first commit appending [generated]`.
  - You can skip it if no such file is edited.
  - Typical folders for generated files:
    - `Test/Generated`
    - `Test/SourceCppGen`
    - `Test/SourceCppGenRpc`
    - `Test/UnitTest/Generated_*`
    - `Test/TypeScript`

## Timing of Git Commit and Push

Usually, git commit and push are required after finishing, but there is one exception:
- When `REPO-ROOT/.github/prompts/investigate.prompt.md` is tagged in a request, if the original request is organized with multiple `## Task X`:
  - Check out the `## Git Commits and Multi-Task Request` session in this file.
  - git commit and push are required to happen right after performing every single `## Task X`.
  - `## Git Commit Structure` describes commits for separate `## Task X`.
  - **DO NOT** wait until all `## Task X` are finished and the do git commit and push. Even when they are related.

## RPC Specific

- If `IRpc` interfaces in `WfLibraryRpc.h` are changed, the following files may have to be updated accordingly:
  - `Test/TypeScript/Rpc.d.ts`
  - Comment on `IRpcLifecycle`
    - If the only code change is this comment after a successful testing process, extra building is required but testing rerun not required.
  - `TODO_RPC_Definition.md`
  - `TODO_RPC_Json.md`
  - `TODO_RPC_GeneratedWrappers.md`
