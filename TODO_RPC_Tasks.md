# Repro

You should complete tasks one by one.
`General Instruction` is for each tasks, which means, during doing each task:
- you have to run unit test to make sure your change works.
- you have to follow the instruction to commit and push for each task, before doing the next task.

## Task 1

In generated wrapper Workflow scripts:
- Rename `rpcsvc_RaiseInvalidServiceTypeId` to `rpcwrapper_IsInterfaceTypeId`.
  - It returns `bool` instead of raising an exception, the exception should be raised at the caller side.
  - The case from -1 to -7 could becomes an if statement before the switch case says `if (typeId in range [-7, -1]) ...`.
- Rename `rpcsvc_EnsureCtorServiceTypeId` to `rpcwrapper_IsCtorInterfaceTypeId`.
  - It returns `bool` instead of raising an exception, the exception should be raised at the caller side.
- Therefore in `RegisterService` it could says `if rpcwrapper_IsCtorInterfaceTypeId do the work else if rpcwrapper_IsInterfaceTypeId raise not a ctor interface otherwise raise invalid type id`.
- You would also need to fix other codegen places that affected by this change.

## General Instruction

If any test case fail, you could continue to run until you collect results from all `RuntimeTest` and `CppTest*`. By seeing if a failure exists in all projects or only some projects, you will have a better guess of the root cause.
- Pass all unit test, fix any test failure including pre-existings.
- After finishing everything, git commit and git push to the current branch.
- If in any task you are adding new test sample, or modifying any C++ or Workflow generation code, causing a huge amount of files generated from test samples to change:
  - Two commits are required. First commit only has all modified files and files you created directly, second commit has all new files that not created by you (aka auto generated)
  - Otherwise, One commit is good.
- DO NOT ASK ME ANY QUESTION, I will not be watching you, you must make your best decision and run through the end.
