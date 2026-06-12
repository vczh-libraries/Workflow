investigate repro

DO NOT ASK ME ANY QUESTION, I will not be watching you, you must make your best decision and run through the end.
Follow `REPO-ROOT/.github/Rules/document-and-commit.md` to finish the work.

## Task 1

Overfile of refactoring of `ChatBot(Server|Client)` projects:
- Three classes `ChatBotJsonDispatcher*` is going to be reorganize into only `ChatBotJsonDispatcherClient` and `ChatBotJsonDispatcherServer`.
- One shared header `ChatBotHttp.h` to store `RpcChannel`, `ChatBotHttpBaseUrl` and `ChatBotHttpPort`.
- Currently `ChatBotJsonDispatcherServer` hosts `IChatServer`:
  - It no longer host any server, instead `RpcJsonDispatcher::DeclareLocalService` will do actual job for service declaring.
  - The `ChatBotServer` project will need two local client now:
    - The first one is `ChatBotJsonDispatcherServer`.
    - The second one is a `ChatBotJsonDispatcherClient` which hosts `IChatServer`, nothing special to do, everything should be already handled by `IRpcLifecycle` and `IRpcDispatcher` implementations.
    - `ChatBotServer` project will report the local client id of `ChatBotJsonDispatcherServer` as the client id:
      - This should be the first `IChannelServer::ConnectLocalClient` call.
      - And then report the client id so that it could be sent to all subsequent connecting `ChatBotJsonDispatcherClient`.
      - And then register the local `ChatBotJsonDispatcherClient` for `IChatServer`.
      - And then call `IChannelServer::Start`, since before calling this function all clients can't really try to connect, it ensures remote clients come after.
- The overall message sequences is not affected, but now `Request:IRpcDispatcher_DeclareRemoteService` kicks in, which is the reason of the refactoring.

`IRpcDispatcher::DeclareLocalService`:
- The only argument of `IRpcDispatcher::DeclareLocalService` becomes `RpcObjectReference`.
- The only argument of `IRpcLifecycle::DeclareRemoteService` becomes `RpcObjectReference`.
  - I think the current `DeclareRemoteService` is not correct as it actually miss the most important data (objectId), I am really astonished that the implementation could handle that, but it is wrong.
  - It should just stores the map `typeId->ref` and call `RefToPtr` on that stored ref directly.
- `RpcJsonDispatcher::DeclareLocalService` generates `Request:IRpcDispatcher_DeclareRemoteService` instead.
  - `Rpc.d.ts` needs to update, there is no more `Request:IRpcDispatcher_DeclareLocalService`.
  - `ref` will be directed used in this TypeScript interface.
  - `RpcJsonDispatcher::Translate` needs to update.

`IRpcJsonMessageDispatcher::JsonRequest`:
- The second argument become an enum class `RequestType` with `Direct, Broadcast, BroadcastAndDrop`:
  - `Direct` sends the message to specific client.
  - `Broadcast` notices everyone, it is sent to `ChatBotJsonDispatcherServer` which do the real broadcasting, consolidating all response and response.
    - `Request:IObjectEventOps_InvokeEvent` becomes such request.
  - `BroadcastAndDrop` notices everyone, it it sent to `ChatBotJsonDispatcherServer` which do the real broadcasting, but no consolidating responses, client will also not response to such request.
    - `Request:IRpcDispatcher_DeclareRemoteService` becomes such request.
    - `RpcDualJsonRequestBridge::JsonRequest` it returns nullptr on such request.
      - I think a better way is to make `IRpcJsonMessageDispatcher::DefaultDispatch` (will be renamed to `DefaultTranslate`) returns nullptr, because this function already parses the request.
    - `ChatBotJsonDispatcherClient::JsonRequest` will no longer do the loop until getting a response when handling `BroadcastAndDrop` requests, it should returns nullptr directly after sending out the request.

`ChatBotJsonDispatcherClient`:
- `ChatBotJsonDispatcherBase` merges into this class.
- The synchronization of `JsonRequest` does not change.
- Expose `WaitForServer` and `ConnectLocalClient` function, accepting an `IChannel`, which will register itself as a `IChannelReader`, doing the following works **in this order**
  - This function can only be called once, otherwise `CHECK_FAIL`.
  - It accepts a `const List<WString>& _waitingForServices`, copy to a field `waitingForServices`.
  - For `WaitForServer`:
    - It accepts extra `IChannelClient` as an argument, and call `WaitForServer` in this function.
  - For `ConnectLocalClient`:
    - It accepts extra `IChannelServer` as an argument, and call `RegiterLocalClient` in this function.
  - This part is better to be in a function for sharing:
    - When `waitingForServices` is not empty, `EventObject::CreateManualSignal` is called on field `eventWaitingForServices` and wait.
    - Upon receiving `Request:IRpcDispatcher_DeclareRemoteService`, when any type id matches the type name, the type name is removed from `waitingForServices`.
    - When `waitingForServices` becomes empty, the event is signaled.
    - `IRpcLifecycle::GetTypeIdFromName` needs to be added:
      - It returns `RpcTypeId_NotFound` for inexisting types.
      - `RpcLifecycleBase` implements it, it uses the map from `SetIdMap`.
      - `GetTypeIdFromName` will be useful here.
      - Scan everywhere in `Source/Rpc` and files affected here, use `GetTypeIdFromName` as a standard way to translate from type name to type id:
        - Especially checking all friended classes, and if type name translation is the only reason, you can remove the class from friend.
- Processing `Request:IRpcDispatcher_DeclareRemoteService` in `JsonRequest`
  - Before calling `Initialize`, `JsonRequest` will caches all `Request:IRpcDispatcher_DeclareRemoteService` messages, `CHECK_FAIL` on all other messages.
  - When calling `Initialize`, all cached `Request:IRpcDispatcher_DeclareRemoteService` will be sent directly.
  - After calling `Initialize`, all `Request:IRpcDispatcher_DeclareRemoteService` will be sent directly, no more caching is needed.
  - **IMPORTANT** The above rule is for the message comming from the argument, `IRpcJsonMessageDispatcher::DefaultTranslate` is called on messages retrieved from the list, do not mix them up.
    - It is currently named `DefaultDispatch` and for naming consistency you should rename it to `DefaultTranslate`.

`ChatBotJsonDispatcherServer`:
- It no longer share any code with `ChatBotJsonDispatcherClient`, and there is no more `ChatBotJsonDispatcherBase` anyway.
- When `Request:IRpcDispatcher_DeclareRemoteService` is called, all such messages will be cached.
- When a new client joined, the first message will still be the client id of `ChatBotJsonDispatcherServer`, followed by all cached `Request:IRpcDispatcher_DeclareRemoteService`.
- The `ChatBotJsonDispatcherClient` running in the server, which is a local client of the server, also talks to `ChatBotJsonDispatcherServer`.

`RpcDualJsonRequestBridge` would be affected.

## Task 2

- `ChatBotJsonDispatcher(Client|Server)` will be stored in `ChatBotJsonDispatcher(Client|Server).(h|cpp)`, remove the current file.

`TODO_RPC_JsonRequest.md` is created, it describes how the server and the client should response to all requests in `Rpc.d.ts`:
- Mention this file in `document-and-commit.md` as well as any other places that mentioning the `TODO_RPC_(Definition|GeneratedWrappers|Json).md` list.
- No need to mention class names or project names of `ChatBot*`, the document is for developers who want to implement RPC in other languages.
- No need to mention how Workflow interfaces map to responses, since explanation of `@rpc:` attributes are covered in other documents, but all concepts need precise and detailed explanation.
- Need to mention handling of lifecycle.
- Need to mention expected request sequences.
- Need to mention three kinds of request.
- Anything that you think is necessary.

To ensure the refactoring works, all test cases should pass, and the standard procedure of testing `ChatBot(Server|Client)` is also required.
