# Workflow Interface Based RPC

## Goal

- When running with an interpretor or running generated code, all features should be available.
- References:
  - [Definition](./TODO_RPC_Definition.md)
  - [Scenarios](./TODO_RPC_Scenarios.md)

## ToDo

- Continue to add more test cases until all features are covered.
  - Local objects and remote objects.
    - Local objects pass to remote and pass back, remote side gets a wrapper, expect the same local object instance.
    - Remote objects pass to remote and pass back, remote side gets a local object, expect the same wrapper instance.
    - Destructor calling.
  - Function overloading.
  - Byval and byref collections.
    - A function accepts a byval or byref arguments, add one element, return it back as byval or byref arguments.
      - Byval -> byval.
      - Byval -> byref.
      - Byref -> byval.
      - Byref -> byref.
    - remote side will remember the object.
    - local side add another element.
    - check performed in boch local and remote side to see if side effects are visible or invisible as expected.
    - Including array, list, dictionary, observable list.
  - Simpler collection cases using property, testing if property attributes applied to both getter return values and setter arguments.
  - Collections of interfaces.
  - Collections of collections of primitive types.
  - Collections of collections of interfaces.
- C++ codegen for rpc data type JSON serialization.
- Solve interface inheritance and casting.
- Delete Runtime test category.

# Prompt

You must complete all following tasks. In order to make the work easier, when performing each task you should:
- Execute the task.
- Pass all unit test, fix any test failure including pre-existings.
- git commit and git push to the current branch.

Perform this per task, one after another, and only after all task is done:
- Run `..\Tools\Tools\Build.ps1 Workflow` for a complete CI.
- Pass all unit test, fix any test failure including pre-existings.
- git commit and git push to the current branch.

Perform each task like a new `# Repro`, which means when a task is done, you can wipe the `Copilot_Investigate.md` for the next task.

DO NOT ASK ME ANY QUESTION, I will not be watching you, you must make your best decision and run through the end.

## Task 1
```
