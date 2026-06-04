# Guidelines of Documentation and Git Commit Maintenance

- After finishing everything, git commit and git push to the current branch. NO NEED TO make a new branch.
- Git commit structure:
  - Multiple commits are strongly recommended to separate manual edited files and generated files.
    - First commit only has all modified files and files you created directly.
    - Followed by a commit with all new files that not created by you (aka auto generated), focus on *.txt files. The commit title would be the first commit appending ` [txt]`.
      - You can skip it if no such file is edited.
    - Followed by a commit with all new files that not created by you (aka auto generated), focus on the rest. The commit title would be the first commit appending ` [generated]`.
      - You can skip it if no such file is edited.
      - Typical folders for generated files:
        - `Test/Generated`
        - `Test/SourceCppGen`
        - `Test/SourceCppGenRpc`
        - `Test/UnitTest/Generated_*`
        - `Test/TypeScript`
  - Pay attention when `investigate.prompt.md` is tagged, if it is a multi-tasks request:
    - Check out the `## Git Commits and Multi-Task Request` session.
    - Commits structure here are for each separate task.
    - You are required to perform a task and immediately push changes, before touching the next task.

## RPC Specific

- If `IRpc` interfaces in `WfLibraryRpc.h` are changed, the following files may have to be updated accordingly:
  - `Test/TypeScript/Rpc.d.ts`
  - `TODO_RPC_Definition.md`
  - `TODO_RPC_Json.md`
  - `TODO_RPC_GeneratedWrappers.md`
