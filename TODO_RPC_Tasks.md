# Repro

You should complete tasks one by one.
`General Instruction` is for each tasks, which means, during doing each task:
- you have to run unit test to make sure your change works.
- you have to follow complete instructions in `.github\Rules\verify-and-commit.md` to properly finish any task, before doing the next task.

## Task 1

Due to the previous change which makes registered services no longer unregister, Dtor.txt and Dtor2.txt samples now is not working. You can check out IndexRpc.txt and find that in the long the service is no longer deleted. We need to update these samples. Because service won't release, we should change it to track another object:
- Add a `IValue` interface just like other test cases.
- Move the destructor from IService to IValue implementation.
- clientMain manipulate IValue's lifecycle instead of IValue
- In Dtor2.txt, `func GetServiceAgain() : IService^` will be changed to `func ReviewValue(value : IValue^) : IValue^`. clientMain passes the obtained value to this function, and this function returns itself. Therefore the original RequestService could be replaced by reading the argument, and all tests remain.

You should be able to revert IndexRpc.txt back to use `[Deleted]` for these two samples at the end. Check out comments in Dtor1.txt to understand what were expected to happen before the behacior changing to service lifecycle.
