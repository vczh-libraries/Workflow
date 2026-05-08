# !!!INVESTIGATE!!!

# PROBLEM DESCRIPTION

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

After finishing everything, git commit and push directly.

# UPDATES

# TEST [CONFIRMED]

Confirm the new TypeScript schema against the C++ RPC ops surface in `Source/Library/Rpc/WfLibraryRpc.h`:
- all methods from `IRpcListOps`, `IRpcObjectOps`, `IRpcListEventOps`, and `IRpcObjectEventOps` appear in `Request<T>` and `Response<T>`;
- request envelopes for `SendToClient_*` ops include `targetClientId`;
- request envelopes for `BroadcastFromClient_*` event ops omit `targetClientId`;
- all response envelopes include both `sourceClientId` and `targetClientId`;
- only interfaces with serialized generic JSON payloads use `<T>`.

Success criteria:
- `Test/TypeScript/Rpc.d.ts` type-checks by itself.
- `Test/TypeScript/prepare.ps1` succeeds.
- `npm run build` succeeds in `Test/TypeScript`.

# PROPOSALS

- No.1 Add shared TypeScript RPC dispatcher schema [CONFIRMED]

## No.1 Add shared TypeScript RPC dispatcher schema

### CODE CHANGE

Created `Test/TypeScript/Rpc.d.ts` with shared TypeScript declarations for dispatcher request and response messages. The file declares stable internal transport structs directly and uses generic `T` only where the payload comes from generated JSON serialization schemas.

The schema covers all methods from the four C++ ops interfaces:
- `IRpcListOps`
- `IRpcObjectOps`
- `IRpcListEventOps`
- `IRpcObjectEventOps`

Updated `TODO_RPC_Json.md` with the rule that generated `Serialization_*.d.ts` files own concrete value schemas, while `Rpc.d.ts` owns the dispatcher envelope schema and connects to generated schemas by instantiating `T` as `KnownTypeSchema | UnknownTypeSchema`.

### CONFIRMED

The proposal is confirmed. Verification passed:
- `npm exec tsc -- --ignoreConfig --noEmit --strict Rpc.d.ts --moduleResolution node16 --module node16 --target ES2020`
- `Test/TypeScript/prepare.ps1`
- `npm run build` in `Test/TypeScript`
