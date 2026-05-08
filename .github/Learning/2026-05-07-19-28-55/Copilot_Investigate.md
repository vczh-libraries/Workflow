# !!!INVESTIGATE!!!

# PROBLEM DESCRIPTION

- you have to follow `REPO-ROOT/.github/Guidelines/Coding.md` when coding.
- you have to run unit test to make sure your change works.
- you have to follow complete instructions in `.github\Rules\verify-and-commit.md` to properly finish any task, before doing the next task.
  - It is important to do task one by one strictly, by me designing tasks in this way, we can achieve:
  - Easy-to-understand commits for file changing that is easy to review.
  - Limit side effects so that you don't have to deal with massive of issues at the same time.

## Task 1

Update the current sample `Rpc/Inheritance`, the `IDerived` implementation of `SetOneValue` and `SetTwoValue`, raise exceptions "DoNotSetOneValue" and "DoNotSetTwoValue".
In the `clientMain` function, just before returning `s`, add these code:
```Workflow
try { derived.SetOneValue() } catch (ex) { s = $"$(s)[$(ex)]"; }
try { derived.SetTwoValue() } catch (ex) { s = $"$(s)[$(ex)]"; }
```

And add additional postfix `[DoNotSetOneValue][DoNotSetTwoValue]` to `IndexRpc.txt`.

In order to make this work, you will have to declare a reflectable struct right below `RpcObjectReference`:
```C++
struct RpcException
{
  WString message;
}
```

It should also be JSON serialized just like `RpcException`. Following thing will be affected:
- In `TODO_RPC_Definition.md`, you have to add that `RpcException` and `RpcObjectReference` could not be in any function return values, or function/event arguments.
  - Implement it.
  - Make three errors, one for return value, two for function/event arguments.
  - Error code begins with H, continue the H series numbers.
  - You will need to add enough AnalyzerError for it.
- In `TODO_RPC_Json.md` you have to mention that `RpcException` is serialized just like `RpcObjectReference` in the standard way, and fix the `## Expected format of generated .d.ts files` section.
  - Implement it.
- In `IRpcObjectOps::InvokeMethod` which calls the actual method, you need to try-catch it, and when exception happens, return `RpcException`. Because `RpcException` will never be the return type, it is safe without ambiguity.
- During serialization, handle it properly.
- In a wrapper calling `IRpcObjectOps::InvokeMethod`, if the return value is `RpcException`, it raises an exception with that message, and `EndInvokeMethod` don't need to call.

At the end, since new JSON protocols are added, you need to go to `Test/TypeScript`, run `prepare.ps1` followed by `npm run build`.
And make sure these documents are updated:
- `TODO_RPC_Definition.md` mentioning the new error.
- `TODO_RPC_Json.md` mentioning additional JSON serialization thing.
- `TODO_RPC_GeneratedWrappers.md` if necessary.

# UPDATES

# TEST [CONFIRMED]

Update the existing `Rpc\Inheritance` sample so the derived wrapper calls inherited methods that intentionally raise service-side exceptions. The expected `IndexRpc.txt` result should become `[][One][][Two][][Derived][DoNotSetOneValue][DoNotSetTwoValue]`, proving exceptions raised by the callee cross the RPC wrapper boundary and surface in `clientMain`.

Add AnalyzerError samples for forbidden RPC internal transport types:
- method return values using `system::RpcObjectReference` or `system::RpcException` should report the return-value H-series error;
- method arguments using those types should report the function-argument H-series error;
- event arguments using those types should report the event-argument H-series error.

Success criteria:
- `CompilerTest_LoadAndCompile` accepts the updated `Rpc\Inheritance` sample after implementation and rejects the new AnalyzerError samples with the expected H-series prefixes.
- Runtime and generated C++ RPC execution return the updated inheritance postfix.
- JSON RPC wrapper generation includes `system::RpcException` alongside `system::RpcObjectReference`, and `Test\TypeScript\prepare.ps1` followed by `npm run build` passes.
- Required unit test projects pass according to `Project.md` and `.github\Rules\verify-and-commit.md`.

# PROPOSALS

- No.1 Add `RpcException` as an internal RPC transport result and reject it from user RPC signatures [CONFIRMED]

## No.1 Add `RpcException` as an internal RPC transport result and reject it from user RPC signatures

### CODE CHANGE

Implemented `system::RpcException` as a reflectable RPC-internal transport struct next to `RpcObjectReference`, including library reflection registration, JSON serialization support, and generated TypeScript declaration output.

Updated RPC validation so `RpcObjectReference` and `RpcException` are rejected anywhere they would conflict with transport semantics:
- H10 reports reserved transport types in RPC return values and RPC property values.
- H11 reports reserved transport types in RPC method arguments.
- H12 reports reserved transport types in RPC event arguments.

Updated normal and JSON RPC wrapper generation:
- service-side `IRpcObjectOps::InvokeMethod` and JSON `InvokeMethod` wrap actual method calls in `try/catch`;
- caught exceptions are returned as `RpcException { message = ex.Message }`;
- client-side generated wrappers detect the returned `RpcException`, raise its message locally, and therefore skip `EndInvokeMethod` naturally on exception paths.

Updated `Rpc/Inheritance` so the derived implementation raises `DoNotSetOneValue` and `DoNotSetTwoValue`, and the client appends the caught exception messages before returning. The catch blocks use `ex.Message` because Workflow catch variables are `system::Exception^`.

Updated documentation in `TODO_RPC_Definition.md`, `TODO_RPC_Json.md`, and `TODO_RPC_GeneratedWrappers.md`, including the reserved-type errors and `RpcException` JSON/DTS behavior.

Added AnalyzerError samples for both `RpcObjectReference` and `RpcException` in return, method argument, and event argument positions, and regenerated the RPC/text/C++ outputs.

### CONFIRMED

The proposal is confirmed. The updated analyzer rejects reserved RPC transport structs with the new H10, H11, and H12 errors, and the updated RPC inheritance sample proves service-side exceptions cross generated wrappers and reappear as local Workflow exceptions.

Verification passed:
- Debug build: Win32 and x64.
- `LibraryTest`: Win32 and x64, 14/14.
- `CompilerTest_GenerateMetadata`: Win32 and x64, 2/2.
- `CompilerTest_LoadAndCompile`: x64.
- `RuntimeTest`: Win32 and x64, 258/258. `Rpc/Inheritance` actual output matched `[][One][][Two][][Derived][DoNotSetOneValue][DoNotSetTwoValue]`.
- `CppTest`, `CppTest_Metaonly`, and `CppTest_Reflection`: Win32 and x64, each 224/224.
- `Test\TypeScript\prepare.ps1`.
- `npm run build` in `Test\TypeScript`.
