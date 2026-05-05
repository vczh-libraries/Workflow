# !!!INVESTIGATE!!!

# PROBLEM DESCRIPTION

I like the work you have done. I would like to do a follow up change. `@rpc:Cached` processing is just introduced, and I found that I actually missed a lot of `@rpc:Dynamic` in existing test cases. I would like you to do this:

1. Review all samples in Rpc, but except `Prop(Default|Dynamic|Cached)` you just created, find all properties and add `@rpc:Dynamic` if there are no `@rpc:Dynamic` or `@rpc:Cached`.
2. Run the compiler as well as all following unit test to make sure it does not affect the test result.
3. Commit everything that are changed.
4. At this moment, the last commit will be this change, and the last second commit are all "accidentally affected" geenrated files while implementing the `@rpc:Cached` feature. Now you can run `git reset HEAD~2`, but doing this I believe many generated file changing in these two commit will cancel each other. This is how you verify if you have adding enough `@rpc:Dynamic`. But when there are still  unexpected changes, you need to verify the change and decide if it is reasonable.
5. commit it, but do not push, I will do a manual reviewing after that, to verify if anything needs to add to this feature.

# UPDATES

# TEST [CONFIRMED]

- Static confirmation: scan every file under `Test/Resources/Rpc` except `PropCached.txt`, `PropDefault.txt`, and `PropDynamic.txt`; every `prop` declaration must now have an immediately preceding `@rpc:Dynamic` or `@rpc:Cached` line.
- Static result: the scan found no remaining implicit-mode property declarations outside the three `Prop*` samples.
- Sample edit scope: 33 RPC sample files changed, with one explicit `@rpc:Dynamic` inserted in each file.
- Validation matrix and success criteria:
  - `copilotBuild.ps1 -Configuration Debug -Platform x64`: passed.
  - `copilotBuild.ps1 -Configuration Debug -Platform Win32`: passed.
  - `CompilerTest_GenerateMetadata` `Debug|x64`: passed.
  - `CompilerTest_GenerateMetadata` `Debug|Win32`: passed.
  - `CompilerTest_LoadAndCompile` `Debug|x64`: passed.
  - Rebuild after regenerated outputs:
    - `copilotBuild.ps1 -Configuration Debug -Platform x64`: passed.
    - `copilotBuild.ps1 -Configuration Debug -Platform Win32`: passed.
  - `RuntimeTest` `Debug|x64`: passed.
  - `RuntimeTest` `Debug|Win32`: passed.
  - `CppTest` `Debug|x64`: passed.
  - `CppTest` `Debug|Win32`: passed.
  - `CppTest_Metaonly` `Debug|x64`: passed.
  - `CppTest_Metaonly` `Debug|Win32`: passed.
  - `CppTest_Reflection` `Debug|x64`: passed.
  - `CppTest_Reflection` `Debug|Win32`: passed.
- Reset verification criteria: after a checkpoint commit and `git reset HEAD~2`, the remaining generated diff should be explainable by the explicit `@rpc:Dynamic` sample sweep and the real cached-feature generator output, not by unrelated wrapper metadata churn.
- Reset verification result: all `Test/Generated/RpcMetadata*` diffs disappeared. The surviving generated changes were limited to `Test/Generated/Workflow*`, `Test/Generated/CppRpc*`, `Test/SourceCppGenRpc`, and `Release/VlppWorkflowCompiler.cpp`, all scoped to the same 33 edited sample stems plus `TestCasesRpc`.

# PROPOSALS

- No.1 Add explicit `@rpc:Dynamic` to all remaining implicit RPC properties [CONFIRMED]

## No.1 Add explicit `@rpc:Dynamic` to all remaining implicit RPC properties

### CODE CHANGE

- Added `@rpc:Dynamic` to the 30 remaining collection-interface RPC sample properties that previously relied on the default property mode:
  - `Collection_Interface_(Default|InByref_OutByref|InByref_OutByval|InByval_OutByref|InByval_OutByval)`
  - `Collection_Interface_Nested_(Default|InByref_OutByref|InByref_OutByval|InByval_OutByref|InByval_OutByval)`
  - `CollectionDict_Interface_(Default|InByref_OutByref|InByref_OutByval|InByval_OutByref|InByval_OutByval)`
  - `CollectionDict_Interface_Nested_(Default|InByref_OutByref|InByref_OutByval|InByval_OutByref|InByval_OutByval)`
  - `CollectionOblist_Interface_(Default|InByref_OutByref|InByref_OutByval|InByval_OutByref|InByval_OutByval)`
  - `CollectionOblist_Interface_Nested_(Default|InByref_OutByref|InByref_OutByval|InByval_OutByref|InByval_OutByval)`
- Added `@rpc:Dynamic` to the remaining standalone interface properties in:
  - `Dtor3.txt`
  - `FailDoubleRegistration.txt`
  - `Overloading.txt`
- Regenerated affected Workflow and C++ outputs by rerunning the compiler and downstream unit tests.
- No parser, compiler, or wrapper-generator source changes were needed for this follow-up.

### CONFIRMED

- The post-edit scan confirmed there are no remaining implicit-mode property declarations outside `PropCached`, `PropDefault`, and `PropDynamic`.
- The full downstream validation matrix passed on both platforms, so making the property mode explicit did not change test outcomes unexpectedly.
- After creating a checkpoint commit and performing the requested `git reset HEAD~2`, wrapper metadata output changes canceled out completely, which is the expected signal that the missing explicit `@rpc:Dynamic` annotations had been covering previously accidental generated churn.
- The remaining generated changes were reasonable: they matched the same 33 edited sample stems in Workflow/C++ outputs, updated the shared `TestCasesRpc` generated sources, and preserved the real `Release/VlppWorkflowCompiler.cpp` regeneration from the cached-property feature work.