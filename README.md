# Workflow

**Debuggable script language for interoperation with C++ using reflection, which can also generate C++ code.**

## License

This project is licensed under [the License repo](https://github.com/vczh-libraries/License).

This project is not open sourced and for development only. But you are able to use the open sourced version in [the Release repo](https://github.com/vczh-libraries/Release).

The documentation for this library has not been done yet. Currently you can refer to the unit test to learn how to write this script and how to integrate this scripting system into your C++ projects:

## Document

Unfortunately I don't have time to document the code at this moment.
I promise I will do it before GacUI 1.0 get released.
Please learn by unit test at this moment.
Also note that there are comments before important classes.

- **Unit Test** solution is found [here](./Test/UnitTest/UnitTest.sln).
- **Script Files to Test** is found [here](../../tree/master/Test/Resources).

## Getting Started

Here is a simple tutorial to integrate this scripting system to your C++ project:

Call `vl::workflow::emitter::Compile` to compile multiple script files in memory to a `Ptr<vl::workflow::runtime::WfAssembly>`.

Call `WfAssembly::Serialize` to serialize the compiled assembly to a stream in binary format. If you provide a `vl::stream::FileStream` you can write the assembly to a file.

Call the constructor `WfAssembly::WfAssembly` to load a previous compiled assembly from a stream. If you provide a `vl::stream::FileStream` you can read the assembly from a file.

Create a `vl::workflow::runtime::WfRuntimeGlobalContext` from the assembly before running script functions. This context class act as the execution environment for the script to share the global variables. If you create multiple context instances, you will get multiple separated environments for global variables.

Use `vl::workflow::runtime::LoadFunction` to load a script function to a strong-typed functor or a weak-typed `Ptr<vl::reflection::description::IValueFunctionProxy>`. After that you can run functions in script.

Use `LoadFunction<void()>(globalContext, L"<initialize>")` to get the initialize function. This function should be called before using any other script function with the same global context.

The type T in `LoadFunction<T>` is decided by the declaration of the script function. For example, if you write the script function like this:

```workflow
func Main(input : int[]) : int
{
    return input.Count;
}
```

Then you can called `LoadFunction<int(vl::collections::List<int>&)>(globalContext, L"Main")` to load the function.

## Getting Started with Debuggable Scripts

- Use `vl::workflow::runtime::SetDebuggerForCurrentThread` to set a debugger. This debugger will be automatically activated if there is a script function running in this thread, and only for this thread. You should set different debuggers for different threads if you really want multi-threading execution (normally you don't need to do that).
  - You should create your own debugger class inheriting from **vl::workflow::runtime::WfDebugger** and override the following protected functions:
    - **OnBlockExecution**. This is a callback when the debugger is waiting for the next debug instructions. For example, after a break point is hit, the script will pause and call the **OnBlockExecution** function multiple times until you call `WfDebugger::Run` or other function to unpause the script again. So you should implement your pending logic by yourself.
      - If you run your debug logic in the differnt thread to the debugger (just like what the test case does), you can wait a lock in this function, and let the debug logic free the lock.
      - If you run your debug logic in the UI thread which also run the script, you can call the message loop in this function. Then you press the "Run" button (it's your button) to call `WfDebugger::Run`, `WfDebugger` will know and stop calling this callback, and the script continues.
    - **OnStartExecution**. This function will be called before any execution. Nested script and C++ function call will not call this callback multiple times.
    - **OnStopExecution**. This function will be called after any execution. Nested script and C++ function call will not call this callback multiple times.
  - If the script function calls a C++ function which calls another script function (and nested multiple levels like this), the **WfDebugger** will correctly handle the call stack, and let you see all nested script function calls.

## Generating C++ Code from Scripts

- Use `vl::workflow::cppcodegen::GenerateCppFiles` to generate C++ code from Workflow scripts. You have some options to choose some names and header comments.
- Use `vl::workflow::cppcodegen::MergeCppFileContent` to merge the latest generated code with a modified old version of generated code, if you use `@cpp:UseImpl` on some functions to indicate that user should insert their implementation in C++. You are able to import their modification for those functions **only** to your new generated code.

## Leaning by Unit Test

There are two important test that will help you build the concept:

- **Codegen** test. Test cases in this test test all features of this script language.
  - In [this folder](https://github.com/vczh-libraries/Workflow/tree/master/Test/Resources/Codegen) you can find all script files to test.
  - In [this file](https://github.com/vczh-libraries/Workflow/blob/master/Test/Resources/IndexCodegen.txt) you can find all expected result for each Main function in all script files to test.
  - In [this file](https://github.com/vczh-libraries/Workflow/blob/master/Test/Source/TestCodegen.cpp) you can see how the C++ unit test load all script files, print all internal information during compiling, and run all test functions.
  - Almost all test casses in **Codegen** generates C++ code while running the unit test to [this folder](https://github.com/vczh-libraries/Workflow/tree/master/Test/SourceCppGen), which can be executed in [the UnitTest_CppCodegen solution](https://github.com/vczh-libraries/Workflow/tree/master/Test/UnitTest/UnitTest_CppCodegen).
- **Debugger** test.
  - In [this folder](https://github.com/vczh-libraries/Workflow/tree/master/Test/Resources/Debugger) you can find all script files to test the debugger.
  - In [this file](https://github.com/vczh-libraries/Workflow/blob/master/Test/Source/TestDebugger.cpp) you can see how the C++ unit test load all script files, set every kind of break points and print variable values during debugging.

## Content

This scripting system provide the following features:

- **Interoperability with C++ classes** if you write your C++ classes using [vczh-libraries/VlppReflection](https://github.com/vczh-libraries/VlppReflection) following the [tutorial in comment](http://www.gaclib.net/CodeIndex.html#~/index/vl.reflection.Description`1/symbol/::vl::reflection::Description`1).
- **Serializing compiled script to binary** to increase the loading performance if the script is large.
- **Data Binding** infrastructure. The script language provide a special bind and observe syntax, to create a event object, which will fire if the value of the expression to observe is changed.
- **Debugger** infrastructure.
