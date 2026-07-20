# Workflow

**Debuggable script language for interoperation with C++ using reflection, which can also generate C++ code.**

## License

This project is licensed under [the License repo](https://github.com/vczh-libraries/License).

Source code in this repo is for reference only, please use the source code in [the Release repo](https://github.com/vczh-libraries/Release).

You are welcome to contribute to this repo by opening pull requests.

## Document

[![Ask DeepWiki](https://deepwiki.com/badge.svg)](https://deepwiki.com/vczh-libraries/Workflow)

- For **Gaclib**: click [here](http://vczh-libraries.github.io/doc/current/home.html)
- For **Workflow**: click [here](http://vczh-libraries.github.io/doc/current/workflow/home.html)
  - Using **Workflow** in C++: click [here](http://vczh-libraries.github.io/doc/current/workflow/running.html)
  - Sample code for above document: click [here](https://github.com/vczh-libraries/Release/tree/master/Tutorial/Console_Workflow)
  - **Workflow** language reference: click [here](http://vczh-libraries.github.io/doc/current/workflow/lang.html)

## Features

### Executing Workflow by C++ code generation

Workflow scripts are able to be converted to equivalent C++ code.

If all scripts are known at compile time,
this could significantly improve the script performance.

You can also get rid of runtime reflection overhead in this way.

### Executing Workflow script directly

- Workflow and C++ classes call each other by **reflection**.
- **Serialize** compiled script to a file in binary format, you don't need to call the compiler every time.
- **bind(EXPR)** expression creates a subscriptable object to observe changing of the expression value.
- **Coroutine** allows implementing **Enumerable** and **Async** by coroutine functions.
- **Coroutine** allows creating your own coroutine syntax.
- **State Machine** offers a new way to write classes, taking methods as input and drive the state machine forward.
- Being able to install a **debugger** on a thread to track Workflow script.

## Unit Test

Checkout [Project.md](./Project.md) for details about compiling and running each test projects.

On Windows, use MSBuild or Visual Studio to build.

On Linux/macOS, run `REPO-ROOT/.github/Ubuntu/build.sh` in a project folder to update the makefile from vcxproj and build the project.
