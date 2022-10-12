# Workflow

**Debuggable script language for interoperation with C++ using reflection, which can also generate C++ code.**

## License

This project is licensed under [the License repo](https://github.com/vczh-libraries/License).

Source code in this repo is for reference only, please use the source code in [the Release repo](https://github.com/vczh-libraries/Release).

You are welcome to contribute to this repo by opening pull requests.

You are allowed to build `Tools/CppMerge` and run the executable in your local machine if it is not convenient for you to use the executable in **the Release repo** directly, but you should be awared that, the development of this repo could be ahead of **the Release repo**, so there could be compatible issues.

## Document

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

For **Windows**, open `Test/UnitTest/UnitTest.sln`, and run the following projects in order:
- **CompilerTest_GenerateMetadata** : Generate metadata file for involved C++ types (including classes only for unit test).
- **CompilerTest_LoadAndCompile** : Load the generated metadata, compile and run Workflow scripts, and generate C++ code from Workflow scripts.
- **CppTest** : Run generated C++ code.
- **CppTest_Metaonly** : Run generated C++ code.
- **CppTest_Reflection** : Run generated C++ code.
- **LibraryTest** : Test built-in library for Workflow.
- **RuntimeTest** : Test Workflow debugger.

For **Linux**, use `Test/Linux/*/makefile` to build and run unit test projects as described above.
