investigate repro

DO NOT ASK ME ANY QUESTION, I will not be watching you, you must make your best decision and run through the end.
Follow `REPO-ROOT/.github/Rules/document-and-commit.md` to finish the work.

## Task 1

Overfile of refactoring of `ChatBot(Server|Client)` projects:
- Three classes `ChatBotJsonDispatcher*` is going to be reorganize into only `ChatBotJsonDispatcherClient` and `ChatBotJsonDispatcherServer`.
- They will be stored in `ChatBotJsonDispatcher(Client|Server).(h|cpp)`, remove the current file.
- One shared header `ChatBotHttp.h` to store `RpcChannel`, `ChatBotHttpBaseUrl` and `ChatBotHttpPort`.
- Currently `ChatBotJsonDispatcherServer` hosts `IChatServer`:
  - It no longer host any server, instead `RpcJsonDispatcher::DeclareLocalService` will do actual job for service declaring.
  - The `ChatBotServer` project will need two local client now:
    - The first one is `ChatBotJsonDispatcherServer`, it should be the first one to call `RegisterLocalClient`.
    - The second one is a `ChatBotJsonDispatcherClient` which hosts `IChatServer`, nothing special to do, everything should be already handled by `IRpcLifecycle` and `IRpcDispatcher` implementations.
- The overall message sequences is not affected, but now `Request:IRpcDispatcher_DeclareRemoteService` kicks in, which is the reason of the refactoring.

`IRpcDispatcher::DeclareLocalService`:
- The only argument of `IRpcDispatcher::DeclareLocalService` becomes `RpcObjectReference`.
- The only argument of `IRpcLifecycle::DeclareRemoteService` becomes `RpcObjectReference`.
- `RpcJsonDispatcher::DeclareLocalService` generates `Request:IRpcDispatcher_DeclareRemoteService` instead.
  - `Rpc.d.ts` needs to update, there is no more `Request:IRpcDispatcher_DeclareLocalService`.
  - `ref` will be directed used in this TypeScript interface.
  - `RpcJsonDispatcher::Translate` needs to update.

`IRpcJsonMessageDispatcher::JsonRequest`:
- The second argument become an enum class `RequestType` with `Direct, Broadcast, BroadcastAndDrop`:
  - `Direct` sends the message to specific client.
  - `Broadcasts` notices everyone, it is sent to `ChatBotJsonDispatcherServer` which do the real broadcasting, consolidating all response and response.
    - `Request:IObjectEventOps_InvokeEvent` becomes such request.
  - `BroadcastAndDrop` notices everyone, it it sent to `ChatBotJsonDispatcherServer` which do the real broadcasting, but no consolidating responses, client will also not response to such request.
    - `Request:IRpcDispatcher_DeclareRemoteService` becomes such request.
    - When `JsonRequest` receives such request, it won't block, it returns nullptr.

`ChatBotJsonDispatcherClient`:
- `ChatBotJsonDispatcherBase` merges into this class.
- The synchronization of `JsonRequest` does not change.
- Expose `WaitForServer` and `RegisterLocalClient` function, accepting an `IChannel`, which will register itself as a `IChannelReader`, doing the following works **in this order**
  - This function can only be called once, otherwise `CHECK_FAIL`.
  - It accepts a `const List<WString>& _waitingForServices`, copy to a field `waitingForServices`.
  - For `WaitForServer`:
    - It accepts `IChannelClient` as an argument, and call `WaitForServer` in this function.
  - For `RegisterLocalClient`:
    - It accepts `IChannelServer` as an argument, and call `RegiterLocalClient` in this function.
  - This part is better to be in a function for sharing:
    - When `waitingForServices` is not empty, `EventObject::CreateManualSignal` is called on field `eventWaitingForServices` and wait.
    - Upon receiving `DeclareRemoteService`, when any type id matches the type name, the type name is removed from `waitingForServices`.
    - When `waitingForServices` becomes empty, the event is signaled.
- Processing `DeclareRemoteService` in `JsonRequest`
  - Before calling `Initialize`, `JsonRequest` will caches all `DeclareRemoteService` messages, `CHECK_FAIL` on all other messages.
  - When calling `Initialize`, all cached `DeclareRemoteService` will be sent directly.
  - After calling `Initialize`, all `DeclareRemoteService` will be sent directly, no more caching is needed.
  - **IMPORTANT** The above rule is for the message comming from the argument, `IRpcJsonMessageDispatcher::DefaultTranslate` is called on messages retrieved from the list, do not mix them up.

`ChatBotJsonDispatcherServer`:
- It no longer share any code with `ChatBotJsonDispatcherClient`, and there is no more `ChatBotJsonDispatcherBase` anyway.
- When `DeclareRemoteService` is called, all such messages will be cached.
- When a new client joined, the first message will still be the client id of `ChatBotJsonDispatcherServer`, followed by all cached `DeclareRemoteService`.
- The `ChatBotJsonDispatcherClient` running in the server, which is a local client of the server, also talks to `ChatBotJsonDispatcherServer`.

`RpcDualJsonRequestBridge` would be affected.

`TODO_RPC_JsonRequest.md` is created, it describes how the server and the client should response to all requests in `Rpd.d.ts`:
- Mention this file in any instruction files that mentioning the `TODO_RPC_*.md` list.
- No need to mention class names or project names of `ChatBot*`, the document is for developers who want to implement RPC in other languages.
- No need to mention how Workflow interfaces map to responses, since explanation of `@rpc:` attributes are covered in other documents, but all concepts need precise and detailed explanation.
- Need to mention handling of lifecycle.
- Need to mention expected request sequences.
- Need to mention three kinds of request.
- Anything that you think is necessary.

To ensure the refactoring works, all test cases should pass, and the standard procedure of testing `ChatBot(Server|Client)` is also required.
