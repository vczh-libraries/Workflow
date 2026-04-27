# Continue

You should complete tasks one by one.
`General Instruction` is for each tasks, which means, during doing each task:
- you have to run unit test to make sure your change works.
- you have to follow the instruction to commit and push for each task, before doing the next task.

## Task 1

Checkout commit `c8cc8579349c0d827ac45bab50049b0d9d825af4` you will see, in order to complete `DtorList.txt` and `DtorList2.txt`, `IRpcLocalMutationTarget` is added.
The test case is about how elements in containers can be stably deleted.
You can check out the content of existing Copilot_Investigate.md to understand what happened. Copilot_Investigate.md is not changed in all later commits so you can just read it directly. 

My thought is that, in `Dtor[23]?.txt` we proved that, only when both local object and wrapper is not hold by both side, the local object will get destructed.
`Dtor3.txt` is about a local object holding a wrapper (remote object), and it works well.
So obviously, only when both local container and wrapper container is not hold by both side, the local container will get destructed.
And that's when elements in the container destructed, no matter if elements are wrapper or not.

So the added `IRpcLocalMutationTarget` looks wired to me. I expect a solution that no extra construction is added.
I would like you to dig deeper into the source code, find the root cause, and try to avoid `IRpcLocalMutationTarget`.

I believe Workflow compiler and code generation should be good, so they should be the least thing you want to change.

## General Instruction

If any test case fail, you could continue to run until you collect results from all `RuntimeTest` and `CppTest*`. By seeing if a failure exists in all projects or only some projects, you will have a better guess of the root cause.
- Pass all unit test, fix any test failure including pre-existings.
- After finishing everything, git commit and git push to the current branch.
- If in any task you are adding new test sample, or modifying any C++ or Workflow generation code, causing a huge amount of files generated from test samples to change:
  - Two commits are required. First commit only has all modified files and files you created directly, second commit has all new files that not created by you (aka auto generated)
  - Otherwise, One commit is good.
  - Typical files that are generated: `Test\Generated`, `Test\SourceCppGen`, `Test\SourceCppGenRpc`, `Test\UnitTest\Generated_*`.
- DO NOT ASK ME ANY QUESTION, I will not be watching you, you must make your best decision and run through the end.
