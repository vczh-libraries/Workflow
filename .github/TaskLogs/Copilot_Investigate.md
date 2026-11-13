# !!!INVESTIGATE!!!

# PROBLEM DESCRIPTION

In commit a12010e764ef33d867ebb53a388f9211cafa308f, `Source/Parser/Syntax/Ast/Expressions.txt` is changed to add `CallExpression::typedResult`.
This field is to cache semantic analysing result for a `CallExpression` node.
But such thing is not good, AST should only contain parsing result in order not to confuse users.

Remove that field, instead add a `Dictionary` to `WfLexicalScopeManager`, following the current pattern.
I would recommend `CallExpressionTypedResultMap` below `BaseConstructorCallResolvingMap` and `baseConstructorCallResolvings`.
Put the cache here.

Run unit test to make sure the change is working.

## DETAILS

The current field is a compiler-generated return-type override, rather than a general cache of ordinary call analysis. Variant `foreach` lowering assigns it to the generated enumerator `GetCurrent()` call so semantic analysis sees the variant element type and C++ generation unboxes the reflected return value accordingly. Preserve this behavior and ordinary call resolution.

- In `<Workflow repo>/Source/Analyzer/WfAnalyzer.h`, add `CallExpressionTypedResultMap` immediately after `BaseConstructorCallResolvingMap`, and its `callExpressionTypedResults` member immediately after `baseConstructorCallResolvings`. Use `collections::Dictionary<Ptr<WfCallExpression>, Ptr<reflection::description::ITypeInfo>>`, following the neighboring expression-cache ownership pattern. Store the already-resolved foreach symbol type rather than converting it back into a `WfType` AST node for caching.
- In `<Workflow repo>/Source/Analyzer/WfAnalyzer_ExpandStatement.cpp`, replace the variant-only `GetCurrent()` assignment with a map entry. In `<Workflow repo>/Source/Analyzer/WfAnalyzer_ValidateSemantic_Expression.cpp`, keep normal overload selection, then use the cached type when an entry exists. In `<Workflow repo>/Source/Cpp/WfCpp_Expression.cpp`, use map membership to preserve the existing `WriteUnboxValue` branch. Calls without an entry retain their current behavior.
- Remove the field-specific type validation from `<Workflow repo>/Source/Analyzer/WfAnalyzer_ValidateStructure_Expression.cpp`. The new cache contains a resolved semantic type; do not replace the removed AST field with another parsing-visible annotation.
- Preserve entries when compiler lowering copies generated call nodes. The generated AST copier currently copies `typedResult` automatically. After removal, extend the existing analyzer copy support around `CopyWithExpandVirtualVisitor` in `<Workflow repo>/Source/Analyzer/WfAnalyzer_ExpandBindExpression.cpp` to transfer entries from original calls to copied calls within the same manager. Audit its derived visitors and the lowering uses of `CopyExpression`, `CopyStatement`, and `CopyDeclaration`, especially nested foreach/switch lowering and `GenerateFlowChartModuleVisitor` in `<Workflow repo>/Source/Analyzer/WfAnalyzer_ExpandNewCoroutineExpression.cpp`. Keep generic parsed-AST copying independent of semantic state.
- Clear the map alongside the other semantic maps in `WfLexicalScopeManager::Clear` in `<Workflow repo>/Source/Analyzer/WfAnalyzer.cpp`. Audit retained expansions as well: `Rebuild` calls `Clear(..., false)`, but context-free desugaring does not discard existing context-sensitive expansions, and semantic validation can reuse them. A cache entry must be available before a retained or copied generated call is revalidated or emitted; coordinate regeneration/repopulation with the existing scope and expansion lifecycle. Compare retained-AST rebuilding with the pre-change behavior, since scope reconstruction for retained expansions also needs consideration. Do not preserve stale resolved types across a clear merely to avoid missing entries.
- Remove the AST declaration from `<Workflow repo>/Source/Parser/Syntax/Ast/Expressions.txt`, then regenerate with `<Tools repo>/Tools/GlrParserGen.exe` using `<Workflow repo>/Source/Parser/Syntax/Parser.xml`. This updates the generated AST, reflection registration, builder, copier, traversal, JSON schema/serialization, and assembler field definitions. Do not hand-edit `<Workflow repo>/Source/Parser/Generated` or `<Workflow repo>/Test/Generated`.

Relevant design guidance is in `<Workflow repo>/.github/KnowledgeBase/KB_Workflow_Design_CompilerRebuildAndDesugaring.md` and `<Workflow repo>/.github/KnowledgeBase/KB_Workflow_AstBuildingHelperFunctions.md`.

## VERIFICATION

These checks are for implementation of the task; this review only updates the task document.

1. Run the existing `<Workflow repo>/Test/Resources/Codegen/VariantIteration.txt` sample through both the VM and generated C++ tests, preserving the indexed result `1s2|2s1|1s2`. It covers forward iteration, reversed iteration, and a variant loop crossing coroutine suspension. Verify that generated C++ still unboxes `GetCurrent()` to the same ordered variant type.
2. Add focused regression coverage for copied generated calls where existing samples do not cover it, such as variant loops nested inside another loop or switch. Verify both semantic resolution of the copied call and its executed result. Include ordinary non-variant calls/iteration as controls.
3. Verify that `Clear` empties the new cache and that reusing a manager with fresh modules does not retain old call nodes or resolved types. Compare repeated `Rebuild` on retained modules against the pre-change baseline and diagnose scope/expansion lifecycle failures separately from cache-transfer failures. The existing codegen sample loop reparses each module and rebuilds it only once, so it does not establish retained-AST behavior.
4. Scan the handwritten source, regenerated parser artifacts, and regenerated parsing dumps for obsolete AST-field `typedResult` references. Confirm that call-expression reflection, builders, copying, traversal, and JSON no longer expose it. Expect parsing-log JSON to lose this field even on ordinary calls where its previous value was null; these generated-text changes are intentional. Investigate unrelated changes to Workflow behavior, generated C++/assembly semantics, or RPC fixture results.
5. Follow the Windows build/test order in `<Workflow repo>/Project.md` using the prescribed wrappers: build Debug Win32 and x64; run `LibraryTest` and `CompilerTest_GenerateMetadata` on both; run `CompilerTest_LoadAndCompile` on x64; rebuild both platforms when generated C++ changes; then run `RuntimeTest`, `CppTest`, `CppTest_Metaonly`, and `CppTest_Reflection` on both. Parser changes require metadata regeneration. Inspect raw build/test logs and any memory-leak output, and update baselines only for verified expected changes.
6. Run `RpcStdioTest_Driver` with the matching `RpcStdioTest_Service` on both platforms, using `<Workflow repo>/Test/StartRpcStdio_SharedMemspSkipList.txt` as required for the native provider. Include the required `<Workflow repo>/Test/StartRpcStdio.ps1` invocation with that skip list. Run `<Workflow repo>/Test/TypeScript/prepare.ps1`, then `npm run build` in `<Workflow repo>/Test/TypeScript`.
7. Complete the canonical `<Tools repo>/Tools/Build.ps1 Workflow` verification required by `<Workflow repo>/Project.md` for the full suite before the implementation's final push. Review generated changes and follow `<Workflow repo>/.github/Rules/document-and-commit.md` when separating source, generated text, and other generated artifacts into commits.

## REVIEW COMMENTS

No unresolved review comments.

# UPDATES

# TEST [CONFIRMED]

- The unwanted field is declared in `<Workflow repo>/Source/Parser/Syntax/Ast/Expressions.txt` and exposed by generated AST, builders, copier, traversal, reflection, assembler, and JSON. The handwritten producer is variant foreach lowering; semantic call validation and C++ unboxing are its consumers.
- Baseline probes in `<Workflow repo>/Test/UnitTest/CompilerTest_LoadAndCompile/TestAnalyzer.cpp` rebuild the same parsed `ForEach` and `VariantIteration` modules twice. The first rebuild succeeds. Both second rebuilds throw `ArrayBase<T, K>::Get(vint)#Argument index not in range.` before changing production code. Scope construction visits original context-sensitive nodes, but subsequent semantic validation also visits retained expansions without rebuilding their scopes. This existing lifecycle failure is separate from moving the cache.
- Preserve `VariantIteration=1s2|2s1|1s2` through both VM and all generated C++ configurations, including the ordered variant unboxing of `GetCurrent()`.
- Add an indexed sample exercising variant loops copied by range loops, enumerable loops, switch lowering, coroutine lowering, and context-free property expansion, with ordinary iteration and calls as controls.
- Replace the baseline probes with successful repeated-rebuild regressions. Verify cache transfer for generated calls, resolved call types, cache clearing, fresh-module reuse, and regeneration after retaining modules. Keep generic copying independent of the manager unless it is supplied explicitly.
- Regenerate parser artifacts through `<Tools repo>/Tools/GlrParserGen.exe`. Scan handwritten source, parser outputs, and parsing dumps for the removed AST field. Review changed parsing JSON separately from C++ and assembly semantics.
- Run the required Debug Win32/x64 build, metadata, compiler, VM, generated-C++, RPC stdio, and TypeScript checks; inspect raw logs for failures and memory leaks. Complete `<Tools repo>/Tools/Build.ps1 Workflow` before the final push.

Completed Debug validation (all test files enabled):

| Check | Win32 | x64 |
| --- | --- | --- |
| Build and rebuild after generated C++ | 0 warnings, 0 errors | 0 warnings, 0 errors |
| LibraryTest | 21/21 cases | 21/21 cases |
| CompilerTest_GenerateMetadata | 2/2 cases | 2/2 cases |
| CompilerTest_LoadAndCompile (x64 executable targeting both architectures) | 873/873 cases | 873/873 cases |
| RuntimeTest | 285/285 cases | 285/285 cases |
| CppTest | 251/251 cases | 251/251 cases |
| CppTest_Metaonly | 251/251 cases | 251/251 cases |
| CppTest_Reflection | 251/251 cases | 251/251 cases |
| RPC stdio launcher, explicit SharedMemsp skip list | 127 cases, exit 0 | 127 cases, exit 0 |

Raw Debug logs contain no memory-leak dump. Both iteration samples return their indexed expected results in VM and C++ execution. TypeScript preparation and `npm run build` also pass.

All 512 changed existing parsing logs compare identically after removing `typedResult` JSON properties; their non-AST sections are unchanged. Existing assembly, generated C++ implementations, and RPC fixtures are unchanged. New fixtures and test registration belong to `VariantIterationCopied`. The source and generated-parser scan finds no obsolete field references.

Canonical `<Tools repo>/Tools/Build.ps1 Workflow` completed with exit code 0. It rebuilt Release Win32 and x64, passed the library, metadata, compiler, runtime, and all three C++ suites on both platforms, passed TypeScript preparation/install/build and Release x64 stdio RPC, regenerated the release bundle, and rebuilt CppMerge. Only the compiler release header/source changed in `<Workflow repo>/Release`; imports and existing RPC/type metadata baselines remain unchanged. The final source, parser, release, and parsing-log scan finds no obsolete `typedResult` field.

# PROPOSALS

- No.1 Own generated-call types in the scope manager and regenerate expansions after clear [CONFIRMED]

## No.1 Own generated-call types in the scope manager and regenerate expansions after clear

Use `CallExpressionTypedResultMap` and `callExpressionTypedResults` in `WfLexicalScopeManager`, immediately after the corresponding base-constructor aliases and member. Store the resolved foreach symbol type, preserving normal overload selection before applying the override and preserving the existing C++ unboxing branch.

Extend `CopyWithExpandVirtualVisitor` with an optional manager and transfer cached entries in its call-expression visitor. Thread the manager through analyzer lowering and derived copying visitors, including bind, range foreach, co-provider, raw coroutine, and state-machine expression copying. Calls to generic copy helpers without a manager remain syntax-only.

Clear the new map with the other semantic tables. When retaining modules, discard virtual expansions before scope reconstruction and regenerate them through the existing pipeline. Include context-free expansions because they can contain copied context-sensitive descendants; retaining those flattened descendants would retain generated calls whose semantic types were cleared. Do not retain resolved types across a clear.

The new property-initializer regression also exposed a C++ collection failure independent of cache transfer. CDB caught a missing `nodeScopes` entry in `WfCollectModuleVisitor::Traverse(WfFunctionExpression*)`, reached through the original auto-property initializer. Only the copied, expanded declaration is analyzed. Collect context-free virtual declarations through their expanded declarations, matching the analyzer's ownership of executable syntax.

### CODE CHANGE

- Added the manager-owned generated-call result cache, populated directly with the variant foreach symbol's resolved type. Normal call overload selection still runs before the override, and C++ generation checks cache membership before the existing unboxing path. Removed AST-field-specific structural validation.
- Extended the analyzer copy visitor and copy helper overloads with an optional manager. Generated-call entries transfer to copies made by foreach, switch, bind, coroutine, co-provider, state-machine, property, and cast lowering. Syntax-only copies remain independent of semantic state.
- Added virtual-expansion invalidation to retained-module clearing and clear the new cache with the other semantic tables. Rebuilding now regenerates both context-free and context-sensitive expansions and their scopes.
- Updated C++ module collection to visit expanded context-free declarations, avoiding unscoped original property-initializer lambdas. Keep the property regression enabled for VM and all C++ configurations.
- Removed the AST field declaration and regenerated the parser artifacts with the prescribed generator.
- Added `VariantIterationCopied`, with expected result `1s21s2|2s12s1|1s2|1s22s1|6|p3`, plus analyzer regressions for direct repeated rebuilds, explicit retained-module clearing, generic module copying, fresh-module reuse, and manager-aware call copying. Updated the compiler rebuild and AST helper knowledge documents.

### CONFIRMED

The generated-call override now belongs to the scope manager and contains the already-resolved variant type. The regressions confirm that ordinary overload selection still resolves `GetCurrent`, while each generated or copied call receives the cached result type. Calls copied without a manager and ordinary calls do not acquire entries. Generated C++ retains the existing unboxing behavior, including both `variant<int, string>` and `variant<string, int>` orderings in the new sample.

The retained-module failure was reproduced before the production change and is now fixed. Six representative modules successfully rebuild repeatedly, including a direct second rebuild of expanded syntax, an explicit retained-module clear, and a generic copy of an analyzed module. The map empties on clear, regenerated calls have fresh identities, and reusing the manager for an ordinary foreach module leaves no variant-call entries. Regenerating virtual expansions restores their scopes as well as their result types, without preserving stale semantic data.

The focused property initializer exposed a separate collection bug: C++ collection traversed the unscoped original lambda instead of its analyzed expansion. Following expanded context-free declarations resolves that failure while preserving all existing generated C++ implementations. The original iteration sample remains `1s2|2s1|1s2`, and the new copied-loop sample returns `1s21s2|2s12s1|1s2|1s22s1|6|p3` in VM and every C++ configuration.

All verification above passed, including the canonical Release sequence. The final comparison of 512 existing parsing logs confirms that their only semantic JSON change is removal of the AST field; existing assembly and RPC results are unchanged. This is the sole confirmed proposal, and its implementation is retained.
