investigate repro

DO NOT ASK ME ANY QUESTION, I will not be watching you, you must make your best decision and run through the end.
Follow `REPO-ROOT/.github/Rules/document-and-commit.md` to finish the work.

## Task 1

Refactor `RpcDualJsonRequestDispatcherMock` to use `RpcDualDispatcherMockBase` to avoid duplicating code.
To achieve this, the following change will be needed.
Currently two clients have its own `RpcDualJsonRequestDispatcherMock` which connects with one `RpcDualJsonMessageBridge` instance.
I would like you to refactor the code so that there will be only one `RpcDualJsonRequestDispatcherMock` and also no `RpcDualJsonMessageBridge` is needed.
`BroadcastFromClient_ObjectEventOps` and `SendToClient_ObjectOps` should return one in two `RpcJsonObject(Event)?Ops` according to the specified client id.

In `WfLibraryRpcJson.cpp`, you are going to find out how many helper functions do their own serialization, and replace them to use, or replace them completely with `JsonSerializePredefinedTypes` and `JsonDeserializePredefinedTypes`. Even if these pair of functions do more thing than the original function. If you found nothing is repeating, it is also good. Note than `JsonSerializePredefinedTypes` and `JsonDeserializePredefinedTypes` also use other functions, no need to inline them, just keep the current way.

## Task 2

In `WfAnalyzer_(Validate|Generate)Rpc*.cpp` there is a lot of Workflow AST generation code.
Many of them generate types from scratch.
For types that can be directly expressed in C++, there is a better way: C++ type -> ITypeInfo -> WfType helper.
I would like you to identify all of this and change the code, and delete unused helper functions completely.
Wrappers generates many *.txt files during running the compiler, you will be easy to ensure success, by see no *.txt file is changed using `git status`.

`TypeInfoRetriver<T>::CreateTypeInfo()` converts C++ type `T` to `Ptr<ITypeInfo>`.
`GetTypeFromTypeInfo` converts `ITypeInfo*` to `Ptr<WfType>`.
When possible, use `TypeInfoRetriver` inside `GetTypeFromTypeInfo` to avoid creating unnecessary termporary variables.
