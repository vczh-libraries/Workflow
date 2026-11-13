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
