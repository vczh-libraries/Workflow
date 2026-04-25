# Building a Solution

- Only run `copilotBuild.ps1` to build a solution.
- DO NOT use msbuild by yourself.
- The script builds all projects in a solution.

## Executing copilotBuild.ps1

Run this script to build the solution:

```
cd SOLUTION-ROOT
& REPO-ROOT\.github\Scripts\copilotBuild.ps1
```

It is possible that, before running `copilotBuild.ps1`, the binary to compile is still running or still being debugged. This could cause the linking to fail. You need to check the error message, and in case when it happens:
- Kill `cdb` process first, if there is any.
  - The cdb path is stored in `$env:CDBPATH`.
  - Avoid running `copilotDebug_Stop.ps1` directly.
- Kill the binary process that is blocked.
- Rebuild, and this issue should gone.

## Ensure Target Configuration

`-Configuration` and `-Platform` arguments are available to specify the target configuration:
- `-Configuration` could be `Debug` (default) or `Release`.
- `-Platform` could be `x64` (default) or `Win32`
- Pick the default option (omit both arguments) when there is no specific requirements.

## The Correct Way to Read Compiler Result

- The only source of trust is the raw output of the compiler.
- Wait for the script to finish before reading the log file.
  - DO NOT need to read the output from the script.
  - Building takes a long time. DO NOT hurry.
  - When the script finishes, the result is saved to `REPO-ROOT/.github/Scripts/Build.log`.
  - A temporary file `Build.log.unfinished` is created during building. It will be automatically deleted as soon as the building finishes. If you see this file, it means the building is not finished yet.
- When build succeeds, the last several lines of `Build.log` indicates the number of warnings and errors in the following pattern:
  - "Build succeeded."
  - "0 Warning(s)"
  - "0 Error(s)"
- DO NOT delete the log file by yourself.
