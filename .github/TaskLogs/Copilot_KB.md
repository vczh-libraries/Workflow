# !!!KNOWLEDGE BASE!!!

# DESIGN REQUEST

you are going to figure out how to run the Workflow compiler. You can find samples in CompilerTest_LoadAndCompile, there are two tests to load normal samples and rpc samples.
- it would be helpful by comparing what extra things is done in rpc sample compiling then normal sample compiling.
- logging is not our interest as who calling the workflow compiler might do it in a different way
- focus on how to read errors, how to do compiling and generated rpc wrappers, how to log generated rpc wrappers Workflow code, how to let workflow do C++ code generation from input Workflow code and rpc Workflow wrappers
- focus on how to run workflow compiler for x86/x64, and merge both generate C++ generated code into a cross-architecture code. but any actual file path is not our interest as who calling the workflow compiler might do it differently.

# INSIGHT

## Compiler Driver Shape

The Workflow compiler has two usable layers.

- Use `vl::workflow::emitter::Compile` in `Source/Emitter/WfEmitter_Assembly.cpp` when the input is only a list of Workflow source strings and the caller only needs a `WfAssembly`. It clears a reusable `WfLexicalScopeManager`, adds every source string with `WfLexicalScopeManager::AddModule(const WString&)`, returns `nullptr` if parsing or analysis adds errors, and otherwise calls `GenerateAssembly`.
- Use the lower-level `WfLexicalScopeManager` flow when the caller needs intermediate analysis results, generated wrapper modules, or C++ code generation. `Test/UnitTest/CompilerTest_LoadAndCompile/TestCodegen.cpp` and `TestRpcCompile.cpp` both use this layer: parse or add modules, call `Rebuild`, inspect `manager.errors`, call `GenerateAssembly`, and optionally call `GenerateCppFiles`.
- `WfLexicalScopeManager` is constructed with a `workflow::Parser` and a `WfCpuArchitecture`. Its constructor in `Source/Analyzer/WfAnalyzer.cpp` installs `glr::InstallDefaultErrorMessageGenerator(workflowParser, errors)`, so parser errors and analyzer errors are collected in the same `manager.errors` list.
- `WfLexicalScopeManager::Rebuild` in `Source/Analyzer/WfAnalyzer.cpp` is the core analysis pipeline. It loads environment names, initializes global names from reflected type descriptors, desugars and validates module structure, builds global names and scopes, completes scopes, validates semantics, populates attributes on type descriptors, and finally runs RPC validation when `validateRpc` is true.
- `GenerateAssembly` in `Source/Emitter/WfEmitter_Assembly.cpp` assumes `Rebuild` has already succeeded. It builds metadata, an `<initialize>` function, function instructions for each module, and debug-location tables before calling `WfAssembly::Initialize`.
- `IWfCompilerCallback` in `Source/Analyzer/WfAnalyzer.h` is the supported progress hook for `Rebuild` and `GenerateAssembly`. It exposes `OnLoadEnvironment`, `OnInitialize`, `OnValidateModule`, `OnGenerateMetadata`, `OnGenerateCode`, and `OnGenerateDebugInfo`. The tests do not use it for logging, so compiler callers can supply their own reporting policy.

## Reading Errors

Compiler errors are read from `manager.errors`, or from the `errors` output list passed to `Compile`.

- Syntax parsing APIs such as `ParseModule` in `Source/Parser/WfExpression.cpp` tokenize, parse, unescape, and return an AST. Parser failures are reported through the parser's installed error handler. When using a `WfLexicalScopeManager`, the handler is installed into `manager.errors`; when parsing standalone, `TestSamples.cpp` shows installing `glr::InstallDefaultErrorMessageGenerator(parser, errors)` manually and removing the handler afterward.
- Analyzer and code-generation errors are `glr::ParsingError` values. `WfErrors::MakeParsingError` in `Source/Analyzer/WfAnalyzer_Errors.cpp` stores the AST node, `node->codeRange`, and a message such as `H*`, `I*`, or `CPP*`.
- The test log helper `LogSampleParseResult` in `Test/Source/Helper.cpp` shows the intended interpretation: display `error.codeRange.start.row + 1`, `error.codeRange.start.column + 1`, and `error.message`. Rows and columns are zero-based in the object and converted to one-based for users.
- `Compile` copies `manager.errors` to the caller's `errors` list and returns `nullptr` after either parsing or `Rebuild` errors. Lower-level flows should check `manager.errors.Count()` after `ParseModule` or `AddModule`, after `Rebuild`, after `GenerateModuleRpc`, after `GenerateModuleRpcJson`, and after `GenerateCppFiles`.
- RPC wrapper generation can add errors after normal analysis. `CollectMangledNames` and `BuildInterfaceModels` in `Source/Analyzer/WfAnalyzer_GenerateRpc.cpp` add `I*` errors for ambiguous mangled names or missing generated RPC attributes, and `GenerateCppFiles` can add `CPP*` errors through `WfCppConfig` when C++ file/class separation cannot be decided.

## Normal Sample Compilation

The normal generated-C++ sample path is `TEST_CATEGORY(L"Code generation")` in `TestCodegen.cpp`.

- `DecodeCodegenName` decodes `IndexCodegen.txt` entries into a sample name, expected result, and whether C++ should be generated. It also supports `@32` and `@64` sample suffixes, skipping a sample when the current `testCpuArchitecture` does not match.
- Each sample reuses one `WfLexicalScopeManager`, calls `manager.Clear(true, true)`, parses the sample with `ParseModule`, adds the parsed module with `manager.AddModule(module)`, and calls `manager.Rebuild(true)`.
- `LogSampleParseResult` is called after `Rebuild` so the log can include the original code, the `WfPrint` output, errors if any, and the JSON AST. The prompt is not asking for logging policy, but this proves the source of parse/analyzer diagnostics.
- After a clean rebuild, `GenerateAssembly(&manager)` creates the VM assembly. `LogSampleCodegenResult` and `LogSampleAssemblyBinary` in `Test/Source/Helper.cpp` demonstrate assembly inspection and binary serialization, but callers can choose different output sinks.
- Under `VCZH_MSVC`, normal samples also configure `WfCppInput(itemName)` with `multiFile = OnDemand`, `reflection = OnDemand`, a generated-file comment, and host includes, then call `GenerateCppFiles(input, &manager)`. The returned `WfCppOutput::cppFiles` dictionary is what the test writes to generated C++ files.
- At the end of the normal test category, `TestCodegen.cpp` writes a generated `TestCases.cpp` that includes each generated assembly entry header, calls `Load<AssemblyName>Types()` for assemblies containing reflection info, and invokes `::vl_workflow_global::<itemName>::Instance().main()` for each sample.

## RPC Sample Compilation

The RPC path is `TEST_CATEGORY(L"RPC compilation")` in `TestRpcCompile.cpp`. It does everything the normal path does, but it first compiles definitions enough to generate RPC metadata and wrapper Workflow modules, then links those wrappers with executable test logic.

- RPC index entries are decoded by `DecodeRpcName`. The sample is split into a definition module loaded as `Rpc/<itemName>.txt` and an executable test module loaded as `Rpc/<itemName>_Test.txt`.
- `CompileRpcSample` clears the manager, parses only the definition sample, adds it, and calls `manager.Rebuild(true)`. Because `validateRpc` defaults to true, this run performs normal semantic analysis and RPC validation.
- After `Rebuild`, `CompileRpcSample` requires `manager.rpcMetadata` and `manager.rpcMetadata->metadataModule`. It then calls `GenerateModuleRpc(&manager, MakeRpcCppAssemblyName(itemName))` to build the flat RPC wrapper module and `GenerateModuleRpcJson(&manager, MakeRpcCppAssemblyName(itemName))` to build the JSON RPC wrapper module. Both return `Ptr<WfModule>` and append errors to `manager.errors` if wrapper generation is invalid.
- `VerifyRpcMetadata` proves metadata stability. It prints `manager.rpcMetadata->metadataModule`, copies it with `CopyAndClearRpcMetadata`, clears the manager, rebuilds the copied metadata module, and compares the printed regenerated metadata. `CopyAndClearRpcMetadata` in `Source/Analyzer/WfAnalyzer_ValidateRPC.cpp` removes type declarations that already exist in reflection, so copied metadata can be recompiled without redefining existing reflected types.
- Generated wrapper Workflow code is logged by printing the returned modules. `TestRpcCompile.cpp` uses `GenerateToStream` with `WfPrint(wrapperModule, L"", writer)` and the same call for `wrapperJsonModule`; this is the key source-independent way to capture generated RPC wrapper Workflow text. It also prints `manager.rpcMetadata->metadataModule` and reads `manager.rpcMetadata->dts` for generated TypeScript schema text.
- `LinkRpcSample` is the final link step. It clears the manager, parses the definition and test modules, adds `definitionModule`, `testModule`, `wrapperModule`, and `wrapperJsonModule`, then calls `manager.Rebuild(true, nullptr, false)`. The `validateRpc = false` argument is important because the RPC metadata and wrapper modules have already been generated from the definition-only pass.
- After successful linking, `GenerateAssembly(&manager)` emits an assembly containing the original RPC definition, executable test code, flat wrapper Workflow, and JSON wrapper Workflow.
- Under `VCZH_MSVC`, `TestRpcCompile.cpp` configures `WfCppInput` with an RPC-prefixed assembly name and RPC-prefixed generated filenames before calling `GenerateCppFiles`. This makes the generated C++ include the input Workflow and both generated wrapper modules.
- The generated RPC `TestCasesRpc.cpp` differs from normal `TestCases.cpp`: it includes RPC runtime test helpers, calls `Load<AssemblyName>Types()` for reflectable generated assemblies, and invokes `RunRpcTestCase<::vl_workflow_global::<assemblyName>, hasEvents>(itemName, itemResult)`. `CollectRpcEventBridgeInfos` gathers event metadata from `manager.rpcMetadata->eventFullNames` and reflected event handler types before the final manager is cleared.

## RPC Metadata and Wrapper Internals

The RPC compiler logic lives on top of ordinary Workflow analysis.

- `WfRpcMetadata` in `Source/Analyzer/WfAnalyzer.h` stores `metadataModule`, generated `.d.ts` text, maps from full names to generated declarations, ordered type/method/event full-name lists, and `orderedIds`.
- `ValidateModuleRPC` in `Source/Analyzer/WfAnalyzer_ValidateRPC.cpp` has three phases: `ValidateModuleRPC_Ast` collects Workflow-defined RPC interfaces and attribute occurrences, `ValidateModuleRPC_Reflection` validates the remaining rules against reflected `ITypeDescriptor` and `ITypeInfo`, and `ValidateModuleRPC_GenerateMetadata` creates the metadata module when there are RPC interfaces and no errors. It then fills `manager.rpcMetadata->dts` with `GenerateDtsFromRpcMetadata`.
- `ValidateModuleRPC_GenerateMetadata` collects all RPC interface type descriptors, recursively collects serializable enum and struct types used by RPC interfaces, preserves struct dependency visit order, collects RPC interfaces in AST order with `CollectOrderedRpcInterfaces`, and then builds a `module RpcMetadata` containing generated enum, struct, and interface declarations.
- RPC id assignment is deterministic. `ValidateModuleRPC_GenerateMetadata` records type, method, and event full names into `manager.rpcMetadata`, calls `AddOrderedRpcId`, and later `GenerateModuleRpc` emits both integer constants and `rpc_GetIds()` from `orderedIds`.
- `GenerateModuleRpc` in `Source/Analyzer/WfAnalyzer_GenerateRpc.cpp` produces flat Workflow infrastructure: id constants, `rpc_GetIds`, `rpcwrapper_IsInterfaceTypeId`, `rpcwrapper_IsCtorInterfaceTypeId`, the generated ops interface, object ops factories, object-event ops factories, caller-side ops factory, wrapper interfaces, event listener factories and dispatcher, wrapper factories, wrapper dispatcher, and type-id helper.
- `GenerateModuleRpcJson` in `Source/Analyzer/WfAnalyzer_GenerateRpc_JsonSerialization.cpp` produces JSON Workflow infrastructure: JSON serialization declarations, generated serializer factory, JSON object ops factory, JSON object-event ops factory, and JSON caller-side ops factory. It reuses `BuildInterfaceModels`, so flat and JSON wrappers are generated from the same RPC metadata model.
- Inherited RPC interface members are collected recursively. `CollectInterfaceProperties`, `CollectInterfaceMethods`, and `CollectInterfaceEvents` recurse through `RpcInterfaceModel::baseFullNames` before adding the current interface members, and generated wrapper factories use those collected lists.
- Where type checks need derived interfaces before base interfaces, `SortInterfaceModelsLeafFirst` builds a dependency group from base interface names to derived interface names and runs `PartialOrderingProcessor`. `GenerateWrapperGetTypeId` uses this order so derived objects are not classified as a base type too early.

## C++ Code Generation from Input and Wrappers

Workflow C++ generation is driven by `WfCppInput` and `GenerateCppFiles`.

- `WfCppInput` in `Source/Cpp/WfCpp.h` controls file splitting, reflection file generation, comments, header guards, assembly name/namespace, generated filename stems, and normal/reflection includes. Its constructor in `Source/Cpp/WfCpp_GenerateCppFiles.cpp` sets defaults from the assembly name.
- `GenerateCppFiles` constructs `WfCppConfig(manager, input->assemblyName, input->assemblyNamespace)`. If `manager.errors` is already non-empty, it returns `nullptr`.
- `GenerateCppFiles` decides `multiFile` and `reflection` from `WfCppFileSwitch`. In `OnDemand` mode, multi-file generation is enabled only when generated/custom file grouping requires it, and reflection files are emitted only when `manager->declarationTypes.Count() > 0`.
- `WfCppOutput` returns `cppFiles`, `containsReflectionInfo`, `multiFile`, `reflection`, and `entryFileName`. The tests use `containsReflectionInfo` to decide whether to emit `Load<AssemblyName>Types()` and use either `reflectionFileName` or `entryFileName` as the header to include in generated test cases.
- Wrapper modules do not need a special C++ generator entry point. The caller adds generated `WfModule` objects to the same manager as normal modules, rebuilds successfully, and calls `GenerateCppFiles` once. `TestRpcCompile.cpp` proves this by adding `definitionModule`, `testModule`, `wrapperModule`, and `wrapperJsonModule` before the final `GenerateAssembly` and `GenerateCppFiles`.

## x86, x64, and Cross-Architecture Merge

The test executable runs the compiler twice, once for x86 and once for x64, then merges generated C++.

- `Main.cpp` in `CompilerTest_LoadAndCompile` sets `testCpuArchitecture = WfCpuArchitecture::x86`, loads x86 meta-only reflection data, runs all tests, unloads types, then repeats with `WfCpuArchitecture::x64` and x64 reflection data.
- `WfLexicalScopeManager` maps Workflow CPU-sized integer types from the selected architecture. Its constructor sets `cputdSInt`, `cputdUInt`, `cputiSInt`, and `cputiUInt` to `vint32_t`/`vuint32_t` for x86, `vint64_t`/`vuint64_t` for x64, and `vint`/`vuint` for `AsExecutable`.
- The path choice in `Helper.cpp` is test infrastructure, but the concept is important: each architecture writes generated Workflow assemblies, RPC metadata, normal C++ files, and RPC C++ files to separate architecture-specific outputs.
- Merge starts by enumerating generated filenames for x86 and x64 with `FillFileNames` and requiring the filename sets to be identical. `Main.cpp` performs this separately for normal generated C++, RPC generated C++, and generated app C++.
- For each matching file, `Main.cpp` reads the 32-bit and 64-bit generated text and calls `MergeCppMultiPlatform` from `Source/Cpp/WfMergeCpp.cpp`. If the target file already exists, it then calls `MergeCppFileContent(existing, merged)` to preserve user implementation blocks before writing the final cross-architecture file.
- `MergeCppMultiPlatform` writes common text and only tolerates architecture-sized differences: `vint32_t` versus `vint64_t` becomes `vint`, `vuint32_t` versus `vuint64_t` becomes `vuint`, 64-bit integer suffix differences are removed, and selected generated `static_cast<::vl::vint32_t>` or `static_cast<::vl::vint64_t>` numeric forms become `static_cast<::vl::vint>`. Any other mismatch throws `MergeCppMultiPlatformException` with row and column positions for both inputs.
- `MergeCppFileContent` preserves custom implementation regions from an existing target. It recognizes generated `USER_CONTENT_BEGIN` / `USER_CONTENT_END` regions and `USERIMPL` shaped function bodies, restores matching old content into the newly merged generated file, and appends unmatched old user content under `UNUSED_USER_CONTENT`.
- For an external compiler driver, the reusable architecture recipe is: compile the same logical input twice with matching reflected type metadata and `WfCpuArchitecture::x86`/`x64`, require the same generated filename set, merge each pair with `MergeCppMultiPlatform`, then preserve existing custom regions with `MergeCppFileContent`.

# ASKS

## QUESTION

````text
the process is complex, you can write sample code in ```C++ to make the document clear, multiple pieces of sample codes are also OK if necessary
````

### ANSWER

Yes. The final KB draft should include C++ sample code because the compiler flow is easier to understand as a sequence of API calls than as prose only. The snippets should be marked as skeletons: they should show control flow, error checkpoints, and where generated modules are inserted, but should leave file-system layout and logging policy to the caller.

Use several smaller snippets instead of one large sample. A good split is:

- one helper for reading `glr::ParsingError`;
- one normal Workflow compile path;
- one RPC wrapper preparation path;
- one final link/codegen path for input modules plus generated RPC wrapper modules;
- one x86/x64 merge path.

Recommended snippet for reading errors:

```C++
void WriteWorkflowErrors(stream::TextWriter& writer, const collections::List<glr::ParsingError>& errors)
{
	for (auto&& error : errors)
	{
		writer.WriteLine(
			L"Line: " + itow(error.codeRange.start.row + 1) +
			L", Column: " + itow(error.codeRange.start.column + 1) +
			L", Message: " + error.message);
	}
}
```

Recommended snippet for compiling ordinary Workflow modules when no RPC wrappers are needed:

```C++
Ptr<runtime::WfAssembly> CompileWorkflowModules(
	workflow::Parser& parser,
	analyzer::WfCpuArchitecture architecture,
	const collections::List<WString>& moduleCodes,
	collections::List<glr::ParsingError>& errors)
{
	analyzer::WfLexicalScopeManager manager(parser, architecture);
	for (auto&& moduleCode : moduleCodes)
	{
		manager.AddModule(moduleCode);
	}
	if (manager.errors.Count() > 0)
	{
		CopyFrom(errors, manager.errors);
		return nullptr;
	}

	manager.Rebuild(true);
	if (manager.errors.Count() > 0)
	{
		CopyFrom(errors, manager.errors);
		return nullptr;
	}

	return emitter::GenerateAssembly(&manager);
}
```

Recommended snippet for generating C++ from an already analyzed manager:

```C++
Ptr<cppcodegen::WfCppOutput> GenerateWorkflowCpp(
	analyzer::WfLexicalScopeManager& manager,
	const WString& assemblyName,
	const collections::List<WString>& normalIncludes,
	collections::List<glr::ParsingError>& errors)
{
	auto input = Ptr(new cppcodegen::WfCppInput(assemblyName));
	input->multiFile = cppcodegen::WfCppFileSwitch::OnDemand;
	input->reflection = cppcodegen::WfCppFileSwitch::OnDemand;
	CopyFrom(input->normalIncludes, normalIncludes);

	auto output = cppcodegen::GenerateCppFiles(input, &manager);
	if (manager.errors.Count() > 0)
	{
		CopyFrom(errors, manager.errors);
		return nullptr;
	}
	return output;
}
```

Recommended snippet for the RPC definition pass. This is the extra work that ordinary samples do not need:

```C++
struct RpcGeneratedWorkflow
{
	Ptr<WfModule>	definitionModule;
	Ptr<WfModule>	wrapperModule;
	Ptr<WfModule>	wrapperJsonModule;
	WString			metadataWorkflow;
	WString			wrapperWorkflow;
	WString			wrapperJsonWorkflow;
	WString			dts;
};

bool GenerateRpcWorkflowModules(
	workflow::Parser& parser,
	analyzer::WfCpuArchitecture architecture,
	const WString& definitionCode,
	const WString& cppAssemblyName,
	RpcGeneratedWorkflow& generated,
	collections::List<glr::ParsingError>& errors)
{
	analyzer::WfLexicalScopeManager manager(parser, architecture);

	generated.definitionModule = ParseModule(definitionCode, parser);
	if (!generated.definitionModule || manager.errors.Count() > 0)
	{
		CopyFrom(errors, manager.errors);
		return false;
	}

	manager.AddModule(generated.definitionModule);
	manager.Rebuild(true);
	if (manager.errors.Count() > 0 || !manager.rpcMetadata || !manager.rpcMetadata->metadataModule)
	{
		CopyFrom(errors, manager.errors);
		return false;
	}

	generated.wrapperModule = analyzer::GenerateModuleRpc(&manager, cppAssemblyName);
	generated.wrapperJsonModule = analyzer::GenerateModuleRpcJson(&manager, cppAssemblyName);
	if (manager.errors.Count() > 0 || !generated.wrapperModule || !generated.wrapperJsonModule)
	{
		CopyFrom(errors, manager.errors);
		return false;
	}

	generated.metadataWorkflow = GenerateToStream([&](stream::StreamWriter& writer)
	{
		WfPrint(manager.rpcMetadata->metadataModule, L"", writer);
	});
	generated.wrapperWorkflow = GenerateToStream([&](stream::StreamWriter& writer)
	{
		WfPrint(generated.wrapperModule, L"", writer);
	});
	generated.wrapperJsonWorkflow = GenerateToStream([&](stream::StreamWriter& writer)
	{
		WfPrint(generated.wrapperJsonModule, L"", writer);
	});
	generated.dts = manager.rpcMetadata->dts;
	return true;
}
```

Recommended snippet for the RPC final link pass. The important point is that `validateRpc` is false here, because the wrappers were already generated from the definition-only pass:

```C++
Ptr<runtime::WfAssembly> LinkRpcWorkflowAssembly(
	workflow::Parser& parser,
	analyzer::WfCpuArchitecture architecture,
	Ptr<WfModule> definitionModule,
	const WString& testCode,
	Ptr<WfModule> wrapperModule,
	Ptr<WfModule> wrapperJsonModule,
	collections::List<glr::ParsingError>& errors)
{
	analyzer::WfLexicalScopeManager manager(parser, architecture);
	auto testModule = ParseModule(testCode, parser);
	if (!testModule || manager.errors.Count() > 0)
	{
		CopyFrom(errors, manager.errors);
		return nullptr;
	}

	manager.AddModule(definitionModule);
	manager.AddModule(testModule);
	manager.AddModule(wrapperModule);
	manager.AddModule(wrapperJsonModule);
	manager.Rebuild(true, nullptr, false);
	if (manager.errors.Count() > 0)
	{
		CopyFrom(errors, manager.errors);
		return nullptr;
	}

	return emitter::GenerateAssembly(&manager);
}
```

Recommended snippet for merging generated x86 and x64 C++ text. The caller decides how to enumerate, read, and write files:

```C++
WString MergeGeneratedCppFile(
	const WString& code32,
	const WString& code64,
	const Nullable<WString>& existingCode)
{
	auto mergedCode = cppcodegen::MergeCppMultiPlatform(code32, code64);
	if (existingCode)
	{
		mergedCode = cppcodegen::MergeCppFileContent(existingCode.Value(), mergedCode);
	}
	return mergedCode;
}
```

In the draft, place these samples after the conceptual explanation, not before it. The prose should first name the real APIs (`WfLexicalScopeManager::Rebuild`, `GenerateModuleRpc`, `GenerateModuleRpcJson`, `GenerateAssembly`, `GenerateCppFiles`, `MergeCppMultiPlatform`, `MergeCppFileContent`), then the snippets should show how those APIs are normally composed.

# DRAFT

## DRAFT REQUEST

api explanation. by the way I would not use the term `recommended snippet` as the user will have their own choices. try to phrase code samples as a reference to helping them understanding the API.

## IMPROVEMENTS

### IMPROVEMENT

multiple C++ source files will be generated include translation of Workflow, reflection supports and others. You are going to fill this part

### IMPROVEMENT

"use the rpc flow when any input interface uses `@rpc:Interface`" is not detailed enough. Workflow compiler actually tell you if rpc is involved by having rpcMetadata to non-null. Read the source code and figure out if it is true, clue can also be figured out in the test case

## API EXPLANATION (Workflow)

Add a new section under `### Choosing APIs` in `.github/KnowledgeBase/Index_Workflow.md`, after `#### JSON Request Routing` and before `### Design Explanation`.

The section title should be:

`#### Compiler Driver and C++ Generation`

The section should explain that Workflow compiler callers can compile source modules into `WfAssembly`, generate RPC wrapper Workflow modules from `@rpc:Interface` definitions, generate C++ from the final analyzed modules, and merge x86/x64 generated C++ into architecture-neutral output.

The section should use this link:

`[API Explanation](./KB_Workflow_CompilerDriverAndCppGeneration.md)`

## DOCUMENT

# Workflow Compiler Driver and C++ Generation

Workflow source code can be compiled into a runtime `WfAssembly`, converted into generated C++ files, or extended with generated RPC wrapper Workflow modules before either of those outputs is produced. The ordinary compiler path and the RPC compiler path share the same `WfLexicalScopeManager` analyzer, but the RPC path has an additional definition-only pass that creates wrapper modules and metadata before the final link.

This document focuses on using the compiler APIs. Logging, file layout, and build integration are intentionally left to the caller.

## Main APIs

- Use `vl::workflow::emitter::Compile` when all inputs are ordinary Workflow source strings and the caller only needs a `vl::workflow::runtime::WfAssembly`.
- Use `vl::workflow::analyzer::WfLexicalScopeManager` directly when the caller needs intermediate compiler state, generated RPC wrapper modules, generated C++ output, or custom progress reporting through `IWfCompilerCallback`.
- Use `vl::workflow::emitter::GenerateAssembly` after `WfLexicalScopeManager::Rebuild` succeeds.
- Use `vl::workflow::analyzer::GenerateModuleRpc` and `vl::workflow::analyzer::GenerateModuleRpcJson` after RPC validation succeeds and `manager.rpcMetadata->metadataModule` exists.
- Use `vl::workflow::WfPrint` to turn generated `WfModule` values, such as RPC metadata and wrapper modules, back into Workflow text.
- Use `vl::workflow::cppcodegen::GenerateCppFiles` after the final set of modules has been added to a successfully rebuilt `WfLexicalScopeManager`.
- Use `vl::workflow::cppcodegen::MergeCppMultiPlatform` and `vl::workflow::cppcodegen::MergeCppFileContent` when x86 and x64 generated C++ need to become one architecture-neutral output while preserving user implementation regions.

## Compiler State and Errors

`WfLexicalScopeManager` owns the parser error handler, the parsed modules, all analyzer state, optional RPC metadata, and the `manager.errors` list. Construct it with a `workflow::Parser` and a `WfCpuArchitecture`.

`WfCpuArchitecture` controls Workflow's CPU-sized integer mapping:

- `WfCpuArchitecture::x86` maps CPU-sized signed and unsigned integer types to `vint32_t` and `vuint32_t`.
- `WfCpuArchitecture::x64` maps them to `vint64_t` and `vuint64_t`.
- `WfCpuArchitecture::AsExecutable` maps them to `vint` and `vuint`.

Compiler errors are `glr::ParsingError` values. `error.codeRange.start.row` and `error.codeRange.start.column` are zero-based positions. A user-facing diagnostic normally adds 1 to both values and prints `error.message`.

The following code is a reference example for reading compiler errors. It shows the fields that compiler callers usually need; callers can send the text to any log, UI, exception, or diagnostic system.

```C++
void WriteWorkflowErrors(stream::TextWriter& writer, const collections::List<glr::ParsingError>& errors)
{
	for (auto&& error : errors)
	{
		writer.WriteLine(
			L"Line: " + itow(error.codeRange.start.row + 1) +
			L", Column: " + itow(error.codeRange.start.column + 1) +
			L", Message: " + error.message);
	}
}
```

Check `manager.errors` after every phase that can add diagnostics:

- after `AddModule(const WString&)` or explicit parsing with `ParseModule`;
- after `WfLexicalScopeManager::Rebuild`;
- after `GenerateModuleRpc`;
- after `GenerateModuleRpcJson`;
- after `GenerateCppFiles`.

The convenience `Compile` API copies `manager.errors` to the caller-provided error list and returns `nullptr` when parsing or analysis fails.

## Ordinary Workflow Compilation

For ordinary Workflow source modules, the shortest path is `Compile`. Use the explicit manager path when the caller also needs C++ generation from the same analyzed modules.

The following reference example shows the explicit manager flow for compiling ordinary modules into a `WfAssembly`.

```C++
Ptr<runtime::WfAssembly> CompileWorkflowModules(
	workflow::Parser& parser,
	analyzer::WfCpuArchitecture architecture,
	const collections::List<WString>& moduleCodes,
	collections::List<glr::ParsingError>& errors)
{
	analyzer::WfLexicalScopeManager manager(parser, architecture);
	for (auto&& moduleCode : moduleCodes)
	{
		manager.AddModule(moduleCode);
	}

	if (manager.errors.Count() > 0)
	{
		CopyFrom(errors, manager.errors);
		return nullptr;
	}

	manager.Rebuild(true);
	if (manager.errors.Count() > 0)
	{
		CopyFrom(errors, manager.errors);
		return nullptr;
	}

	return emitter::GenerateAssembly(&manager);
}
```

`WfLexicalScopeManager::Rebuild(true)` keeps reflected type descriptor names in the manager while clearing compiler state for the current module set. During rebuild, the compiler desugars modules, validates structure, builds scopes, validates semantics, populates attributes, and validates RPC declarations by default.

## C++ Generation from Workflow Modules

`GenerateCppFiles` consumes an already analyzed `WfLexicalScopeManager`. It does not parse or rebuild modules by itself. The input object `WfCppInput` controls assembly naming, generated filename stems, file splitting, reflection-file generation, comments, header guard prefix, and includes.

The result is not a single source file. `WfCppOutput::cppFiles` is the complete generated file set, and the caller decides how to store, log, or pass each entry to the C++ build. `WfCppOutput::entryFileName` names the header stem that downstream code should include, but it is not the only generated file.

Generated file groups include:

- the main Workflow translation files: `<defaultFileName>.h` and `<defaultFileName>.cpp`, generated for every successful C++ generation call;
- internal split headers: `<defaultFileName>N.h`, generated when the compiler separates generated classes into additional non-custom headers;
- custom split files: `<CustomFile>.h` and `<CustomFile>.cpp`, generated when Workflow declarations are assigned to explicit C++ file groups;
- the aggregate include header: `<includeFileName>.h`, generated when multi-file output is enabled, and used as `entryFileName` so consumers can include one generated header;
- reflection support files: `<reflectionFileName>.h` and `<reflectionFileName>.cpp`, generated when reflection output is enabled; with `WfCppFileSwitch::OnDemand`, this happens only when the analyzed modules define Workflow types needing reflection registration;
- conditional reflection includes in generated `.cpp` files, so reflection support and caller-provided `reflectionIncludes` can be excluded when `VCZH_DEBUG_NO_REFLECTION` is defined.

`input->normalIncludes` are added to the main generated header. `input->reflectionIncludes` are added to the generated reflection header when reflection files are emitted, or as conditional includes in generated `.cpp` files when the caller supplies reflection dependencies without generating the reflection source pair.

`WfCppOutput::containsReflectionInfo` reports whether the analyzed Workflow modules contain reflection metadata. `WfCppOutput::reflection` reports whether reflection files were actually emitted. These can differ when `input->reflection` is disabled: Workflow types may exist, but no reflection source pair is generated.

The following reference example shows the shape of a C++ generation call after the manager has already been rebuilt successfully.

```C++
Ptr<cppcodegen::WfCppOutput> GenerateWorkflowCpp(
	analyzer::WfLexicalScopeManager& manager,
	const WString& assemblyName,
	const collections::List<WString>& normalIncludes,
	collections::List<glr::ParsingError>& errors)
{
	auto input = Ptr(new cppcodegen::WfCppInput(assemblyName));
	input->multiFile = cppcodegen::WfCppFileSwitch::OnDemand;
	input->reflection = cppcodegen::WfCppFileSwitch::OnDemand;
	for (auto&& include : normalIncludes)
	{
		input->normalIncludes.Add(include);
	}

	auto output = cppcodegen::GenerateCppFiles(input, &manager);
	if (manager.errors.Count() > 0)
	{
		CopyFrom(errors, manager.errors);
		return nullptr;
	}
	return output;
}
```

`WfCppFileSwitch::OnDemand` lets the generator decide whether to emit multi-file output or reflection files:

- multi-file output is enabled when generated/custom file grouping requires it;
- reflection files are emitted when the analyzed modules define Workflow types that need reflection registration.

The caller should consume every `cppFiles` entry, not only the entry header. The following reference example shows the shape of that handoff without assuming where files are written or how generated text is logged.

```C++
void VisitGeneratedCppFiles(cppcodegen::WfCppOutput& output)
{
	for (auto [fileName, index] : indexed(output.cppFiles.Keys()))
	{
		const auto& code = output.cppFiles.Values()[index];
		ConsumeGeneratedCppFile(fileName, code);
	}
}
```

## RPC Wrapper Generation

RPC compilation has two stages:

1. Compile the RPC definition module alone with RPC validation enabled, then generate metadata and wrapper Workflow modules.
2. Add the original definition module, the executable/user modules, and the generated wrapper modules into a fresh final manager, rebuild with RPC validation disabled, and then generate an assembly or C++.

The definition-only stage is necessary because `GenerateModuleRpc` and `GenerateModuleRpcJson` need `manager.rpcMetadata`, which is produced by `ValidateModuleRPC` during `WfLexicalScopeManager::Rebuild`.

The compiler signal for "RPC is involved" is `manager.rpcMetadata`, not a caller-side scan for `@rpc:Interface` text. `WfLexicalScopeManager::Rebuild` first clears `rpcMetadata` to `nullptr`, then runs `ValidateModuleRPC` for each module when `validateRpc` is true. `ValidateModuleRPC` creates `manager.rpcMetadata` only when phase 1 collected at least one valid Workflow RPC interface and no errors have been reported. Metadata generation also fills `manager.rpcMetadata->metadataModule`, and both `GenerateModuleRpc` and `GenerateModuleRpcJson` return `nullptr` if either `rpcMetadata` or `metadataModule` is missing.

After a successful definition-only rebuild with RPC validation enabled, interpret the state as:

- `manager.errors` is non-empty: report diagnostics first; do not decide the compile flow from missing metadata.
- `manager.rpcMetadata && manager.rpcMetadata->metadataModule`: RPC interfaces were found and metadata was generated, so wrapper generation can run.
- `manager.rpcMetadata == nullptr`: no valid Workflow RPC interface was found during this rebuild, so there are no RPC wrapper modules to generate.

This signal belongs to the RPC-validating pass. The final RPC link intentionally calls `Rebuild(..., validateRpc = false)`, so it should not be expected to populate `rpcMetadata`.

`manager.rpcMetadata` contains:

- `metadataModule`, a generated `WfModule` containing serializable enum, struct, and RPC interface declarations;
- `dts`, generated TypeScript schema text for RPC JSON serialization;
- `typeNames`, `methodNames`, and `eventNames`, mapping RPC full names to generated declarations;
- `typeFullNames`, `methodFullNames`, `eventFullNames`, and deterministic `orderedIds`.

The following reference example shows the definition-only stage. It captures generated metadata and wrapper Workflow text with `WfPrint`, but the caller decides what to do with the strings.

```C++
struct RpcGeneratedWorkflow
{
	Ptr<WfModule>	wrapperModule;
	Ptr<WfModule>	wrapperJsonModule;
	WString			metadataWorkflow;
	WString			wrapperWorkflow;
	WString			wrapperJsonWorkflow;
	WString			dts;
};

bool GenerateRpcWorkflowModules(
	workflow::Parser& parser,
	analyzer::WfCpuArchitecture architecture,
	const WString& definitionCode,
	const WString& cppAssemblyName,
	RpcGeneratedWorkflow& generated,
	collections::List<glr::ParsingError>& errors)
{
	analyzer::WfLexicalScopeManager manager(parser, architecture);

	auto definitionModule = ParseModule(definitionCode, parser);
	if (!definitionModule || manager.errors.Count() > 0)
	{
		CopyFrom(errors, manager.errors);
		return false;
	}

	manager.AddModule(definitionModule);
	manager.Rebuild(true);
	if (manager.errors.Count() > 0 || !manager.rpcMetadata || !manager.rpcMetadata->metadataModule)
	{
		CopyFrom(errors, manager.errors);
		return false;
	}

	generated.wrapperModule = analyzer::GenerateModuleRpc(&manager, cppAssemblyName);
	generated.wrapperJsonModule = analyzer::GenerateModuleRpcJson(&manager, cppAssemblyName);
	if (manager.errors.Count() > 0 || !generated.wrapperModule || !generated.wrapperJsonModule)
	{
		CopyFrom(errors, manager.errors);
		return false;
	}

	generated.metadataWorkflow = GenerateToStream([&](stream::StreamWriter& writer)
	{
		WfPrint(manager.rpcMetadata->metadataModule, L"", writer);
	});
	generated.wrapperWorkflow = GenerateToStream([&](stream::StreamWriter& writer)
	{
		WfPrint(generated.wrapperModule, L"", writer);
	});
	generated.wrapperJsonWorkflow = GenerateToStream([&](stream::StreamWriter& writer)
	{
		WfPrint(generated.wrapperJsonModule, L"", writer);
	});
	generated.dts = manager.rpcMetadata->dts;
	return true;
}
```

`GenerateModuleRpc` creates flat RPC Workflow infrastructure, including id constants, `rpc_GetIds`, type-id helpers, object ops, object-event ops, caller-side ops, event listeners, wrapper interfaces, wrapper factories, and wrapper dispatch.

`GenerateModuleRpcJson` creates JSON RPC Workflow infrastructure, including JSON serializers, JSON object ops, JSON object-event ops, and JSON caller-side ops. Flat and JSON generation use the same RPC metadata model.

## Final RPC Link

After wrapper modules are generated, the final compilation should parse the definition code and user/executable code again, then add both generated wrapper modules to the final manager.

The final rebuild should pass `validateRpc = false`. The input definition has already been validated, and the generated wrapper modules are compiler output. Revalidating RPC metadata during the final link is not the intended flow.

The following reference example produces a final `WfAssembly` from a definition module, an executable module, and generated wrapper modules.

```C++
Ptr<runtime::WfAssembly> LinkRpcWorkflowAssembly(
	workflow::Parser& parser,
	analyzer::WfCpuArchitecture architecture,
	const WString& definitionCode,
	const WString& executableCode,
	Ptr<WfModule> wrapperModule,
	Ptr<WfModule> wrapperJsonModule,
	collections::List<glr::ParsingError>& errors)
{
	analyzer::WfLexicalScopeManager manager(parser, architecture);

	auto definitionModule = ParseModule(definitionCode, parser);
	auto executableModule = ParseModule(executableCode, parser);
	if (!definitionModule || !executableModule || manager.errors.Count() > 0)
	{
		CopyFrom(errors, manager.errors);
		return nullptr;
	}

	manager.AddModule(definitionModule);
	manager.AddModule(executableModule);
	manager.AddModule(wrapperModule);
	manager.AddModule(wrapperJsonModule);
	manager.Rebuild(true, nullptr, false);
	if (manager.errors.Count() > 0)
	{
		CopyFrom(errors, manager.errors);
		return nullptr;
	}

	return emitter::GenerateAssembly(&manager);
}
```

The same final manager can be passed to `GenerateCppFiles` to generate C++ for the original Workflow input and both generated RPC wrapper modules.

## Cross-Architecture C++ Output

When generated C++ should be architecture-neutral, run the same logical generation twice:

1. once with `WfCpuArchitecture::x86` and matching x86 reflection metadata;
2. once with `WfCpuArchitecture::x64` and matching x64 reflection metadata.

The caller should require both runs to produce the same generated filename set. For each matching file, call `MergeCppMultiPlatform(code32, code64)`. If there is already an output file containing user implementation regions, call `MergeCppFileContent(existingCode, mergedCode)` before writing the result.

The following reference example shows only the text-level merge. File enumeration and storage are caller policy.

```C++
WString MergeGeneratedCppFile(
	const WString& code32,
	const WString& code64,
	const WString* existingCode)
{
	auto mergedCode = cppcodegen::MergeCppMultiPlatform(code32, code64);
	if (existingCode)
	{
		mergedCode = cppcodegen::MergeCppFileContent(*existingCode, mergedCode);
	}
	return mergedCode;
}
```

`MergeCppMultiPlatform` accepts only expected architecture differences:

- `vint32_t` versus `vint64_t`;
- `vuint32_t` versus `vuint64_t`;
- integer literal suffix differences such as 64-bit `L` or `UL`;
- selected generated `static_cast<::vl::vint32_t>` or `static_cast<::vl::vint64_t>` numeric forms.

Any other difference throws `MergeCppMultiPlatformException` with row and column positions for both inputs. Treat that exception as a generator difference that needs investigation, not as a normal merge conflict.

`MergeCppFileContent` preserves existing user implementation content in generated `USER_CONTENT_BEGIN` / `USER_CONTENT_END` and `USERIMPL` regions. User content that no longer matches a generated region is kept under `UNUSED_USER_CONTENT`, so it can be inspected instead of silently dropped.

## Choosing the Flow

Use the ordinary compile flow when:

- after a successful rebuild with RPC validation enabled, `manager.rpcMetadata` is `nullptr`;
- no generated RPC wrapper modules are needed for the final manager;
- the caller only needs a `WfAssembly`, or C++ generated directly from those modules.

Use the RPC flow when:

- an initial rebuild with RPC validation enabled succeeds and leaves both `manager.rpcMetadata` and `manager.rpcMetadata->metadataModule` non-null;
- generated wrappers, JSON serializers, or `rpc_GetIds` are needed;
- the final assembly or generated C++ must include generated flat or JSON RPC Workflow modules.

Use the x86/x64 merge flow when:

- generated C++ should compile as architecture-neutral code using `vint` and `vuint`;
- the Workflow code or reflected metadata contains CPU-sized integer mappings;
- existing generated C++ files can contain preserved user implementation regions.
