call "%VS140COMNTOOLS%VsDevCmd.bat"
msbuild ..\UnitTest\UnitTest.vcxproj /p:Configuration=Debug;Platform=x86
msbuild ..\UnitTest\UnitTest.vcxproj /p:Configuration=Debug;Platform=x64
msbuild ..\UnitTest_MergeCpp\UnitTest_MergeCpp.vcxproj /p:Configuration=Debug;Platform=x86
..\Debug\UnitTest.exe
..\x64\Debug\UnitTest.exe
..\Debug\UnitTest_MergeCpp.exe
msbuild ..\UnitTest_CppCodegen\UnitTest_CppCodegen.vcxproj /p:Configuration=Debug;Platform=x86
..\Debug\UnitTest_CppCodegen.exe
pause