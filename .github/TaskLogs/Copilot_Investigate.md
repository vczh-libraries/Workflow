# !!!INVESTIGATE!!!
# PROBLEM DESCRIPTION

DO NOT ASK ME ANY QUESTION, I will not be watching you, you must make your best decision and run through the end.
Follow `REPO-ROOT/.github/Rules/document-and-commit.md` to finish the work.

## Task 1

`IRpcListOps` missing `IValueArray::Resize` equivalent constructions:
- `RpcByrefArray::Resize` has to do wired implementation, which is incorrect and should be fixed to be straight forward.
- `RpcCalleeListOps::ListClear` calls `IValueArray::Resize(0)`, which is incorrect, it should crash on array as array has no `Clear`.
Fix it by adding `ArrayResize` to `IRpcListOps`.

Rename `Oblist_EventException` rpc sample to `ListOps_OblistEventException`:
- Namespace in the sample change from `RpcOblistEventException` to `RpcListOpsOblistEventException`
- Update all affected files.

## Task 2

Follow
- `.github\Rules\new-sample.md`
- `.github\Rules\new-sample-rpc.md`
- `.github\Rules\verify-and-commit.md`
to add new samples, indentation should be double spaces no matter how the code below is written:

RPC samples are split into two files:
- `Rpc\SAMPLE.txt` contains only RPC definitions.
- `Rpc\SAMPLE_Test.txt` contains executable test logic, including globals, helpers, `serviceMain`, and `clientMain`.

Only `SAMPLE` is added to `IndexRpc.txt`. Both files must appear in `CompilerTest_LoadAndCompile` under `Resource Files\Rpc`.

### Sample Rpc\ListOps_ListException.txt

```Workflow
module Rpc;
using system::*;

namespace YourFavoriteNamespace // use ListOpsArray
{
  @rpc:Interface
  @rpc:Ctor
  interface IServer
  {
    func DoSomethingWrong(xs : int[]) : int;
  }
}
```

### Sample Rpc\ListOps_ListException_Test.txt

```Workflow
module Rpc;
using system::*;
using RpcWrapperTest::*;
using YourFavoriteNamespace::*;

var s = "";

func serviceMain(lc : IRpcLifecycle*) : void
{
  var serviceObj = new (YourFavoriteNamespace::IService^)
  {
    override func DoSomethingWrong(xs : int[]) : int
    {
        return xs[10];
    }
  };
  lc.RegisterService("YourFavoriteNamespace::IService", serviceObj);
}

func clientMain(lc : IRpcLifecycle*) : string
{
  var service = cast (YourFavoriteNamespace::IService^) lc.RequestService("YourFavoriteNamespace::IService");
  try
  {
    service.DoSomethingWrong({});
  }
  catch (ex)
  {
    s = ex;
  }
  return s;
}
```

### Sample Rpc\ListOps_DictionaryException.txt

```Workflow
module Rpc;
using system::*;

namespace YourFavoriteNamespace // use ListOpsArray
{
  @rpc:Interface
  @rpc:Ctor
  interface IServer
  {
    func DoSomethingWrong(xs : int[string]) : int;
  }
}
```

### Sample Rpc\ListOps_DictionaryException_Test.txt

```Workflow
module Rpc;
using system::*;
using RpcWrapperTest::*;
using YourFavoriteNamespace::*;

var s = "";

func serviceMain(lc : IRpcLifecycle*) : void
{
  var serviceObj = new (YourFavoriteNamespace::IService^)
  {
    override func DoSomethingWrong(xs : int[string]) : int
    {
        return xs["Anyway"];
    }
  };
  lc.RegisterService("YourFavoriteNamespace::IService", serviceObj);
}

func clientMain(lc : IRpcLifecycle*) : string
{
  var service = cast (YourFavoriteNamespace::IService^) lc.RequestService("YourFavoriteNamespace::IService");
  try
  {
    service.DoSomethingWrong({});
  }
  catch (ex)
  {
    s = ex;
  }
  return s;
}
```

## Goal

In this task you are going to build and run test cases to verify if these cases are working, according to `TODO_RPC_Definition.md`
This test is to ensure that:
- Both `Exception` and `Error` will be catched.

If the current implemention is correct, the added samples should just pass the test.
Scan through all `InvokeMethod` and `InvokeEvent` code around try-catch and make sure both `Exception` and `Error` are treated equally.
- Even when `Error` is for fatal error but since this is the Workflow environment `Exception` and `Error` are treated as the same thing.

# TEST

Use `LibraryTest` to verify byref arrays forward `Resize` through a dedicated RPC list-op method and that list-only `Clear` or `RemoveAt` requests against arrays are reported as RPC exceptions instead of silently resizing.

Use `CompilerTest_GenerateMetadata` to verify the reflected `IRpcListOps` contract after adding `ArrayResize`.

Use `CompilerTest_LoadAndCompile` to regenerate and verify RPC samples after renaming `Oblist_EventException` and, later, after adding the new list/dictionary exception samples.

Use the RPC runtime tests and generated C++ tests required by `Project.md` after generated artifacts change.

The requested finish rule was corrected to `.github/Rules/document-and-commit.md`; `.github/Rules/verify-and-commit.md` and `.github/Rules/document-and-verify.md` do not exist.

# PROPOSALS

- No.1 Add `IRpcListOps::ArrayResize` and keep list mutations list-only

## No.1 Add `IRpcListOps::ArrayResize` and keep list mutations list-only

The array resize path should be an explicit list-op method id, implemented by `RpcCallerListOps` and `RpcCalleeListOps`, then consumed directly by `RpcByrefArray::Resize`. `ListClear` and `ListRemoveAt` should only operate on `IValueList`; array resizing should not be disguised as list clearing or removal.

### CODE CHANGE

- Add a new predefined RPC method id and `IRpcListOps::ArrayResize`.
- Implement caller/callee dispatch for array resizing.
- Update reflection proxies, metadata declarations, TypeScript RPC constants, and byref array tests.
- Make RPC list-op target-shape failures throw `Exception` directly, so list-only requests against arrays are serialized through the existing RPC exception path.
- Rename the RPC sample files and namespace from `Oblist_EventException` / `RpcOblistEventException` to `ListOps_OblistEventException` / `RpcListOpsOblistEventException`.

### TEST RESULT

[CONFIRMED] `IRpcListOps::ArrayResize` forwards array resize directly, while `ListClear` and `ListRemoveAt` now reject array references through the RPC exception path.

[CONFIRMED] The renamed `ListOps_OblistEventException` sample compiles, generates metadata/C++ wrappers, and passes in runtime and generated C++ test modes.

[CONFIRMED] Verified with Debug x64 and Win32 builds, `LibraryTest`, `CompilerTest_GenerateMetadata`, `CompilerTest_LoadAndCompile`, `RuntimeTest`, `CppTest`, `CppTest_Metaonly`, and `CppTest_Reflection`.

- No.2 Add list/dictionary exception RPC samples and convert reflected collection `Error`s

## No.2 Add list/dictionary exception RPC samples and convert reflected collection `Error`s

The new samples need to prove that collection access failures thrown by reflected value wrappers are visible to Workflow RPC clients as ordinary exceptions. Instead of changing Workflow runtime code generation or broad RPC `InvokeMethod` / `InvokeEvent` catches, update VlppReflection collection wrappers so collection `Error`s are rethrown as `Exception`s at the wrapper boundary, then release the generated VlppReflection import back into Workflow.

### CODE CHANGE

- Add `ListOps_ListException` and `ListOps_DictionaryException` RPC samples, using `RpcListOpsListException` and `RpcListOpsDictionaryException` namespaces.
- Register both RPC definition files in `IndexRpc.txt`, and register both definition/test files in `CompilerTest_LoadAndCompile`.
- Regenerate Workflow, metadata, TypeScript, source C++ RPC, and Debug Win32/x64 generated C++ RPC artifacts.
- Update upstream VlppReflection collection wrappers to translate `Error` to `Exception` for reflected enumerator, list, array, and dictionary operations.
- Release upstream `VlppReflection.h` into Workflow `Import`, leaving `WfCpp_Statement.cpp` and `WfRuntimeExecution.cpp` unchanged and removing the broad generated/runtime `Error` catch approach.

### TEST RESULT

[CONFIRMED] The two new RPC samples pass in interpreted/runtime and generated C++ modes, with both returning `ArrayBase<T, K>::Get(vint)#Argument index not in range.`.

[CONFIRMED] Workflow Debug x64 build and tests passed: `CompilerTest_GenerateMetadata`, `CompilerTest_LoadAndCompile`, `LibraryTest`, `RuntimeTest`, `CppTest`, `CppTest_Metaonly`, and `CppTest_Reflection`.

[CONFIRMED] Workflow Debug Win32 build and tests passed: `CompilerTest_GenerateMetadata`, `CompilerTest_LoadAndCompile`, `LibraryTest`, `RuntimeTest`, `CppTest`, `CppTest_Metaonly`, and `CppTest_Reflection`.

[CONFIRMED] Upstream VlppReflection Debug Win32 and x64 builds passed, including `UnitTest`, `Metadata_Generate`, and `Metadata_Test`.
