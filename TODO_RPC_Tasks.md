investigate repro

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
