investigate repro

- you have to follow `REPO-ROOT/.github/Guidelines/Coding.md` when coding.
- you have to run unit test to make sure your change works.
- you have to follow complete instructions in `.github\Rules\verify-and-commit.md` to properly finish any task, before doing the next task.
  - It is important to do task one by one strictly, by me designing tasks in this way, we can achieve:
  - Easy-to-understand commits for file changing that is easy to review.
  - Limit side effects so that you don't have to deal with massive of issues at the same time.
- When I say `RunRpcTestCase` needs to change, usually it means all version of `RunRpcTestCase`.
  - No need to create helper functions just to share code between them unless explicitly instructed.

## Task 1

Remote `inline` of functions in `WfLibraryRpc.h`, extern them and put implementation in `WfLibraryRpc.cpp`.
Verify all `WfLibrary*.(h|cpp)`, even C++ not requires but we want to have `extern` on all function forward declarations.

