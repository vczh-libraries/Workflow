# !!!INVESTIGATE!!!

# PROBLEM DESCRIPTION

You are going to create 4 test cases for destructor in new interface expression.

- Follow NewEmptyInterface to make a NewEmptyInterfaceDtor and see if the destructor working correctly
- Follow NewCustomIntarface*.txt to create 3 NewCustomInterfaceDtor*.txt. The point for the 3 original test cases are testing how a new interface methods could access captured functions and the implicit "this" pointer at the outer scope. Now you are going to do the same thing in destructor. It should contain:
  - visit captured variables and variables in self interface (NewCustomInterface.txt)
  - visit non-override methods in self interface (NewCustomInterface2.txt)
  - visit members in outer class scope (NewCustomInterface3.txt)
  - variables/non-override methods in outer new interface expression scope are not accessible because those "this" pointers captured from outer new interface expression scopes do not expose them (verify if my statement is correct)

All test cases should detect string(s) generated inside destructor(s), just like how NewInterfaceDtor.txt has done. After running CompilerTest_LoadAndCompile, if everything works well, new cpp files will be generated, and you must add those cpp files to correct vcxitems projects and rebuild the solution again, therefore CppTest* test projects work.

git commit and git push, DO NOT ASK ME ANY QUESTION

When everything went through, run ..\Tools\Tools\Build.ps1 VlppParser2, a full CI will run for this repo, and fix any issues. git commit and git push, DO NOT ASK ME ANY QUESTION

# UPDATES

# TEST

1. **NewEmptyInterfaceDtor.txt** - Simple interface with static method and destructor. Destructor logs `[dtor]` to global string. Expected: `[50][dtor]`.
2. **NewCustomInterfaceDtor.txt** - Destructor accesses captured outer variables (`x`, `y`) and self interface variable (`name`). Expected: `[hello:10:20][hello:10:20]`.
3. **NewCustomInterfaceDtor2.txt** - Destructor calls non-override method (`DoubleValue()`) in self interface. Expected: `[84][84]`.
4. **NewCustomInterfaceDtor3.txt** - Destructor accesses outer class static method (``GetBase()``) and member variable (``factor``). Expected: ``[10:5][10:5]``.
5. Verify that variables/non-override methods in outer new interface expression scope are not accessible from inner destructors (the user's statement should be correct).

Criteria:
- All 4 new test cases pass in `CompilerTest_LoadAndCompile` (Debug|x64 and Debug|Win32).
- Generated C++ files are added to `Generated_Cpp.vcxitems` and `Generated_Reflection.vcxitems`.
- All `CppTest`, `CppTest_Metaonly`, `CppTest_Reflection` pass (Debug|x64 and Debug|Win32).
- Full CI via `Build.ps1 VlppParser2` passes.

# PROPOSALS
