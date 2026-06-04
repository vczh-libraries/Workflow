investigate repro

DO NOT ASK ME ANY QUESTION, I will not be watching you, you must make your best decision and run through the end.
Follow `REPO-ROOT/.github/Rules/document-and-commit.md` to finish the work.

## Task 1

`IRpcListOps` missing `IValueArray::Resize` equivalent constructions:
- `RpcByrefArray::Resize` has to do wired implementation, which is incorrect and should be fixed to be straight forward.
- `RpcCalleeListOps::ListClear` calls `IValueArray::Resize(0)`, which is incorrect, it should crash on array as array has no `Clear`.
Fix it by adding `ArrayResize` to `IRpcListOps`.
