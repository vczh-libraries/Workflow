# Guidelines of Documentation and Git Commit Maintenance

- After finishing everything, git commit and git push to the current branch. NO NEED TO make a new branch.
- If in any task you are adding new test sample, or modifying any C++ or Workflow generation code, causing a huge amount of files generated from test samples to change:
  - Three commits are required:
    - First commit only has all modified files and files you created directly.
      - If reflection are changed causing binary and text metadata files updated, they are included in the first commit.
    - Second commit has all new files that not created by you (aka auto generated), focus on *.txt files. The commit title would be the first commit appending ` [txt]`
    - Third commit has all new files that not created by you (aka auto generated), focus on the rest. The commit title would be the first commit appending ` [generated]`
    - Typical folders for generated files:
      - `Test\Generated`
      - `Test\SourceCppGen`
      - `Test\SourceCppGenRpc`
      - `Test\UnitTest\Generated_*`
  - Otherwise, One commit is good.

## RPC Specific

- If `IRpc*Ops` interfaces in `WfLibraryRpc.h` are changed, the following files may have to be updated accordingly:
  - `Test/TypeScript/Rpc.d.ts`
  - `TODO_RPC_Definition.md`
  - `TODO_RPC_Json.md`
  - `TODO_RPC_GeneratedWrappers.md`
