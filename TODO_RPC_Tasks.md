# Repro

You should complete tasks one by one.
`General Instruction` is for each tasks, which means, during doing each task:
- you have to run unit test to make sure your change works.
- you have to follow the instruction to commit and push for each task, before doing the next task.

## Task 1

In generated `rpclistener_*`, the last parameter should be `*` not `^`.
This includes the function for an interface, and the `rpclistener_Attach`.
This should help removing unnecessary conversion from `T*` to `Ptr<T>` in `TestCasesRpc.cpp`, as well as other places.

## General Instruction

If any test case fail, you could continue to run until you collect results from all `RuntimeTest` and `CppTest*`. By seeing if a failure exists in all projects or only some projects, you will have a better guess of the root cause.
- Pass all unit test, fix any test failure including pre-existings.
- After finishing everything, git commit and git push to the current branch.
- If in any task you are adding new test sample, or modifying any C++ or Workflow generation code, causing a huge amount of files generated from test samples to change:
  - Two commits are required. First commit only has all modified files and files you created directly, second commit has all new files that not created by you (aka auto generated)
  - Otherwise, One commit is good.
  - Typical files that are generated: `Test\Generated`, `Test\SourceCppGen`, `Test\SourceCppGenRpc`, `Test\UnitTest\Generated_*`.
- DO NOT ASK ME ANY QUESTION, I will not be watching you, you must make your best decision and run through the end.
