# Repro

You should complete tasks one by one.
`General Instruction` is for each tasks, which means, during doing each task:
- you have to run unit test to make sure your change works.
- you have to follow the instruction to commit and push for each task, before doing the next task.

## Task 1

- In generated `rpc_IRpcObjectOps`, `RequestService` and `RegisterService` is doing switch-case on type ids.
  - A switch-cast should be generated at the beginning of the `RegisterService` function to crash if the type id does not exist or the type id is not `@rpc:Ctor` decorated. The rest of the `RegisterService` is just binding an object with the type id into the map. And `RequestService` doesn't need to care about the type id at all, because illegal type id cannot register.
  - In `RegisterService`, two kinds of errors are expected: type id is not an interface with `@rpc:Ctor`; type id does not exists.
    - You are going to use your own words for them.
    - Two helper functions could be created: `if it is ctor type do the work; if it is valid type id raise error 1 otherwise raise error 2;`.
  - In `RequestService`, you should check first if the type id exists, checkout reflection code for `IValueDictionary` for details.
    - If a type id is not registered, raise an error, don't need to care if the type id is valid or not.

## General Instruction

If any test case fail, you could continue to run until you collect results from all `RuntimeTest` and `CppTest*`. By seeing if a failure exists in all projects or only some projects, you will have a better guess of the root cause.
- Pass all unit test, fix any test failure including pre-existings.
- After finishing everything, git commit and git push to the current branch.
- If in any task you are adding new test sample, or modifying any C++ or Workflow generation code, causing a huge amount of files generated from test samples to change:
  - Two commits are required. First commit only has all modified files and files you created directly, second commit has all new files that not created by you (aka auto generated)
  - Otherwise, One commit is good.
- DO NOT ASK ME ANY QUESTION, I will not be watching you, you must make your best decision and run through the end.
