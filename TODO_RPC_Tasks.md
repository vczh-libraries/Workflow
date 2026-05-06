# Repro

You should complete tasks one by one.
`General Instruction` is for each tasks, which means, during doing each task:
- you have to run unit test to make sure your change works.
- If you can't fix a runtime issue within a few rounds of guess-and-edit, you need to debug the process.
- I am a fan of crash early. When something should happen, it should just happen, do not play a game like "what if it is not the case" and silently covers the issue. One example is that, if an object should not be null, then we should just use it, if a nullable object should not be null, we should just cast it. No test is performed in this case, using it will crash if it is null, and we know there is a problem. Fix the actual problem instead of doing "error tolerance".
- you have to follow complete instructions in `.github\Rules\verify-and-commit.md` to properly finish any task, before doing the next task.

It is important to do task one by one strictly, by me designing tasks in this way, we can achieve:
- Easy-to-understand commits for file changing that is easy to review.
- Limit side effects so that you don't have to deal with massive of issues at the same time.

## Workflow Script Best Practice

- Avoid explicit type specification whenever possible:
  - Prefer `var v = e;` whenever `T` can be omitted.
  - Prefer `var v : T = e;` over `var v = e over T;` if `T` cannot avoid.
  - When implicit type conversion works at the place, avoid `cast`, `as` and `infer` expression.
  - Prefer `cast *` over `cast T` when the context accepts `T`.

## Task 1

I would like you to implement `RpcDualJsonDispatcherMock`, inheriting from `RpcDualDispatcherMock`, override 4 methods returning ops interfaces.
All code should be in `Test/Source/RpcDualJsonDispatcherMock.(h|cpp)`
All 4 functions are implemented in the same way, so I only take one as an example, say we pick `BroadcastFromClient_ListEventOps`.
There is only two possible arguments passed to `BroadcastFromClient_ListEventOps`, basically we are going to implement it like this:

```C++
rpc_controller::IRpcListEventOps* RpcDualJsonDispatcherMock::BroadcastFromClient_ListEventOps(vint selfClientId)
{
  auto ops = RpcDualDispatcherMock::BroadcastFromClient_ListEventOps(selfClientId);
  auto&& opsPtr = ops == lifecycle1->GetController() ? &listEventOps1 : &listEventOps2;
  if (!opsPtr) opsPtr = Ptr(new RpcJsonListEventOpsMock(this, ops));
  return opsPtr.Obj();
}
```
`listEventOps[12]` is `Ptr<IRpcListEventOps>`, private fields in `RpcDualJsonDispatcherMock`.

What `RpcJsonListEventOpsMock` does will be simple, all arguments or return values, if it is `Value`, or `Value` in `arguments` array:
- `CHECK_ERROR` to ensure that it is a `Ptr<JsonNode>`.
- Add that object to `RpcDualJsonDispatcherMock`'s list.
- Redirect everything to the `ops` passed into its constructor.

Finally, use `RpcJsonListEventOpsMock` to replace `RpcDualDispatcherMock` only in `TestCasesRpc.h`, so that only `CppTest` test projects use it.
To distinguish `CppTest` and other `CppTest*`, you can protect the code with `VCZH_DEBUG_NO_REFLECTION`, when it is defined use the JSON version, otherwise use the original dual version.
The goal of this change is to make sure that, all arguments and return values passing through ops interfaces are actually JSON objects.
So if that `CHECK_ERROR` crashes, you are going to fix it.
To speed up testing, you only need to run `CppTest` test projects, others should be unrelated.

## Task 2

In `Test/Source/Helper.(h|cpp)` there should be a function `GetRpcMetadataOutputPath` for `Test/Generated/RpcMetadata(32|64)`:
- And `TestRpcCompile.cpp` should use it.
- And in `CompilerTest_LoadAndCompiler`'s `Main.cpp`, this folder should also be deleted just like `GetWorkflowOutputPath`.
  - Just like `GetCppOutputPathRpc`, 32/64 versions of `GetWorkflowOutputPath` and `GetRpcMetadataOutputPath` should be created in `Helper.(h|cpp)`.
  - Those 4 functions should be passed to `createOrCleanFolder` in `Main.cpp`, so that `Workflow(32|64)` and `RpcMetadata(32|64)` will be deleted.
  - In `Main.cpp` no need to handle `GetWorkflowOutputPath` separately, all 8 folders will be listed.
    - You can define a macro, list all 8 functions, under that macro, so the code looks clean.

At the end of the test case in `TestCasesRpc.h` for `CppTest` with , you are going to dump a big object to a `.ts` file.
- That means only when `VCZH_DEBUG_NO_REFLECTION` is defined.
- Each case already has a `Serialization_*.d.ts`
- Make a `JsonValue_*.ts` in `Test/TypeScript/JsonValues(32|64)`.

The content of `JsonValue_*.ts` looks like this:
```TypeScript
import { KnownTypeSchema } from `./Serialization_*` // pretent them to be in the same folder

const json : KnownTypeSchema[] = {
  JSON representation of a JsonNode recorded in `RpcJsonListEventOpsMock` in the first task.
}
```

To write JSON representation, you can use the predefined `JsonPrint` function, omit the formatter parameter and everything will be compact into single line.
To speed up testing, you only need to run `CppTest` test projects, others should be unrelated.

## Task 3

You are going to fix each `Serializtion_*.d.ts`, adding the missing `KnownTypeSchema` as described in `TODO_RPC_Json.md`, as well as other changes.
In commit `83af71e7bce0a0a94994a60a0116759ebd9ea18b`, `TODO_RPC_Json.md` is updated, read the commit to recognize what is changed, and you need to verify generated `Serialization_*.d.ts` to make sure it actually align with the latest `TODO_RPC_Json.md`.
This time you should run `CompilerTest_LoadAndCompile`. But if only `Serialization_*.d.ts` files are affected (that is the goal), to speed up testing, all following tests could be omitted.

## Task 4

Please be awared that, `Workflow` folder (this repo) and `VlppParser2` (`REPO-ROOT/../VlppParser2`) are siblings.

You are now going to complete `Test/TypeScript`. Just like `REPO-ROOT/../VlppParser2/Test/TypeScript`, you need to have:
- `.gitignore`, `package.json`, `package-lock.json` (this one should be generated by `npm run build`), `tsconfig.json`
- `prepare.ps1` to copy all `Test/Generated/RpcMetadata(32|64)/Serialization*.d.ts` to `Test/TypeScript/JsonValues(32|64)`
- You can just copy these 5 files to `Workflow` and then edit them.

You can read `REPO-ROOT/../VlppParser2/Test/TypeScript` for understanding, basically:
- `"typescript": "^6.0.2"`
- `tsconfig.json` needs to be twisted because typescript source files are in different folder than `VlppParser2` one.

Run `prepare.ps1` followed by `npm install` and `npm run build`, ensure it builds.

The goal is to test that, serialized JSON values actually match the generated .d.ts file.
If it can't build, besides of TypeScript or npm configuration issues, it is the Workflow code generation part that messed up.
You need to carefully re-read `TODO_RPC_Json.md` and figure out:
- Is the document has conflict information causing the issue?
- Is the implementation not matching the document causing the issue?

When the document has conflict information, I would like you to stick to the `## Expected format of generated .d.ts files`, and when it doesn't offer enough information, stick to all sections above. In this case, you should edit the document `TODO_RPC_Json.md`.
Since `TODO_RPC_Json.md` is updated, the compiling might expect to fail. In order to fix the issue, after modifying code generation, you should at least run:
- `CompilerTest_LoadAndCompile` debug x64
- `CppTest` debug x64 + Win32
to refresh all test files.

When issues are fixed, run `Build.ps1 Workflow` followed by compiling `Test/TypeScript` to make sure you have addressed every issues, including pre-existing ones.
