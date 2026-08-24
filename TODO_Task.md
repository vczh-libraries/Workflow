Follow `REPO-ROOT/.github/Rules/document-and-commit.md` to finish the work.

- `Tools` repo update:
  - Add `StartRpcStdio.sh` to `vgo vbuild Workflow`.
  - Add `StartRpcStdio.ps1` to `Build.ps1 -Project Workflow`.
- You can find a recent commit making `StartRpcStdio.(ps1|sh)` to work:
  - The root cause is the original test cases assume service and client are running in the same memory space, so in that commit they are separated. But the change is not making the script obvious enough for that intent.
  - Further improvements are required for each rpc test case Workflow script (txt files)
    - In the script there are multiple global variables, remove which is no longer used.
    - If a global variable, or a global function is only used (including recursively/indirectly) in service side, move it inside the service class (inside `new (IService^)` inside `serviceMain`).
    - Otherwise, such global variable or function should only be used in client side, or when a function is stateless it could be shared between service/client side:
      - If a global variable or a global function is not used (including recursively/indirectly) in service side, move them below `serviceMain`.
    - Add the above policy to `Project.md` and `.github/Rules/new-sample-rpc.md`.
  - Since no semantic in test cases is actually changed, I would expect `IndexRpc.txt` should not be touched as the result should just be the same.
- To verify, both `StartRpcStdio.ps1` and `UnitTest` should be executed and ensure every single case passes.
