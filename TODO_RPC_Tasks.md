# Repro

You should complete tasks one by one.
`General Instruction` is for each tasks, which means, during doing each task:
- you have to run unit test to make sure your change works.
- you have to follow complete instructions in `.github\Rules\verify-and-commit.md` to properly finish any task, before doing the next task.

## Task 1

I would like to move RpcControllerMock.(h|cpp) to Source\Library with these changes:
- Now this pair of file needs to be in the library vcxitems instead of in each test projects' vcxproj files
- Rename it to WfLibraryRpcController.(h|cpp)
- Rename the class to vl::rpc_controller::RpcControllerDefault.
- You should adjust the guard macro naming and the header of the header file align with all other library header files.

Further more, you need to fix all CHECK_ERROR or CHECK_FAIL which have actual messages to align with the rest of the source code:
- At the beginning and the end of the function, define and undef ERROR_MESSAGE_PREFIX, to include the full class and function name.
- Use it inside CHECK_ERROR or CHECK_FAIL.
- You must check out other CHECK_ERROR samples in Source or Import to see how they are used.
- Ignore `CHECK_FAIL(L"Not Supported|);` or `CHECK_FAIL(L"Not Supported!");`

No reflection registration is needed, you can skip CompilerTest_GenerateMetadata and CompilerTest_LoadAndCompiler and Build.ps1, to shortern your test. This change should not affect the compiler and any code generation.
