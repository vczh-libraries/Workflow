investigate repro

- you have to follow `REPO-ROOT/.github/Guidelines/Coding.md` when coding.
- you have to run unit test to make sure your change works.
- you have to follow complete instructions in `.github\Rules\verify-and-commit.md` to properly finish any task, before doing the next task.
  - It is important to do task one by one strictly, by me designing tasks in this way, we can achieve:
  - Easy-to-understand commits for file changing that is easy to review.
  - Limit side effects so that you don't have to deal with massive of issues at the same time.

## Task 1

You are going to create Test/TypeScript/Rpc.d.ts. You must read TODO_RPC_(Json|Definition).md with source codes in Source/Library/Rpc for more understanding of the interface and schema

Rpc.d.ts basically contains a JSON schema that is supposed to represent IRpcDispatcher. For example, we can call IRpcDispatcher::BroadcastFromClient_ObjectEventOps::InvokeEvent`, and in the document we knows it either returns null or RpcException[int]. and such dictionary will be serialized to `null | [number, system_RpcException][]`. So the schema of the type looks like this

```TypeScript
export interface IObjectEventOps_InvokeEvent_Request<T>
{
  rpcMethod: "IObjectEventOps_InvokeEvent";
  sourceClientId : number;
  // SendToClient::* would have an additional targetClientId because this is not broadcasting
  ref : system_RpcObjectReference;
  eventId : number;
  arguments: T[];
}

export interface IObjectEventOps_InvokeEvent_Response
{
  rpcMethod: "IObjectEventOps_InvokeEvent";
  // a response is sent from the client being requested to the client requesting.
  // so it is one to one regardless SendToClient::* or BroadcastFromClient::*
  sourceClientId : number;
  targetClientId : number;
  response : null | [number, system_RpcException][]; // for functions returning void, there will be no response
}

export type Request<T> =
  | IObjectEventOps_InvokeEvent_Response<T>
  | ...
  ;

export type Response =
  | IObjectEventOps_InvokeEvent_Response
  | ...
  ;
```

Both `Request` and `Response` should contains all methods in all 4 ops interfaces.
`T` would be `KnownTypeSchema | UnknownTypeSchema`, but since the `Rpc.d.ts` is shared but schemas are generated, so there is no choice but to use generic.
You will have to determine of `T` will be used in any interface or not, only add `<T>` if yes.

Update `TODO_RPC_Json.md` about `Type/TypeScript/Rpc.d.ts`, and rules of how `.d.ts` connects to C++ interfaces using your own words.

## Task 2

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
