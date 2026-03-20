# General Instruction

## Solution to Work On

You are working on the solution `REPO-ROOT\Test\UnitTest\UnitTest.sln`,
therefore `SOLUTION-ROOT` is `REPO-ROOT\Test\UnitTest`.

## Projects for Verification

The `REPO-ROOT\Test\UnitTest\UnitTest\UnitTest.vcxproj` is the unit test project.
When any *.h or *.cpp file is changed, unit test is required to run.

### Code Generation Projects

Code generation projects could be either CLI or UnitTest projects.
They are required to run when a certain set of files are changed,
in order to generate code paring with them.
Here are a list of projects to run and files that should trigger them:

This repo does not have any code generation project.
