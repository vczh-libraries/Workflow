# !!!INVESTIGATE!!!

# PROBLEM DESCRIPTION

DO NOT ASK ME ANY QUESTION, I will not be watching you, you must make your best decision and run through the end.
Follow `REPO-ROOT/.github/Rules/document-and-commit.md` to finish the work.

Currently both `RpcJsonObjectOps` and `RpcJsonObjectEventOps` receive a `IRpcSerializer*` argument, but in fact all callings to the serializer are all shorted cut. So there is no need to have them. Remove them and make sure everything (especially generated .ts files) are not changed.

In [.gitignore](Test/TypeScript/.gitignore) remove the last two lines so that all *.ts files are checked in.

In [prepare.ps1](Test/TypeScript/prepare.ps1) serialization files are copied to both `JsonValue(32|64)` and `JsonRequest(32|64)`, this is not good. I would like you to copy them to `DataSchema(32|64)`, and generated files in `Json(Value|Request)(32|64)` should reference that files. Therefore affected C++ code needs to change, all `CppTest*` test projects need to run.

# TEST [CONFIRMED]

Use the existing RPC JSON test coverage as the regression test. The work is successful when:

- `RpcJsonObjectOps` and `RpcJsonObjectEventOps` no longer store or receive `IRpcSerializer*`, and all call sites build without passing a serializer.
- `Test/TypeScript/prepare.ps1` copies `Serialization_*.d.ts` only into `DataSchema32` and `DataSchema64`.
- Generated `JsonValues32`, `JsonValues64`, `JsonRequest32`, and `JsonRequest64` files import schema declarations from the corresponding `DataSchema*` folder.
- JSON value and request payload contents remain stable except for the schema import path change.
- `CppTest`, `CppTest_Metaonly`, and `CppTest_Reflection` pass for Win32 and x64.
- `Test/TypeScript/prepare.ps1` followed by `npm run build` passes.

# PROPOSALS

- No.1 [CONFIRMED] Remove serializer storage from JSON transport adapters and centralize TypeScript schemas

## No.1 Remove serializer storage from JSON transport adapters and centralize TypeScript schemas

`RpcJsonObjectOps` and `RpcJsonObjectEventOps` already operate on boxed `JsonNode` values at the RPC JSON boundary. Their serializer field only supports a fallback conversion that current callers do not need, while their response translation path already rejects non-`JsonNode` values. Remove the constructor parameter and field, make argument conversion require boxed `JsonNode` values directly, update JSON dispatcher call sites, and change the TypeScript JSON dump writer to import schemas from `../DataSchema32` or `../DataSchema64`.

Change `prepare.ps1` so it refreshes `DataSchema32` and `DataSchema64`, and remove stale copied schema declarations from `JsonValues*` and `JsonRequest*`.

### CODE CHANGE

- Removed the `IRpcSerializer*` field and constructor parameter from `RpcJsonObjectOps` and `RpcJsonObjectEventOps`.
- Changed JSON transport argument conversion to require boxed `JsonNode` values directly.
- Updated the dual JSON dispatcher to construct JSON object ops without `GetSerializer()` and to emit `../DataSchema32` or `../DataSchema64` imports in generated TypeScript files.
- Changed `Test/TypeScript/prepare.ps1` to copy `Serialization_*.d.ts` into `DataSchema32` and `DataSchema64`, and to remove stale schema files from `JsonValues*` and `JsonRequest*`.
- Removed `JsonValues32/*.ts` and `JsonValues64/*.ts` from `Test/TypeScript/.gitignore` so generated TypeScript files are not ignored.

### CONFIRMED

- `copilotBuild.ps1 -Configuration Debug -Platform Win32` passed with 0 errors.
- `copilotBuild.ps1 -Configuration Debug -Platform x64` passed with 0 errors.
- `CppTest`, `CppTest_Metaonly`, and `CppTest_Reflection` passed on Win32, each with 2/2 files and 227/227 cases.
- `CppTest`, `CppTest_Metaonly`, and `CppTest_Reflection` passed on x64, each with 2/2 files and 227/227 cases.
- `Test/TypeScript/prepare.ps1` passed and placed 248 `Serialization_*.d.ts` files under `DataSchema32` and `DataSchema64`.
- `npm run build` in `Test/TypeScript` passed.
- Verified no generated `JsonValues*` or `JsonRequest*` file imports `./Serialization_*`, and no `Serialization_*.d.ts` files remain under those JSON output folders.
- Regeneration normalized one stale 32-bit JSON request trace (`JsonRequest32/JsonRequest_Oblist_EventException.ts`) in addition to the intended schema import path changes; this matches the current test output without adding artificial layout padding.
