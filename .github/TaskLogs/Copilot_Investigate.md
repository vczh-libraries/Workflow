# !!!INVESTIGATE!!!

# PROBLEM DESCRIPTION

- you have to follow `REPO-ROOT/.github/Guidelines/Coding.md` when coding.
- you have to run unit test to make sure your change works.
- you have to follow complete instructions in `.github\Rules\verify-and-commit.md` to properly finish any task, before doing the next task.
  - It is important to do task one by one strictly, by me designing tasks in this way, we can achieve:
  - Easy-to-understand commits for file changing that is easy to review.
  - Limit side effects so that you don't have to deal with massive of issues at the same time.

## Task 1

Update the current sample `Rpc/Inheritance`, add following events to `IOne` and `ITwo`, and add `GuardCrashAtClient` to `IDerived`:
```Workflow
interface IOne
{
  event CrashAtServer();
}

interface ITwo
{
  event CrashAtClient();
}

interface IDerived : IOne, ITwo
{
  func GuardCrashAtClient() : string;
}
```

In `CreateDerived`, the implementation will attach a handler to `CrashAtServer` and raise `"CrashedAtServer"`.
In `clientMain`, attach a handler to `CrashAtClient` and raise `"CrashedAtClient"`.

Now we have to call them to test the exception. `clientMain` calls them like this:
```Workflow
try { derived.CrashAtServer(); } catch (ex) { s = $"$(s)[$(ex)]"; }
s = $"$(s)[$(derived.GuardCrashAtClient())]";
```

`GuardCrashAtClient` calls the event and returns the exception message:
```Workflow
override func GuardCrashAtClient() : string
{
  try { CrashAtClient(); } catch (ex) { return ex; }
  return "";
}
```

New calls will be added right before returning `s`, therefore add additional postfix `[CrashedAtServer][CrashedAtClient]` to `IndexRpc.txt`.

----------------------------

In order to make this work, update `IRpcObjectEventOps::InvokeEvent` to return `system::RpcException[int]` instead of `void`.
`RpcException` already exists. The key of the returned dictionary is the client id of the lifecycle whose event handler raised an exception. The value is the `RpcException` containing that exception message.

The expected behavior is:
- An event broadcast must attempt to deliver the event to every lifecycle that should receive it.
- If only part of the lifecycles raise exceptions while handling the event, the result dictionary contains only those lifecycles.
- If no lifecycle raises an exception, the result dictionary is empty.
- If multiple lifecycles raise exceptions, the lifecycle that triggered the event receives all of them together, keyed by client id.
- Exceptions from one lifecycle must not prevent other lifecycles from receiving the same event.

Following things will be affected:
- In `IRpcObjectEventOps::InvokeEvent`, which calls the actual event, you need to try-catch it, and when exception happens, return a dictionary containing one item: `_lc.ClientId` to `RpcException`.
  - Keep the event suppression flag set before raising the local event.
  - Keep clearing the event suppression flag in `finally`, even when an exception is caught and converted to the dictionary.
  - Unknown event ids should still behave consistently with the new return value path.
- In a generated local event listener, after calling `IRpcDispatcher::BroadcastFromClient_ObjectEventOps(...)->InvokeEvent(...)`, check the returned `RpcException[int]`.
  - If the dictionary is empty, finish normally.
  - If the dictionary is not empty, raise an exception on the lifecycle that triggered the event.
  - The returned dictionary is an internal transport value. It must not be exposed to Workflow script catch variables; `catch (ex)` still receives a string exception message.
- In the dispatcher or broadcast object-event ops implementation, aggregate all dictionaries returned from target lifecycles.
  - Do not stop at the first returned exception.
  - Preserve the client id keys from every target lifecycle.
  - This should work when only one lifecycle raises and when multiple lifecycles raise.
- During JSON serialization, reuse the existing support for serializing `RpcException[int]`.
  - The dictionary key is the lifecycle client id.
  - JSON event receiving should deserialize arguments before raising the local event, catch exceptions into the returned dictionary, and serialize the dictionary back to the trigger lifecycle.
  - JSON event sending should deserialize the returned dictionary before deciding whether to raise an exception.
- Update generated wrappers and reflection declarations to use the new return type of `IRpcObjectEventOps::InvokeEvent` everywhere.
- Update mocks and tests that implement or wrap `IRpcObjectEventOps`, including JSON mocks, so they aggregate and forward event exceptions instead of dropping them.

At the end, make sure these documents are updated:
- `TODO_RPC_GeneratedWrappers.md` mentioning how generated object event ops return `RpcException[int]` and how generated event senders raise exceptions for non-empty dictionaries without exposing client ids to Workflow script.
- `TODO_RPC_Definition.md` if the definition document needs to mention the new event exception behavior.

`TODO_RPC_Json.md` does not need an update for this task, because the exception map uses the existing JSON serialization support.

# UPDATES

# TEST [CONFIRMED]

Update the existing `Rpc\Inheritance` sample so inherited RPC events can throw on both service and client lifecycles:
- `CrashAtServer` is handled in `CreateDerived` and raises `CrashedAtServer`.
- `CrashAtClient` is handled in `clientMain` and raises `CrashedAtClient`.
- `clientMain` catches the server-side event exception directly from `derived.CrashAtServer()`.
- `GuardCrashAtClient` catches the client-side event exception raised while the service triggers `CrashAtClient()`.

The expected `IndexRpc.txt` result should become `[][One][][Two][][Derived][DoNotSetOneValue][DoNotSetTwoValue][CrashedAtServer][CrashedAtClient]`, proving generated event senders convert a non-empty internal `system::RpcException[int]` map into a normal Workflow string exception message without exposing client ids.

Success criteria:
- Event broadcasts deliver to all target lifecycles even when one or more handlers throw, aggregating returned exception maps by lifecycle client id.
- `CompilerTest_LoadAndCompile` accepts the updated `Rpc\Inheritance` sample and regenerates RPC wrappers with `IRpcObjectEventOps::InvokeEvent` returning `system::RpcException[int]`.
- Runtime and generated C++ RPC execution return the updated inheritance postfix.
- Existing JSON RPC tests/mocks continue to compile and pass, using existing JSON support for `system::RpcException[int]`.
- Required unit test projects pass according to `Project.md` and `.github\Rules\verify-and-commit.md`.

# PROPOSALS

- No.1 Return and propagate object event exception maps [CONFIRMED]

## No.1 Return and propagate object event exception maps

### CODE CHANGE

Updated the RPC object-event transport so `IRpcObjectEventOps::InvokeEvent` returns the internal exception map `system::RpcException[int]` instead of `void`.

Implemented exception-map construction and merging in the RPC library layer, updated reflection declarations and C++ proxy glue for the new return value, and adjusted generated normal and JSON RPC wrappers to:
- set and clear event suppression around local event invocation using `try`/`finally`;
- catch local event handler exceptions and return `{ _lc.ClientId : RpcException { message = ex.Message } }`;
- aggregate all returned exception maps during event broadcast without stopping at the first failing lifecycle;
- raise a normal Workflow string exception at the triggering lifecycle when the aggregated map is non-empty, without exposing lifecycle client ids to script `catch (ex)` variables.

Updated JSON RPC object-event receiving and sending to serialize and deserialize the returned exception map with the existing `RpcException[int]` JSON support.

Updated RPC mocks used by unit tests so object-event broadcasts aggregate exception maps and JSON mock dispatching wraps the broadcast ops rather than assuming a lifecycle-local object-event ops instance.

Updated `Rpc/Inheritance`:
- `IOne` declares `CrashAtServer`;
- `ITwo` declares `CrashAtClient`;
- `IDerived` declares `GuardCrashAtClient`;
- `CreateDerived` raises `CrashedAtServer` from the service-side event handler;
- `clientMain` raises `CrashedAtClient` from the client-side event handler and appends both caught messages.

Updated `TODO_RPC_GeneratedWrappers.md` and `TODO_RPC_Definition.md` with the new object-event exception behavior. `TODO_RPC_Json.md` was intentionally unchanged because the feature reuses existing exception-map JSON serialization.

Regenerated metadata, Workflow text outputs, and C++ RPC generated outputs.

### CONFIRMED

The proposal is confirmed. The updated `Rpc/Inheritance` sample returns `[][One][][Two][][Derived][DoNotSetOneValue][DoNotSetTwoValue][CrashedAtServer][CrashedAtClient]`, proving both service-side and client-side event handler exceptions cross the object-event broadcast path and surface as ordinary Workflow string exceptions.

Verification passed:
- Debug build: Win32 and x64.
- `CppTest`: Win32 and x64, 224/224.
- `CppTest_Metaonly`: Win32 and x64, 224/224.
- `CppTest_Reflection`: Win32 and x64, 224/224.
- `RuntimeTest`: Win32 and x64, 258/258.
- `CompilerTest_LoadAndCompile`: Win32 and x64, 706/706 for both generated target passes.
- `CompilerTest_GenerateMetadata`: Win32 and x64, 2/2.
- `LibraryTest`: Win32 and x64, 14/14.
- `Test\TypeScript\prepare.ps1`.
- `npm ci` in `Test\TypeScript`, then `npm run build`.
