investigate repro

- you have to follow `REPO-ROOT/.github/Guidelines/Coding.md` when coding.
- you have to run unit test to make sure your change works.
- you have to follow complete instructions in `.github\Rules\verify-and-commit.md` to properly finish any task, before doing the next task.
  - It is important to do task one by one strictly, by me designing tasks in this way, we can achieve:
  - Easy-to-understand commits for file changing that is easy to review.
  - Limit side effects so that you don't have to deal with massive of issues at the same time.

## Task 1

When `IObjectEventOps::InvokeEvent` does not raise exception, it should return null.
When it raises exception, the exception message should be formatted like `clientId:message;...`. The last semicolon should exist, it is not just a delimeter.
But the wrapper should still verify if it is not null and not empty to raise the exception.
It will affect the last two block of `Rpc/Inheritance` sample's expected test result in `IndexRpc.txt`.

## Task 2

I would like to split `Rpc/Inheritance` sample into 3, keep the `s` thing, All 4 interfaces and their inheritance relationship remain in all 3 samples:
- `Rpc/Inheritance`: No event is declared. No member raise exception and no try-catch (except `IDerived::Set(One|Two)Value`, they still raise exceptions, to make sure they will not be called in the test case). No `GuardCrashAtClient` is declared.
- `Rpc/Inheritance_MethodException`: No event is declared, `CreateOne`, `CreateTwo`, `GuardCrashAtClient` is not declared. `Value` property and `SetDerivedValue` is not declared. It only calls `Set(One|Two)Value` and test their exceptions.
- `Rpc/Inheritance_EventException`: Deleting all members except events and `GuardCrashAndClient`. It only trigger two events and test their exceptions.

All 3 samples should remain calling involved members just like the original sample, so the first one leaving 4 `[]`, and the other leaving 2 `[]`, in the test result.

## Task 3

Now `IRpcListEventOps::OnItemChanged` will accept exceptions just like `IObjectEventOps::InvokeEvent`.
You need to create a `Rpc/Oblist_EventException`, the `IServer::GetOblist` creates an `observe int[]`, handle its `ItemChanged` event to raise an exception (need to format all arguments to the exception message), save it in a field and return it. Now `clientMain` inside a try-catch, it adds 0 to the list, triggering `ItemChanged` and catch the exception and return.

Unlike object and wrappers, container related stuff are all implemented in C++, mainly in `WfLibraryRpcWrappers.(h|cpp)`.
You can register a static function `IRpcLifecycle::ReadEventException`, in both object wrappers and container wrappers, call it with the return value from `InvokeEvent` and `OnItemChanged`:
- When the argument is null or is empty, nothing happens.
- When the argument has anything, format the exception message and raise it, using `vl::Exception`.
- In this way we DRY.

Remember to update documents and `Rpc.d.ts`.
