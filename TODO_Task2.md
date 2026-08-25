Follow `REPO-ROOT/.github/Rules/document-and-commit.md` to finish the work.

`TODO_Task.md` already perform refactoring to RPC test samples, here are more. For any test sample <SAMPLE> there is:
- Test/Resources/Rpc/<SAMPLE>.txt
- Test/Resources/Rpc/<SAMPLE>_Test.txt

Currently <SAMPLE>.txt has RPC declaration, and <SAMPLE>_Test.txt has interface implementation and test code.
Now we need to refactor them using a new pattern:
- <SAMPLE>.txt has RPC interface and shared code.
- <SAMPLE>_Client.txt has client side only code.
- <SAMPLE>_Service.txt has service side only code.

According to the previous `TODO_Task.md` refactoring task, we should be able to do:
- <SAMPLE>_Service.txt only has a `serviceMain` function, with no other global declaration..
- <SAMPLE>_Client.txt has `clientMain` and things that are only used directly or indirectly from `clientMain`.
- <SAMPLE>.txt will also include shared functions, that used directly or indirectly from both `serviceMain` and `clientMain`. There should be no shared variables, all `serviceMain` used variables are already 

In `TestRpcCompiler.cpp` in `CompileRpcSample`, after all input are parsed, call a function `VerifyRpcSample` to make sure that:
- <SAMPLE>_Service.txt only has one top level function declaration which is `serviceMain`, no need to check the signature.
- <SAMPLE>_Client.txt has a top level function declaration which is `clientMain`, no need to check the signature. Note that unlike <SAMPLE>_Service.txt, we don't require `clientMain` is "the only top level declaration".
- <SAMPLE>.txt should not have top level variable declarations.
  
Additionally:
- Update `Project.md` and `.github/Rules/new-sample-rpc.md` to say the new pattern. We still want that `clientMain` and `serviceMain` could run in different process, that's why they don't share any variable.
  - In `new-sample-rpc.md` move the pattern about rpc sample to a new section `## Rpc Sample Convention`.
  - In `Project.md`, we don't repeat the information, instead we should refer to `## Rpc Sample Convention` in `new-sample-rpc.md`. Remove duplicated things about rpc sample convention in `Project.md`.
- Update the demo in the task template `TaskTemplate_NewSample.md` to follow the new pattern.
- Since no semantic in test cases is actually changed, I would expect `IndexRpc.txt` should not be touched as the result should just be the same.
- To verify, both `StartRpcStdio.ps1` and `UnitTest` should be executed and ensure every single case passes.
