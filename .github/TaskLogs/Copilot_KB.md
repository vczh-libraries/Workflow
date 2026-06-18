# !!!KNOWLEDGE BASE!!!

# DESIGN REQUEST

focus on how WfLexicalScopeManager::Rebuild work:
- what is the purpose of each pass and sub-pass during compiling
- how does Workflow compiler desugar syntax
- how does `bind`, coroutine and state machine syntax are desugared/compiled
do not focus on explanation of workflow syntax, focus on how things are done, it would be for guiding the maintenance of the workflow compiler, not about using the workflow compiler.

# INSIGHT

## Source Map

The main entry point is `WfLexicalScopeManager::Rebuild` in `Source/Analyzer/WfAnalyzer.cpp`.

Important supporting files:

- `WfAnalyzer_ContextFreeDesugar.cpp`: syntax rewrites that do not need type or scope information.
- `WfAnalyzer_ValidateStructure_*.cpp`: structural legality checks before name/type analysis.
- `WfAnalyzer_BuildGlobalNameFromModules.cpp`: namespace/type/member placeholder construction.
- `WfAnalyzer_BuildScope.cpp`: lexical scope and symbol construction.
- `WfAnalyzer_CompleteScope.cpp`: filling symbol/member types once all names exist.
- `WfAnalyzer_CheckScopes_*.cpp`: duplicate symbols, missing symbol types, and declaration dependency cycles.
- `WfAnalyzer_ValidateSemantic_*.cpp`: type checking, overload resolution, and type-dependent virtual AST expansion.
- `WfAnalyzer_ExpandBindExpression.cpp`: `bind` lowering.
- `WfAnalyzer_ExpandNewCoroutineExpression.cpp`: `$coroutine{}` and co-operator expression lowering.
- `WfAnalyzer_ExpandStatement.cpp`: switch, foreach, and co-provider statement lowering.
- `WfAnalyzer_ExpandStateMachine.cpp`: state machine declaration and statement lowering.
- `WfEmitter_*` and `Source/Cpp/WfCpp_*`: bytecode and C++ backends, which consume expanded AST and intentionally reject raw coroutine/state-machine statement nodes.

## Rebuild Pipeline

`Rebuild` is a barriered compiler pipeline. It captures `errorCount = errors.Count()` after `OnInitialize(this)` and later uses `EXIT_IF_ERRORS_EXIST` after each major phase. Since `errorCount` is not reset, any error added by a phase prevents all later phases from running. This is important when adding validations: an error in an early phase must not rely on later derived state being available.

The phases are:

1. Environment load and reset:
   - Calls `callback->OnLoadEnvironment()`.
   - Calls `Clear(keepTypeDescriptorNames, false)`.
   - If `globalName` does not exist, creates the root `WfLexicalScopeName` and loads reflected type names with `BuildGlobalNameFromTypeDescriptors`.
   - Calls `callback->OnInitialize(this)`.

2. Context-free desugaring and structure validation:
   - For each module: `ContextFreeModuleDesugar(this, module)`.
   - For each module: `ValidateModuleStructure(this, module)`.
   - This phase may mutate AST before scope construction. It also checks placement rules that do not require type information, such as no nested `bind`, no `observe` outside `bind`, no `co-pause` outside `$coroutine{}`, no co-operator outside co-provider, state switch/invoke only inside a state, and state machine declarations only as class members.

3. Global name and initial scope construction:
   - `BuildGlobalNameFromModules(this)` adds namespaces, custom type descriptors, and class/type member placeholders.
   - `BuildScopeForModule(this, module)` creates lexical scopes and symbols.
   - `ValidateScopeName(this, globalName)` validates the global name tree.
   - `CheckScopes_DuplicatedSymbol(this)` reports duplicate lexical symbols after all initial scopes exist.

4. Scope completion and scope type checks:
   - `CompleteScopeForModule(this, module)` fills the type information that could not be completed before all global names and scopes were known.
   - `CheckScopes_SymbolType(this)` verifies symbols have valid types.
   - `CheckScopes_CycleDependency(this)` detects declaration/type dependency cycles.

5. Semantic validation and type-dependent expansion:
   - For each module: `callback->OnValidateModule(module)`, then `ValidateModuleSemantic(this, module)`.
   - This is where expressions are resolved into `expressionResolvings`, overloads are selected, lambda captures and interface implementations are recorded, and virtual AST nodes that require type/scope data are expanded.

6. Post-semantic metadata:
   - `PopulateAttributesOnTypeDescriptors(this)` transfers validated Workflow attributes onto generated type descriptors.
   - If `validateRpc` is true, `ValidateModuleRPC(this, module)` validates RPC declarations and generates `rpcMetadata`.

`Clear` resets not only errors and scopes, but also all semantic side tables used by later expansions, including `expressionResolvings`, coroutine resolution maps, `lambdaCaptures`, `interfaceMethodImpls`, declaration metadata maps, and state-machine maps. Any new compiler-maintained analysis cache needs to be cleared here and reconsidered in the semantic recheck path for generated AST.

## Desugaring Model

The AST has two families of virtual nodes:

- `WfVirtualCfe*`: context-free expansion, performed before scope construction.
- `WfVirtualCse*`: context-sensitive expansion, performed during semantic validation after the original node has enough resolved type/scope information.

Context-free desugaring handles syntax that can be rewritten without type information:

- Format expressions parse embedded expressions and lower to a chain of generated expressions.
- Auto properties lower to generated fields, getters, setters, events, and property declarations depending on class/interface/new-interface context.
- Cast-result interfaces lower to helper methods used later by coroutine result conversion.

Context-sensitive desugaring is deliberately later:

- Statement virtual nodes such as switch, foreach, and co-provider statements are first semantically validated, then expanded, then the generated subtree is context-free-desugared, scoped, scope-checked, and semantically validated.
- Expression virtual nodes such as `bind`, `$coroutine{}`, mixin casts, expected-type casts, and delayed co-operator expressions follow the same pattern.
- Declaration virtual nodes such as state machines are expanded into generated declarations, then those declarations are scoped, checked, and semantically validated.

The recheck pattern is visible in `ValidateSemantic_*`:

1. Validate the original virtual node enough to collect required semantic data.
2. If no new errors were introduced, generate `expandedExpression`, `expandedStatement`, or `expandedDeclarations`.
3. Set code ranges on generated nodes.
4. Run context-free desugar on the generated subtree.
5. Build scopes for the generated subtree under the original parent scope.
6. Remove stale entries from `checkedScopes_DuplicatedSymbol` and `checkedScopes_SymbolType` for the affected parent scope.
7. Re-run duplicate-symbol and symbol-type checks.
8. Semantically validate the generated subtree.

This invariant is enforced by the backends. Bytecode and C++ generation dispatch through `expanded*` for virtual nodes, and both backends `CHECK_FAIL` if raw coroutine or state-machine statement nodes reach code generation. Maintenance work should preserve the rule that special syntax is a frontend concern and generated ordinary Workflow AST is what downstream emitters see.

## Bind Lowering

`bind` is a context-sensitive expression. Structure validation only enforces syntactic placement:

- `bind` cannot be nested in another `bind`.
- Manual attach/detach expressions are illegal inside `bind`.
- `observe` is legal only inside `bind`.
- `observe` events cannot contain nested `observe`.
- Simple observe form must use reference-shaped expressions/events.

Semantic validation gives `bind` the readonly type `Ptr<IValueSubscription>` and validates the bound expression first. `ExpandBindExpression` then builds an anonymous `new IValueSubscription` implementation.

The expansion is driven by `BindContext` and `CreateBindContextVisitor`:

- It walks the already-resolved expression tree using `expressionResolvings`.
- It detects observable member expressions by resolved property/event metadata.
- It records explicit observe expressions, parent expressions, observe events, cached expressions, and reference renames from `let` or observe scopes.
- It builds dependency graphs: expression-to-observe effects and observe-to-observe causes. These graphs decide which cached parents and event subscriptions must be refreshed when one event fires.

The generated subscription contains:

- Cache variables such as `<bind-cache>N` for observed parent values.
- Handler variables for attached event callbacks.
- Lifecycle flags such as `<bind-opened>` and `<bind-closed>`.
- An activator method that evaluates the expanded expression and raises `ValueChanged(result)`.
- One callback method per observe event.
- Overrides for `Open`, `Update`, and `Close`.

The generated `Open` method initializes caches in dependency order, attaches handlers, and marks the subscription opened. Generated event callbacks detach affected downstream observes in reverse order, recompute affected caches once, reattach handlers, and then invoke the activator. Generated `Close` detaches all active handlers, clears handler/cache fields to default values, and marks the subscription closed.

Default values for cache reset are built by `CreateDefaultValue`, which is shared with coroutine/state-machine expansion. This makes value-reset behavior consistent across generated compiler code.

Maintenance implication: any new observable expression shape must update both discovery (`CreateBindContextVisitor`) and expression rewrite (`ExpandObserveExpressionVisitor`). Because bind expansion depends on `expressionResolvings`, it must remain a semantic-time expansion.

## Coroutine And Co-Provider Lowering

`$coroutine{}` is a context-sensitive expression that lowers to `new ICoroutine`. Structure validation starts a fresh `ValidateStructureContext` for the coroutine body, records `currentNewCoroutineExpression`, and permits `co-pause` only under that coroutine. Nested `co-pause` is rejected. State-machine statements are rejected during coroutine expansion as an internal invariant.

Semantic validation of `WfNewCoroutineExpression` returns readonly `Ptr<ICoroutine>` and validates the body. `ExpandNewCoroutineExpression` then compiles the body into a resumable state object:

- `FindCoroutineAwaredStatements` marks statements that affect suspension or control-flow splitting: virtual statements, coroutine statements, break/continue/return/goto, try/catch/finally, while, if, and blocks containing aware statements.
- `FindCoroutineAwaredVariables` finds local variables that must survive across generated state nodes.
- `FindCoroutineReferenceRenaming` maps persisted locals and temporary control variables into generated fields.
- `GenerateFlowChartStatementVisitor` converts aware control flow into flow-chart nodes, preserving labeled exits, loop exits, return paths, exception destinations, finally unwinding, and pause destinations.
- `RemoveUnnecessaryNodes` merges trivial continuation nodes and marks branch nodes that can be embedded.

The generated `ICoroutine` implementation contains:

- Fields for renamed variables, initialized with default values.
- `<co-state>` and `<co-state-before-pause>`.
- Override auto properties for `Failure` and `Status`.
- Override `Resume(<raise-exception>, <coroutine-output>)`, implemented as a loop that dispatches on `<co-state>`, handles a previous pause result/failure, executes the current flow-chart node, and updates status/state before returning or continuing.

`co-operator` has two related paths:

- `WfCoOperatorExpression` is delayed until inside the generated coroutine because it needs the generated `<co-impl>` variable. It expands to a provider context query followed by the requested member access.
- `WfCoOperatorStatement` is used inside co-provider functions. Semantic validation resolves the provider method, result conversion helper, and implementation type. Expansion emits a pause around the provider operation, then checks failure/result and optionally declares the requested result variable.

`WfCoProviderStatement` is lowered in `ExpandCoProviderStatement`:

- Semantic validation selects the provider type, resolves static `CreateAndRun` for void-like provider functions or `Create` for value-returning provider functions, and records the selected method in `coProviderResolvings`.
- Expansion wraps the provider body in a generated `$coroutine{}` expression named `<co-result>`.
- It creates an anonymous function that accepts `<co-impl>` and returns `Ptr<ICoroutine>`.
- It calls the provider creator with that function.
- Provider `return` statements are rewritten into `ReturnAndExit(<co-impl>, ...)` plus a final return. If the provider result type exposes `StoreResult`, the returned value is stored through that helper first.

Maintenance implication: coroutine lowering is not just statement rewriting; it is control-flow graph construction with exception/finally semantics. New coroutine-aware syntax must participate in aware-statement discovery, variable persistence, reference renaming, and flow-chart generation.

## State Machine Lowering

State machine declarations are context-sensitive declarations. Structure validation enforces that they are class members, there is at most one state machine per class, and `state switch` / `state invoke` statements appear only inside state bodies.

State machines participate in multiple Rebuild phases before expansion:

- `BuildGlobalNameFromModules` synthesizes class members for each state machine: input methods, input argument fields, state argument fields, and `<state>CreateCoroutine`. It records these in `stateInputMethods`, `stateInputArguments`, `stateDeclArguments`, and `stateMachineInfos`.
- `BuildScopeForModule` adds lexical symbols for inputs, states, state arguments, and switch-case arguments.
- `CompleteScopeForModule` fills generated input method signatures, generated field types, and the create-coroutine method signature.
- Semantic validation requires a default state, validates each state body, resolves state switch cases to generated input methods, assigns case argument symbol types from input method parameters, and validates state invocations against generated state argument symbols.

`ExpandStateMachine` then expands the declaration into generated members:

- It assigns numeric ids to inputs and states; the default state becomes state id `0`.
- It emits private storage fields for input arguments and state arguments.
- It emits input methods that initialize the state machine on first use, store input arguments, set the current input id, and resume the state-machine coroutine.
- It emits `<state>CreateCoroutine(<state>startState:int):void`, which creates a `$coroutine{}` containing the state-machine execution loop.

The generated coroutine contains labeled blocks for leaving the whole state machine and leaving the current state, a local current-state id, and a switch over state ids. Each state case initializes local state arguments from generated fields, executes the expanded state body, and exits when appropriate. A `finally` block restores the previous coroutine reference.

State-machine statements lower inside that generated coroutine:

- `state switch` becomes a pause-if-needed, then a switch over `stateMachineInput`. Cases copy generated input fields into local case arguments and run the copied case body. The configured switch behavior controls whether unhandled input passes, clears the input, returns from the current state, or raises.
- `state invoke` copies invocation arguments into generated state-argument fields. `Goto` updates the current state id and exits the current state loop. `Push` creates another state coroutine for the target state and pauses on it.

Maintenance implication: state machines compile by stacking multiple compiler features. The declaration expansion generates members and a `$coroutine{}`; then normal coroutine lowering compiles the execution loop. Changes in coroutine lowering can therefore affect state machines even if state-machine source files are not touched.

## Maintenance Notes

- Decide whether new syntax is context-free or context-sensitive. Context-free syntax can be expanded before scopes. Context-sensitive syntax must validate the original node first, generate `expanded*`, then rebuild and recheck scopes for the generated subtree.
- Any generated AST that introduces declarations, variables, or member calls must be compatible with `BuildScope`, `CompleteScope`, semantic validation, bytecode emission, and C++ emission.
- If a new pass records analysis side tables, add them to `Clear` and consider whether semantic-time generated AST needs cache invalidation or recomputation.
- Do not rely on logging or callbacks for compiler correctness. The callbacks are progress hooks around environment loading, initialization, and per-module semantic validation.
- Keep generated nodes' code ranges set to the source construct that caused them. Existing expansion paths use this to keep diagnostics attached to the user-written syntax.
- Raw coroutine and state-machine statements are frontend-only. If such nodes reach `WfEmitter_Statement.cpp` or `WfCpp_Statement.cpp`, code generation fails by design.

# ASKS

## QUESTION

explain the underlying algorithm of how to convert $yield or $<HighLevelCoProviderOperator> to trival workflow code, focus on how the code is expressed in internal data structure, and how are control flow rewritten, how original in-function variables are converted to.

### ANSWER

First, the compiler does not have a hardcoded `$yield` node. The lexer defines `COROUTINE_OPERATOR` as a token shaped like `$` followed by an uppercase name. The examples use `$Yield`, `$Join`, and `$Await`. They all become the same internal shape:

- A provider body such as `$ { ... }` or `$Async { ... }` is parsed as `WfCoProviderStatement`.
- A statement operator such as `$Yield i;` is parsed as `WfCoOperatorStatement`.
- A result-producing statement operator such as `var x = $Await async;` is still `WfCoOperatorStatement`, but with `varName = "x"`.
- A context expression such as `$.Context` is parsed as `WfCoOperatorExpression`.

The syntax files proving this are `Source/Parser/Syntax/Syntax/Statements.txt`, `Source/Parser/Syntax/Syntax/Expressions.txt`, and `Source/Parser/Syntax/Lexer.txt`. The generated AST fields are in `WfCoProviderStatement`, `WfCoOperatorStatement`, and `WfCoOperatorExpression` in `Source/Parser/Generated/WorkflowAst.h`.

#### Provider And Operator Resolution

The first important lowering step is semantic. `BuildScopeForStatement::Visit(WfCoProviderStatement*)` creates a provider scope containing two compiler symbols:

- `$PROVIDER`: the selected coroutine provider type.
- `$IMPL`: the provider implementation type passed into the generated coroutine creator.

`ValidateSemanticStatementVisitor::Visit(WfCoProviderStatement*)` fills those symbols:

- If the provider statement has no explicit name, as in `$ { ... }`, the compiler starts from the function return type and searches for `<ReturnTypeDescriptorName>Coroutine`, also checking base type descriptors. For an enumerable return, this is how `EnumerableCoroutine` is selected.
- If the provider statement has a name, as in `$Async { ... }`, the compiler strips the `$`, resolves `Async`, then searches for `AsyncCoroutine`; it can also resolve `AsyncCoroutine` directly.
- If the function return type is `void`, it searches the provider for static `CreateAndRun`.
- Otherwise it searches for static `Create`.
- The selected creator must accept one `Ptr<IValueFunctionProxy<Ptr<ICoroutine>, ImplType>>`; the second generic argument becomes the `$IMPL` type.
- The chosen creator is saved in `manager->coProviderResolvings`.

`ValidateSemanticStatementVisitor::Visit(WfCoOperatorStatement*)` binds each high-level operator:

- It strips the `$` from `opName`, so `$Yield` becomes `Yield` and `$Await` becomes `Await`.
- It searches the selected provider type for static `<OperatorName>AndRead`.
- If `varName` is empty, it also searches for static `<OperatorName>AndPause`.
- Candidate methods must take `$IMPL` as the first parameter.
- Overload selection is run with a dummy first argument for `$IMPL`, followed by the source operator arguments.
- The chosen provider method is saved in `manager->coOperatorResolvings`.

For `var x = $Await ...`, the compiler must also decide the type of `x`. It scans the operator argument types and looks for a static `CastResult(Value)` method returning a non-void type. The selected cast method is saved in `manager->coCastResultResolvings`, and the lexical symbol for `x` receives that return type.

`ValidateSemanticStatementVisitor::Visit(WfReturnStatement*)` has a similar provider-specific branch before normal return checking. Inside an unexpanded `WfCoProviderStatement`, it resolves provider static `ReturnAndExit`, again requiring `$IMPL` as the first parameter, and saves it in `manager->coOperatorResolvings`.

#### Co-Provider Statement Expansion

`ExpandCoProviderStatement` in `WfAnalyzer_ExpandStatement.cpp` turns the whole high-level provider function body into ordinary Workflow AST.

The generated shape is:

```workflow
Provider::Create-or-CreateAndRun(
    func(<co-impl> : Provider::IImpl*) : Coroutine^
    {
        return $coroutine named <co-result> { rewritten provider body };
    }
)
```

This is not emitted as text. It is represented as:

- A `WfFunctionExpression` containing an anonymous `WfFunctionDeclaration`.
- One generated `WfFunctionArgument` named `<co-impl>` typed as the resolved `$IMPL`.
- A `WfNewCoroutineExpression` named `<co-result>`.
- A final `WfCallExpression` to the selected provider `Create` or `CreateAndRun` method.
- A surrounding `WfBlockStatement` assigned to `WfCoProviderStatement::expandedStatement`.

The provider body is copied by `ExpandCoProviderStatementVisitor`, which rewrites provider-specific statements during the copy.

For `$Yield i;`, after semantic binding against `EnumerableCoroutine`, the generated body is conceptually:

```workflow
co-pause
{
    EnumerableCoroutine::YieldAndPause(<co-impl>, i);
}
if (<co-result> is not null)
{
    if (<co-result>.Failure is not null) raise <co-result>.Failure;
}
```

Internally this is a `WfBlockStatement` containing:

- A `WfCoPauseStatement`.
- The pause statement's `statement` is another `WfBlockStatement`.
- That inner block contains a `WfExpressionStatement`.
- The expression is a `WfCallExpression` whose function is a `WfChildExpression` for `EnumerableCoroutine::YieldAndPause`.
- The first argument is a `WfReferenceExpression` named `<co-impl>`.
- The remaining arguments are copied from the original `WfCoOperatorStatement::arguments`.
- The post-pause failure checks are ordinary `WfIfStatement`, `WfTypeTestingExpression`, `WfMemberExpression`, and `WfRaiseExceptionStatement` nodes.

For `var x = $Await async;`, the provider method is usually `AsyncCoroutine::AwaitAndRead`. The expansion is the same pause-and-call pattern, then:

```workflow
if (<co-result>.Failure is not null) raise <co-result>.Failure;
var x = SomeArgumentType::CastResult(<co-result>.Result);
```

The result variable is not assigned directly from the provider call. It is read from the resume input `<co-result>`, converted through the previously resolved `CastResult(Value)` method, and introduced as a normal `WfVariableStatement`. Later coroutine lowering may convert that variable into a coroutine field if it must survive across another pause.

For `return expr;` inside a provider body, `ExpandCoProviderStatementVisitor::Visit(WfReturnStatement*)` generates:

```workflow
Provider::ReturnAndExit(<co-impl>, maybe StoreResult(expr));
return;
```

If the provider function return type has a static `StoreResult(Value)` helper, the return expression is wrapped by that helper before being passed to `ReturnAndExit`.

#### Coroutine Flow-Chart Construction

After `ExpandCoProviderStatement` creates a `WfNewCoroutineExpression`, semantic expression validation expands that coroutine using `ExpandNewCoroutineExpression` in `WfAnalyzer_ExpandNewCoroutineExpression.cpp`.

The key internal data structure is a small flow-chart model:

- `FlowChart` owns `nodes`, `headNode`, `lastNode`, and temporary exception variables.
- `FlowChartNode` contains:
  - `statements`: ordinary copied Workflow statements to execute in this state.
  - `branches`: conditional or unconditional edges.
  - `destination`: the default next node.
  - `exceptionDestination`: where exceptions from this node should jump.
  - `pauseDestination`: where execution resumes after a pause.
  - `action`: currently either `None` or `SetPause`.
  - `exceptionVariable`: the catch variable to assign when entering a catch node.
  - `embedInBranch`: an optimization flag for simple branch destinations.
- `FlowChartBranch` contains an optional condition expression and a destination node.
- `GenerateFlowChartStatementVisitor::ScopeContext` models structured exits for functions, loops, try/finally, and labeled blocks.

The pipeline is:

1. `FindCoroutineAwaredStatements` marks statements that can change coroutine control flow: coroutine statements, virtual statements whose expansion is aware, `break`, `continue`, `return`, `goto`, `if`, `while`, `try`, and blocks containing aware statements.
2. `FindCoroutineAwaredVariables` finds local variable declarations inside aware blocks/statements.
3. `FindCoroutineReferenceRenaming` assigns generated field names for variables and catch/if temporary symbols.
4. `GenerateFlowChart` copies the statement tree into flow-chart nodes, replacing references to renamed symbols.
5. `RemoveUnnecessaryNodes` merges trivial nodes, removes the default destination after a pause sentinel, and marks simple branch destinations for embedding.
6. `ExpandNewCoroutineExpression` emits a `WfNewInterfaceExpression` implementing `ICoroutine`.

#### Variable Conversion

Variables that need to survive across pause boundaries are converted from local variables into fields of the generated anonymous `ICoroutine` implementation.

The renaming map is keyed by `WfLexicalSymbol*`, not by textual names. This matters because generated code can reuse names in nested scopes. `GenerateFlowChartModuleVisitor::Visit(WfReferenceExpression*)` checks `manager->expressionResolvings` for the copied reference, finds the resolved symbol, and if that symbol is in `referenceRenaming`, replaces the copied reference token with the generated field name.

Naming rules in `FindCoroutineReferenceRenaming` are:

- A normal source variable `x` becomes a generated field like `<coN>x`.
- A generated temporary shaped like `<category>local` becomes `<coN-category>local`. For example, foreach expansion creates `<for-begin>i` and `<for-end>i`, and coroutine lowering can rename them to `<co0-for-begin>i` and `<co1-for-end>i`.
- Catch variables and `if` expression variables are also renamed if their enclosing aware statement needs to survive.
- Try/finally temporary exception variables can become names like `<co-tempexvarN>ex`.

When a renamed `WfVariableStatement` appears in an aware region, `GenerateFlowChartStatementVisitor::Visit(WfVariableStatement*)` does not keep it as a local declaration. It emits an assignment:

```workflow
<renamed-field> = copied-original-initializer;
```

Then `ExpandNewCoroutineExpression` emits one `WfVariableDeclaration` in the generated `new ICoroutine` body for every renamed symbol:

- The field name is the generated name from `referenceRenaming`.
- The field type comes from `WfLexicalSymbol::typeInfo`.
- The field initializer is `CreateDefaultValue(symbol->typeInfo)`.

Variables not in the renaming map are copied as ordinary local variables. References to outer function parameters or other outer variables are handled by the normal Workflow function/lambda capture analysis around the generated `WfFunctionExpression`; coroutine field conversion is specifically for locals whose lifetime must cross coroutine state transitions.

In the `$Yield` sample `GetNumbers`, the source loop:

```workflow
for(i in range[0, 4])
{
    $Yield i;
}
```

first has its foreach expanded, then coroutine lowering converts the relevant locals into fields such as:

```workflow
<co0-for-begin>i
<co1-for-end>i
<co2>i
```

Those names appear because the range loop temporaries and loop variable must remain alive after `YieldAndPause` returns to the caller and before the next `Resume`.

#### Pause And Control-Flow Rewriting

The important rewrite is in `GenerateFlowChartStatementVisitor::Visit(WfCoPauseStatement*)`.

A `WfCoPauseStatement` becomes three flow-chart nodes:

1. A node with `action = SetPause`.
2. A node containing a sentinel empty `WfCoPauseStatement`.
3. A resume-continuation node stored in `pauseDestination`.

If the original pause has an inner statement, such as the generated provider call `EnumerableCoroutine::YieldAndPause(<co-impl>, value)`, that inner statement is copied into the `SetPause` node before the sentinel pause.

Later, `ExpandFlowChartNode` converts a `SetPause` node into ordinary Workflow code:

```workflow
SetStatus(Waiting);
<co-state-before-pause> = <co-state>;
<co-state> = NEXT_STATE_AFTER_PAUSE;
... provider call statements ...
return;
```

The sentinel `WfCoPauseStatement` is not emitted downstream. It only tells `ExpandFlowChartNode` to emit `return;` from `Resume`. The backend never sees raw coroutine statements.

On the next `Resume`, the generated coroutine:

- Checks it is currently `Waiting`.
- Sets status to `Executing`.
- If `<co-state-before-pause> != -1`, examines the resume argument, named `<co-result>` for provider-generated coroutines.
- If `<co-result>` is null or has no failure, clears `<co-state-before-pause>`.
- If `<co-result>.Failure` exists, it routes the failure to the correct catch node according to the saved pre-pause state, or raises it if no catch applies.
- Enters `while(true)` and dispatches using `if (<co-state> == N)` blocks.

For normal edges, `ExpandFlowChartNode` emits:

```workflow
<co-state> = NEXT_STATE;
continue;
```

For branch edges, it emits an `if` with the copied condition, then either embeds the destination node or sets `<co-state>` and continues. `break`, `continue`, `goto`, and `return` are represented as edges to the appropriate `ScopeContext` leave/enter nodes. `InlineScopeExitCode` inserts pending finally blocks while walking out of scopes, so structured control flow still observes `finally`.

`try/catch/finally` is represented with `exceptionDestination`. When a node has an exception destination, `ExpandExceptionDestination` wraps node execution in a generated `try/catch`. The catch assigns the caught exception to the renamed exception field, sets `<co-state>` to the catch node, and continues the dispatch loop.

#### What `$Yield` Ultimately Becomes

For `$Yield i` in an enumerable provider:

1. Parser: `WfCoOperatorStatement{ opName = "$Yield", arguments = [i], varName = "" }`.
2. Semantic resolution: `$Yield` binds to `EnumerableCoroutine::YieldAndPause`.
3. Provider expansion: `$Yield i` becomes a `WfCoPauseStatement` whose inner block calls `EnumerableCoroutine::YieldAndPause(<co-impl>, i)`, followed by ordinary failure checks.
4. Coroutine expansion: the pause becomes a state transition that sets `Status = Waiting`, records the pre-pause state, sets the next state, calls `YieldAndPause`, and returns from `Resume`.
5. Variable conversion: loop variables and generated loop temporaries referenced across the pause become fields in the generated `ICoroutine` object, and all references are rewritten to those generated field names.
6. Backend: bytecode/C++ generation sees ordinary generated Workflow declarations, statements, expressions, and `expanded*` nodes. Raw `WfCoOperatorStatement` and `WfCoPauseStatement` do not survive as backend concepts.

`$Await`, `$Join`, or any custom `$HighLevelCoProviderOperator` follow the same compiler algorithm. The only operator-specific behavior comes from reflected provider methods named `<OperatorName>AndRead`, `<OperatorName>AndPause`, `CastResult`, `ReturnAndExit`, `Create`, `CreateAndRun`, and `QueryContext`.

## QUESTION

explain the underlying algorithm of how state machine is converted to trivial workflow code. I believe that compiling state machine should share a lot of constructions with coroutine, explain the shared constructions, similarity and differences

### ANSWER

State-machine lowering is a two-layer transformation:

1. `ExpandStateMachine` converts `WfStateMachineDeclaration` into ordinary class members plus a generated `$coroutine{}` body.
2. `ExpandNewCoroutineExpression` later compiles that generated coroutine body into the same `ICoroutine` implementation shape used by raw coroutines and co-provider coroutines.

So the state-machine compiler shares the coroutine backend, but it does not directly build the coroutine `FlowChart`. Instead, it first writes a normal Workflow program containing state ids, labels, gotos, switches, `try/finally`, and `WfCoPauseStatement`. The coroutine compiler then consumes that generated program.

#### Internal Representation Before Expansion

The parser represents a state machine as `WfStateMachineDeclaration`, a context-sensitive virtual declaration with `expandedDeclarations`.

Its direct children are:

- `WfStateInput`: one `$state_input` declaration, with `name` and function arguments.
- `WfStateDeclaration`: one `$state` body, with `name`, arguments, and a `statement`.
- `WfStateSwitchStatement`: a state-machine statement for `$switch`.
- `WfStateInvokeStatement`: a state-machine statement for `$goto_state` or `$push_state`.

The syntax mapping is in `Source/Parser/Syntax/Syntax/Decls.txt` and `Source/Parser/Syntax/Syntax/Statements.txt`. The AST fields are in `Source/Parser/Generated/WorkflowAst.h`.

Before `ExpandStateMachine` runs, several Rebuild passes prepare side tables:

- `BuildGlobalNameFromModules::BuildClassMemberVisitor::Visit(WfStateMachineDeclaration*)` creates reflected member placeholders on the containing `WfCustomType`:
  - input methods are stored in `manager->stateInputMethods`;
  - input argument cache fields are stored in `manager->stateInputArguments`;
  - state argument cache fields are stored in `manager->stateDeclArguments`;
  - `<state>CreateCoroutine` is stored in `manager->stateMachineInfos[node]->createCoroutineMethod`.
- `BuildScopeForDeclarationVisitor::Visit(WfStateMachineDeclaration*)` adds lexical symbols for state inputs and states, creates function-like scopes for states, and scopes state arguments and state bodies.
- `CompleteScopeForClassMemberVisitor::Visit(WfStateMachineDeclaration*)` fills the input method signatures, cache field types, and `<state>CreateCoroutine(<state>startState:int):void`.
- `ValidateSemanticDeclarationVisitor::Visit(WfClassDeclaration*)` requires a class containing a state machine to inherit from `system::StateMachine`.
- `ValidateSemanticDeclarationVisitor::Visit(WfStateMachineDeclaration*)` requires a default state and validates every state body.
- `ValidateSemanticStatementVisitor::Visit(WfStateSwitchStatement*)` resolves switch cases to `WfStateInput` symbols and assigns switch-case argument types from generated input method parameters.
- `ValidateSemanticStatementVisitor::Visit(WfStateInvokeStatement*)` resolves target states and validates invocation arguments against the target state's argument types.

`WfStateMachineInfo` is the expansion-side state table. It stores `createCoroutineMethod`, `inputIds`, and `stateIds`. `inputIds` and `stateIds` are filled by `ExpandStateMachine`, not by the earlier name-building pass.

#### Generated Class Storage

`ExpandStateMachine` first turns state-machine declarations into class storage.

For each state input argument it emits a private field:

```workflow
var <stateip-INPUT>ARG : ArgType = default;
```

For each state declaration argument it emits a private field:

```workflow
var <statesp-STATE>ARG : ArgType = default;
```

These fields are added to `WfStateMachineDeclaration::expandedDeclarations` as `WfVariableDeclaration` nodes with `@cpp:Private`. The declarations are also connected back to their existing reflected fields through `manager->declarationMemberInfos`.

This is one major difference from raw coroutine lowering. Raw coroutine lowering hoists locals into fields only when the coroutine flow-chart says they must survive a pause. State-machine lowering creates class fields up front for semantic state: input arguments and state invocation arguments must survive across input method calls, across state transitions, and across pushed child states.

#### Generated Input Methods

For each `$state_input`, `ExpandStateMachine` emits a normal class method using the already-created `WfClassMethod` from `stateInputMethods`.

The generated input method does this:

1. If `this.stateMachineInitialized` is false:
   - sets it to true;
   - calls `this.<state>CreateCoroutine(0)` to create the default-state coroutine;
   - calls `this.ResumeStateMachine()` once, so the default state can run until it waits for input.
2. Stores the input id into `this.stateMachineInput`.
3. Copies input method arguments into the generated `<stateip-INPUT>ARG` fields.
4. Calls `this.ResumeStateMachine()` again to deliver the input to the current state coroutine.

This generated method is ordinary Workflow AST: `WfFunctionDeclaration`, `WfIfStatement`, `WfBinaryExpression`, `WfMemberExpression`, `WfCallExpression`, and `WfExpressionStatement`.

#### Generated `<state>CreateCoroutine`

`ExpandStateMachine` emits a private method:

```workflow
func <state>CreateCoroutine(<state>startState : int) : void
```

This method is the bridge into shared coroutine machinery. It constructs a `WfNewCoroutineExpression` and assigns it to `stateMachineCoroutine`.

The generated method begins with:

- `var <state>stateMachineObject = this`;
- `var <state>previousCoroutine = <state>stateMachineObject.stateMachineCoroutine`;
- `<state>stateMachineObject.stateMachineCoroutine = $coroutine { ... }`.

The generated coroutine body has:

- a `try/finally`;
- a protected block labeled `<state-label>OUT_OF_STATE_MACHINE`;
- a `finally` block that restores `<state>stateMachineObject.stateMachineCoroutine = <state>previousCoroutine`;
- local `<state>state = <state>startState`;
- a `while(true)` whose body is labeled `<state-label>OUT_OF_CURRENT_STATE`;
- local `<state>currentState = <state>state`;
- assignment `<state>state = -1`;
- a `WfSwitchStatement` over `<state>currentState`;
- one switch case per declared state.

Each state case:

1. Copies generated state argument fields `<statesp-STATE>ARG` into local state argument variables.
2. Copies the original state body through `ExpandStateMachineStatementVisitor`.
3. Emits `goto <state-label>OUT_OF_STATE_MACHINE` as the default fall-through behavior for the state case.

This generated code intentionally uses normal Workflow control-flow constructs. The later coroutine pass understands labels, gotos, try/finally, loops, switches, and `co-pause`.

#### State Switch Lowering

`ExpandStateMachineStatementVisitor::Visit(WfStateSwitchStatement*)` rewrites `$switch` into ordinary Workflow statements.

The generated shape begins with:

```workflow
if (<state>stateMachineObject.stateMachineInput == -1)
{
    $pause;
}
switch (<state>stateMachineObject.stateMachineInput)
{
    ...
}
```

The `WfCoPauseStatement` is the same coroutine pause node used by raw coroutine lowering. It means "this state cannot continue until an input method calls `ResumeStateMachine` again."

For each explicit switch case:

- the case expression is the numeric input id from `smInfo->inputIds`;
- the case block first clears `stateMachineInput` back to `-1`;
- input arguments are copied from `<stateip-INPUT>ARG` fields into local switch-case argument variables;
- the original case body is copied.

If the switch type is `WfStateSwitchType::Default`, the expander also generates cases for every input not explicitly listed. Those cases clear the input and raise a generated exception saying that the input method cannot be called at this moment.

The generated default branch depends on `WfStateSwitchType`:

- `Default`: no default branch; missing inputs were already turned into raising cases.
- `Pass`: no default branch; unhandled input remains available for later state code.
- `PassAndReturn`: `goto <state-label>OUT_OF_STATE_MACHINE`.
- `Ignore`: clears `stateMachineInput`.
- `IgnoreAndReturn`: clears `stateMachineInput`, then `goto <state-label>OUT_OF_STATE_MACHINE`.

The generated statement is itself a block containing a `WfSwitchStatement`. Since `WfSwitchStatement` is another context-sensitive virtual statement, it is later expanded by the normal switch expander into trivial conditional Workflow code.

#### State Invoke Lowering

`ExpandStateMachineStatementVisitor::Visit(WfStateInvokeStatement*)` handles both state invocation modes.

For both `Goto` and `Push`, it first copies invocation arguments into generated state argument fields:

```workflow
<state>stateMachineObject.<statesp-STATE>ARG = copiedArgument;
```

For `WfStateInvokeType::Goto`, it then emits:

```workflow
<state>state = TargetStateId;
goto <state-label>OUT_OF_CURRENT_STATE;
```

This is a same-coroutine transition. It changes the domain state variable and jumps to the end of the current loop iteration. The generated `while(true)` re-enters with the new state id.

For `WfStateInvokeType::Push`, it emits:

```workflow
<state>stateMachineObject.<state>CreateCoroutine(TargetStateId);
$pause;
```

This creates a child state coroutine by replacing `stateMachineCoroutine`, then pauses the current coroutine. The runtime stack behavior is not implemented by a compiler-specific stack structure; it is encoded by the generated `previousCoroutine` local and the `finally` block that restores `stateMachineCoroutine`.

`system::StateMachine::ResumeStateMachine` drives this protocol:

- If the current coroutine changes after a resume and the previous coroutine has not stopped, it means a child state was pushed.
- If the current coroutine changes after a resume and the resumed coroutine has stopped, it means a child state has finished and control returns to the restored parent coroutine.
- A child failure is packed into a `CoroutineResult` and delivered back to the parent coroutine as the next resume input.

#### Shared Constructions With Coroutine Lowering

State-machine lowering shares the following constructions with coroutine lowering:

- It generates a `WfNewCoroutineExpression`; the rest is compiled by `ExpandNewCoroutineExpression`.
- It generates `WfCoPauseStatement` nodes for waiting on input and for `Push`.
- It relies on ordinary Workflow `try/finally`, labels, `goto`, `while`, `switch`, and local variables; the coroutine flow-chart builder already knows how to split these across resumes.
- It relies on `CreateDefaultValue` for generated field initialization.
- It uses the same virtual-node expansion pipeline: generated declarations are context-free-desugared, scoped, duplicate/type-checked, and semantically validated before backend code generation.
- It ultimately produces an anonymous `ICoroutine` implementation with `<co-state>`, `<co-state-before-pause>`, `Failure`, `Status`, and `Resume`, exactly like raw `$coroutine{}`.
- Locals in the generated coroutine body can be renamed into coroutine fields by `FindCoroutineReferenceRenaming`, just like user-written coroutine locals.

The generated state-machine samples show names such as:

```workflow
<co1-state>currentState
<co2-state>state
<co0-switch>...
<co-state>
<co-state-before-pause>
```

These are not created directly by `ExpandStateMachine`. `ExpandStateMachine` creates names like `<state>state` and switch temporaries; coroutine lowering then renames surviving generated locals into `<co...>` fields when they cross pause/control-flow boundaries.

#### Similarities

State-machine lowering and coroutine lowering are similar in these ways:

- Both are frontend transformations into ordinary Workflow AST before bytecode/C++ emission.
- Both use `WfCoPauseStatement` as the abstract suspension marker.
- Both depend on `ExpandNewCoroutineExpression` for actual resumable control-flow splitting.
- Both encode execution position as integers and dispatch with generated conditions.
- Both route failures through `CoroutineResult` and generated exception handling.
- Both preserve `try/finally` behavior by generating ordinary `try/finally` and letting coroutine flow-chart construction inline or route exits correctly.
- Both leave the backends with expanded ordinary declarations/statements; raw coroutine/state-machine nodes should not reach `WfEmitter_Statement.cpp` or `WfCpp_Statement.cpp`.

#### Differences

The important differences are:

- Raw coroutine lowering starts from a statement body and directly builds a coroutine `FlowChart`. State-machine lowering starts from a class declaration and first generates input methods, storage fields, and a generated coroutine method.
- Raw coroutine state ids are implementation states in `FlowChartNode` order. State-machine ids in `smInfo->stateIds` are domain states such as `default`, `ProcessA`, and `ProcessB`. After state-machine expansion, coroutine lowering adds a second layer of implementation states through `<co-state>`.
- Raw coroutine local persistence is discovered entirely by `FindCoroutineAwaredVariables`. State-machine semantic persistence is explicit: input arguments and state arguments are stored in generated class fields before the coroutine pass runs.
- Raw coroutine resumption receives a `CoroutineResult` directly from whoever calls `Resume`. State-machine resumption is triggered by generated input methods through `StateMachine::ResumeStateMachine`; input payload is delivered through `stateMachineInput` and generated input argument fields.
- `Goto` is a domain-state transition inside the same generated coroutine. `Push` creates a nested coroutine and depends on `StateMachine::ResumeStateMachine` plus the generated `finally` restore to return to the parent.
- State-machine switch handling has input-specific policies (`Default`, `Pass`, `PassAndReturn`, `Ignore`, `IgnoreAndReturn`). Raw coroutine pause has no analogous input dispatch policy; it just suspends and resumes with a `CoroutineResult`.

#### What The Backend Sees

After all expansions, backend code generation sees:

- generated input methods;
- generated private storage fields;
- generated `<state>CreateCoroutine`;
- a generated `new ICoroutine` expression inside that method;
- the normal coroutine-generated implementation of `Resume`;
- ordinary Workflow statements and expressions.

It does not emit `WfStateMachineDeclaration`, `WfStateSwitchStatement`, or `WfStateInvokeStatement` directly. Those are maintenance-time frontend constructs, while the shared coroutine machinery is the actual lowering path to executable trivial Workflow code.

## QUESTION

explain the underlying algorithm of bind converts an expression to a listenable interface implementation, focus on how the code is expressed in internal data structure, and how are control flow rewritten, how original in-function variables are converted to.

### ANSWER

`bind` lowering is not a backend feature. It is a semantic-time AST rewrite from `WfBindExpression` into a generated `WfNewInterfaceExpression` whose type is `Ptr<IValueSubscription>`. The generated object implements the runtime interface declared as `IValueSubscription` in `Import/VlppReflection.h`: it has a `ValueChanged` event and `Open`, `Update`, and `Close` methods.

#### Internal Representation

The parser creates:

- `WfBindExpression`, a `WfVirtualCseExpression` with `expression` and `expandedExpression`.
- `WfObserveExpression`, a normal expression with `parent`, `observeType`, `name`, `expression`, and `events`.
- `WfObserveType::SimpleObserve` for `parent.observe(expression on events...)`.
- `WfObserveType::ExtendedObserve` for `parent.observe as name(expression on events...)`.

The syntax comes from `Source/Parser/Syntax/Syntax/Expressions.txt`; the AST classes are in `Source/Parser/Generated/WorkflowAst.h`.

Before expansion:

- `ValidateStructureExpressionVisitor::Visit(WfBindExpression*)` rejects nested `bind`.
- `ValidateStructureExpressionVisitor::Visit(WfObserveExpression*)` requires `observe` to appear under `bind`, rejects nested `observe` inside observe-event expressions, checks simple observe shapes, and requires extended observe to have at least one event.
- `ValidateStructureExpressionVisitor::Visit(WfAttachEventExpression*)` and `Visit(WfDetachEventExpression*)` reject explicit attach/detach inside `bind`.
- `BuildScopeForExpression::Visit(WfObserveExpression*)` creates a local scope for extended observe and adds the alias symbol named by `observe as`.
- `ValidateSemanticExpressionVisitor::Visit(WfBindExpression*)` validates the original bound expression and returns readonly `Ptr<IValueSubscription>`.
- `ExpandVirtualExpressionVisitor::Visit(WfBindExpression*)` calls `ExpandBindExpression`, stores the generated subscription in `node->expandedExpression`, and then the generic virtual-expression path desugars, scopes, checks, and semantically validates the generated subtree.

#### BindContext

`ExpandBindExpression` first builds a `BindContext` by running `CreateBindContextVisitor` over the already-resolved original expression. `BindContext` is the compiler's data model for the binding graph:

- `observeParents`: observed expression -> parent expression whose current value must be cached.
- `observeEvents`: observed expression -> reflected events to attach.
- `orderedObserves`: observes in source discovery order.
- `cachedExprs`: parent expressions that become `<bind-cache>N` fields.
- `renames`: reference expression -> expression substituted during dependency analysis and expression rewrite.
- `exprAffects`: observe -> expressions that may change when this observe fires.
- `exprCauses`: expression -> observes that may cause this expression to change.
- `observeAffects`: observe -> downstream observes whose parent caches depend on this observe.
- `observeCauses`: observe -> upstream observes this observe depends on.

There are two kinds of observed nodes:

- Implicit observable member access: `CreateBindContextVisitor::Visit(WfMemberExpression*)` checks `manager->expressionResolvings[node].propertyInfo`. If the property has `GetValueChangedEvent()` or an event named `<PropertyName>Changed`, the member expression is treated as an observe, its parent is cached, and that event is recorded.
- Explicit `observe`: `CreateBindContextVisitor::Visit(WfObserveExpression*)` records the observe itself, caches its parent, visits the observed value expression, and records every resolved event expression from `manager->expressionResolvings`.

`renames` is not closure capture. It is an expression-level substitution table:

- A reference to a `let` variable is renamed to that let variable's value expression.
- A reference to an extended observe alias is renamed to the observe parent.

This lets dependency discovery see through source conveniences. For example, `let a = x.bar in ...` and `x.observe as a(...)` do not create separate binding storage merely because the source introduced a name.

#### Dependency Algorithm

`CreateBindContextVisitor::DirectDepend(expr, depended)` means: if `depended` can be changed by some observe, then `expr` can also be changed by that observe. It propagates `exprCauses[depended]` into `exprCauses[expr]` and adds reverse entries to `exprAffects`.

`CreateBindContextVisitor::ObservableDepend(expr, parent)` does more:

1. Adds `expr` to `orderedObserves`.
2. Records `observeParents[expr] = parent`.
3. Makes `expr` depend on `parent`.
4. Adds `parent` or its renamed source to `cachedExprs`.
5. If the parent is itself caused by previous observes, records those observes as causing this observe in `observeCauses` and records this observe as affected by them in `observeAffects`.
6. Records the observe as causing itself in `exprAffects` and `exprCauses`.

This graph is the reason callbacks can refresh only the affected part of the binding. If an event changes a parent object, the compiler knows which downstream event subscriptions must be detached, which cached parent expressions must be recalculated, and which handlers must be attached again.

#### Generated Subscription Object

`ExpandBindExpression` creates:

```workflow
new (::system::Subscription^)
{
    ...
}
```

Internally this is a `WfNewInterfaceExpression` typed as `Ptr<IValueSubscription>`.

The generated declarations include:

- `var <bind-cache>N : ParentType = default;` for every cached parent expression.
- `var <bind-handler>I_J : EventHandler^ = null;` for every observed event.
- `var <bind-opened> : bool = false;`.
- `var <bind-closed> : bool = false;`.
- `func <bind-activator>()`.
- `func <bind-callback>I_J(...)` for every observed event.
- `override func Open() : bool`.
- `override func Update() : bool`.
- `override func Close() : bool`.

`CreateWritableVariable` and `CreateDefaultValue` build the generated fields and their initial values. The handler fields are `Ptr<IEventHandler>`, and callback signatures are copied from each event's reflected handler type.

#### Expression Rewrite

The bound expression is rewritten by `ExpandObserveExpressionVisitor`.

For any expression that is in `cachedExprs`, or that resolves through `renames` to a cached expression, the rewritten expression uses the cache field:

```workflow
<bind-cache>N
```

For a `let`, any let variable whose value is already cached is removed from the generated `let`; the remaining variables are copied with rewritten value expressions.

For an explicit observe:

- Simple observe becomes member access on the rewritten parent, such as `parent.property`.
- Extended observe becomes the rewritten `expression` field of the observe node.

The activator method is then:

```workflow
func <bind-activator>() : void
{
    var <bind-activator-result> = rewritten-original-expression;
    ValueChanged(<bind-activator-result>);
}
```

So `bind` does not keep the original expression tree as a special runtime object. It compiles the original expression into normal Workflow code inside a generated method, with observed parent subexpressions replaced by generated cache fields.

#### Cache Assignment And Null/Fault Protection

`CreateBindCacheAssignStatement` emits an assignment for an observed parent cache:

```workflow
<bind-cache>N = expanded-parent-expression ?? default(parent-type);
```

In the AST this is `WfBinaryExpression` with `op = WfBinaryOperator::FailedThen`, not a special bind statement. This protects subscription maintenance: if reevaluating an observed parent fails, the cache is reset to the default value for its type, which is usually `null` for reference types.

Event expressions are generated by `ExpandObserveEvent`:

- For simple observe, the event is selected from the cached parent, such as `<bind-cache>N.valueChanged`.
- For extended observe, the explicit event expression is copied and rewritten.
- For implicit property observe, the event name comes from `IPropertyInfo::GetValueChangedEvent()` or `<PropertyName>Changed`, again under the cached parent.

#### Control Flow Rewrite

`bind` rewrites control flow into lifecycle methods and event callbacks. It does not build a coroutine flow chart and it does not split a function body. The original expression becomes code that runs inside `<bind-activator>`, while event delivery controls when that activator is called.

`Open` is generated by `CreateBindOpenFunction`:

1. If `<bind-opened>` is false, set it to true.
2. Assign all caches in dependency order.
3. Attach every observed event handler.
4. Return true.
5. If already opened, return false.

The dependency order is computed from `observeCauses`. The algorithm repeatedly finds observes whose upstream observe dependencies have already been removed from the pending set. It assigns each parent cache only once by tracking `assignedParents`.

`Update` is generated by `CreateBindUpdateFunction`:

1. If opened and not closed, call `<bind-activator>()`.
2. Return true in that case.
3. Otherwise return false.

`Close` is generated by `CreateBindCloseFunction`:

1. If `<bind-closed>` is false, set it to true.
2. Detach all handlers that are still non-null.
3. Reset every cache field to its default value.
4. Reset every handler field to its default value.
5. Return true.
6. If already closed, return false.

Each event callback is generated in `ExpandBindExpression`:

1. Start with the observe whose event fired.
2. Compute the transitive affected observe list through `observeAffects`.
3. Remove the original observe from the affected list; the event already tells the compiler that the observed value changed, but downstream cached parent expressions may need to be rebuilt.
4. Detach affected downstream observes in reverse order.
5. Recompute each affected parent cache once.
6. Reattach affected downstream observes.
7. Call `<bind-activator>()`.

The reverse detach and forward attach order avoid leaving handlers on stale child objects. The generated sample `Test/Generated/Workflow64/Parsing.Codegen.BindLet.txt` shows this shape: a parent-change callback detaches handlers under old cached child objects, recomputes child caches like `<bind-cache>5 = <bind-cache>0.bar ?? null`, reattaches handlers, then invokes `<bind-activator>()`.

#### Original Function Variables

There are three different "variable conversion" mechanisms involved, and they should not be conflated:

1. Observed parent expressions become generated cache fields. For example, if the binding observes `x.bar.value`, the parent objects needed to attach events become `<bind-cache>N` variables in the generated subscription object.
2. `let` variables and extended-observe aliases are expression-renamed through `BindContext::renames`. They are not necessarily generated as fields. If their value is cached, references are replaced by `<bind-cache>N`; otherwise the `let` can remain in the activator.
3. Original in-function variables from the surrounding function are captured by the normal anonymous-interface capture machinery.

The third point is the important one for maintenance. `BuildScopeForExpression::Visit(WfNewInterfaceExpression*)` creates a shared `WfLexicalCapture` for the generated anonymous interface and its generated member functions. During semantic resolution, references crossing from the generated subscription methods to the outer function are recorded in `manager->lambdaCaptures`.

That means a source local like `x` in:

```workflow
var x = new Foo();
var sub = bind(x.bar.value);
```

is not rewritten into a bind-specific field by `ExpandBindExpression`. The generated methods refer to `x` while assigning caches or evaluating the activator, and the ordinary closure analysis captures `x` into the generated interface implementation. Bytecode metadata names these captured values as `<captured>x`; generated bind-owned fields are names like `<bind-cache>0`, `<bind-handler>0_0`, `<bind-opened>`, and `<bind-closed>`.

This is why bind can use ordinary Workflow capture behavior for outer locals while still maintaining its own cache and handler fields for subscription state.

#### Backend Shape

After semantic revalidation, bytecode and C++ generation see only ordinary nodes:

- `WfNewInterfaceExpression`;
- `WfVariableDeclaration`;
- `WfFunctionDeclaration`;
- `WfIfStatement`, `WfReturnStatement`, and `WfExpressionStatement`;
- `WfAttachEventExpression` and `WfDetachEventExpression`;
- rewritten member/reference/call expressions.

`WfEmitter_Expression.cpp` and `WfCpp_Expression.cpp` both dispatch `WfVirtualCseExpression` through `expandedExpression`. Raw `WfBindExpression` is therefore only a frontend node, and raw `WfObserveExpression` should only exist inside the source expression that was used to build the generated subscription.

# DRAFT

## DRAFT REQUEST

design explanation

## IMPROVEMENTS

### IMPROVEMENT

you need to explain how co provider converting to raw coroutine and then to trivial workflow code, re organize the coroutine section in the draft

## DESIGN EXPLANATION (Workflow)

Add a new topic under `### Design Explanation` in `.github/KnowledgeBase/Index_Workflow.md`.

Suggested index topic:

```markdown
#### Compiler Rebuild And Desugaring

Workflow compiler analysis is organized around `WfLexicalScopeManager::Rebuild`, a barriered frontend pipeline that validates modules, builds names and scopes, resolves semantics, and rewrites high-level Workflow syntax into ordinary AST before bytecode or C++ generation.

- `Rebuild` separates context-free rewrites, structure checks, global-name construction, scope construction, scope completion, semantic validation, and post-semantic metadata population.
- Context-sensitive virtual AST nodes such as `bind`, `$coroutine`, co-provider statements, and state machines are expanded only after their original nodes have enough resolved type and scope information.
- `bind` lowers to an anonymous `IValueSubscription` implementation; co-provider syntax first lowers to a generated raw `WfNewCoroutineExpression`, and that raw coroutine then lowers to a generated `ICoroutine` implementation; state machines lower to generated class members plus a generated coroutine.
- Backends consume `expanded*` ordinary Workflow AST and treat raw high-level nodes as frontend-only constructs.

[Design Explanation](./KB_Workflow_Design_CompilerRebuildAndDesugaring.md)
```

Place it before `#### Attribute System`, because it describes core compiler frontend architecture that other Workflow design topics can rely on.

## DOCUMENT

# Workflow Compiler Rebuild And Desugaring

## How the Workflow analyzer turns parsed modules and high-level Workflow constructs into ordinary AST for bytecode and C++ generation.

This document describes the maintenance-facing design of the Workflow compiler frontend. It focuses on `WfLexicalScopeManager::Rebuild` in `Source/Analyzer/WfAnalyzer.cpp`, the analysis passes it coordinates, and how high-level syntax such as `bind`, co-provider operators, raw coroutines, and state machines is lowered before the emitters run.

The central invariant is that special syntax is a frontend concern. Bytecode generation in `Source/Emitter` and C++ generation in `Source/Cpp` consume ordinary Workflow AST, or virtual nodes whose `expanded*` field points to ordinary Workflow AST. Raw high-level nodes should not be treated as backend concepts.

## Source Map

The compiler frontend is spread across these files:

- `Source/Analyzer/WfAnalyzer.cpp`: owns `WfLexicalScopeManager`, `Clear`, `Rebuild`, and shared manager state.
- `Source/Analyzer/WfAnalyzer_ContextFreeDesugar.cpp`: performs rewrites that do not need type or scope information.
- `Source/Analyzer/WfAnalyzer_ValidateStructure_*.cpp`: checks syntax placement and structural legality before name/type analysis.
- `Source/Analyzer/WfAnalyzer_BuildGlobalNameFromModules.cpp`: creates global names, reflected custom types, and class/type member placeholders.
- `Source/Analyzer/WfAnalyzer_BuildScope.cpp`: creates lexical scopes, symbols, lambda captures, observe scopes, coroutine scopes, and state-machine scopes.
- `Source/Analyzer/WfAnalyzer_CompleteScope.cpp`: fills symbol and member type information after all global names and scopes exist.
- `Source/Analyzer/WfAnalyzer_CheckScopes_*.cpp`: checks duplicate symbols, missing symbol types, and declaration dependency cycles.
- `Source/Analyzer/WfAnalyzer_ValidateSemantic_*.cpp`: resolves expressions, overloads, captures, interface implementations, and context-sensitive expansion.
- `Source/Analyzer/WfAnalyzer_ExpandBindExpression.cpp`: lowers `bind`.
- `Source/Analyzer/WfAnalyzer_ExpandStatement.cpp`: lowers `switch`, `foreach`, co-provider statements, and co-operator statements.
- `Source/Analyzer/WfAnalyzer_ExpandNewCoroutineExpression.cpp`: lowers `$coroutine{}` to `ICoroutine`.
- `Source/Analyzer/WfAnalyzer_ExpandStateMachine.cpp`: lowers state-machine declarations and state-machine-only statements.
- `Source/Emitter/*` and `Source/Cpp/*`: consume expanded AST for bytecode and C++ output.

Parser syntax and generated AST definitions are in:

- `Source/Parser/Syntax/Syntax/Expressions.txt`.
- `Source/Parser/Syntax/Syntax/Statements.txt`.
- `Source/Parser/Syntax/Syntax/Decls.txt`.
- `Source/Parser/Syntax/Lexer.txt`.
- `Source/Parser/Generated/WorkflowAst.h`.

Generated sample outputs under `Test/Generated/Workflow32` and `Test/Generated/Workflow64` are useful for checking the printed shape of expansions. They are generated files and should not be edited directly.

## Rebuild Pipeline

`WfLexicalScopeManager::Rebuild` is a barriered compiler pipeline. It captures `errorCount = errors.Count()` after initialization and uses `EXIT_IF_ERRORS_EXIST` after major phases. Because `errorCount` is not reset between phases, any error added by a phase prevents later phases from running. A validation added to an early phase must not depend on state that is only built by a later phase.

### Environment Load And Reset

The first phase:

- calls `IWfCompilerCallback::OnLoadEnvironment`;
- calls `Clear(keepTypeDescriptorNames, false)`;
- creates `globalName` if needed;
- loads reflected type names with `BuildGlobalNameFromTypeDescriptors`;
- calls `IWfCompilerCallback::OnInitialize`.

`Clear` resets more than modules and errors. It clears semantic side tables such as `expressionResolvings`, coroutine resolution maps, `lambdaCaptures`, `interfaceMethodImpls`, declaration metadata maps, and state-machine maps. Any new analysis cache maintained by `WfLexicalScopeManager` should be cleared here and considered when generated AST is revalidated.

### Context-Free Desugaring And Structure Validation

For each module, `Rebuild` calls:

- `ContextFreeModuleDesugar(this, module)`;
- `ValidateModuleStructure(this, module)`.

Context-free desugaring rewrites syntax that can be transformed without resolved types or scopes. Existing examples include format expressions, auto properties, and generated helper interfaces used by coroutine result conversion.

Structure validation checks placement rules that do not require semantic information. Examples include:

- no nested `bind`;
- no `observe` outside `bind`;
- no explicit `attach` or `detach` inside `bind`;
- no `co-pause` outside `$coroutine{}`;
- no co-operator outside a co-provider statement;
- no state-machine switch or invoke outside a state body;
- state-machine declarations only as class members.

### Global Names And Initial Scopes

The next phase creates the name and initial scope model:

- `BuildGlobalNameFromModules(this)` adds namespaces, Workflow-defined type descriptors, and class/type member placeholders.
- `BuildScopeForModule(this, module)` builds lexical scopes and symbols.
- `ValidateScopeName(this, globalName)` validates the global-name tree.
- `CheckScopes_DuplicatedSymbol(this)` reports duplicate symbols after scopes exist.

This phase is intentionally before type completion. It gives the compiler a complete declaration graph before signatures and member types are fully filled.

### Scope Completion And Scope Checks

The next phase calls:

- `CompleteScopeForModule(this, module)`;
- `CheckScopes_SymbolType(this)`;
- `CheckScopes_CycleDependency(this)`.

`CompleteScopeForModule` fills type information that could not be known until all names and scopes were available. State machines rely heavily on this phase because generated input methods, generated cache fields, and the generated `<state>CreateCoroutine` method all need completed signatures and field types.

### Semantic Validation And Type-Dependent Expansion

For each module, `Rebuild` then calls:

- `IWfCompilerCallback::OnValidateModule(module)`;
- `ValidateModuleSemantic(this, module)`.

This phase resolves expression types into `expressionResolvings`, selects overloads, records lambda captures, records interface method implementations, validates type-dependent rules, and expands context-sensitive virtual AST nodes.

High-level constructs such as `bind`, `$coroutine{}`, co-provider statements, switch/foreach statement virtual nodes, and state machines are deliberately expanded here because they depend on semantic side tables produced by earlier validation.

### Post-Semantic Metadata

After semantic validation:

- `PopulateAttributesOnTypeDescriptors(this)` transfers validated Workflow attributes onto generated type descriptors.
- `ValidateModuleRPC(this, module)` runs when `validateRpc` is true and can populate `rpcMetadata`.

The compiler callback is a progress hook, not part of compiler correctness. Logging and external diagnostics can be implemented differently by different callers.

## Virtual AST Expansion Model

Workflow has two important virtual-node families:

- `WfVirtualCfe*`: context-free virtual nodes expanded before scope construction.
- `WfVirtualCse*`: context-sensitive virtual nodes expanded during semantic validation.

The common context-sensitive recheck pattern is:

1. Validate the original virtual node enough to collect semantic data.
2. If no new errors were added, create `expandedExpression`, `expandedStatement`, or `expandedDeclarations`.
3. Attach source code ranges from the original node to generated nodes for diagnostics.
4. Run context-free desugaring on the generated subtree.
5. Build scopes for the generated subtree under the original parent scope.
6. Remove stale duplicate-symbol and symbol-type check records for the affected parent scope.
7. Re-run `CheckScopes_DuplicatedSymbol` and `CheckScopes_SymbolType`.
8. Semantically validate the generated subtree.

This pattern is visible in `ValidateSemantic_Expression.cpp`, `ValidateSemantic_Statement.cpp`, and `ValidateSemantic_Declaration.cpp`. It is the main reason generated code can use the same language features as user-written Workflow code: generated AST goes back through the ordinary compiler checks before backend emission.

The emitters preserve the invariant:

- `WfEmitter_Expression.cpp` and `WfCpp_Expression.cpp` dispatch virtual expressions through `expandedExpression`.
- Statement and declaration emitters similarly use expanded statements and declarations.
- Raw coroutine and state-machine-only statements reaching backend generation are internal errors.

## Bind Lowering

`bind` is parsed as `WfBindExpression`, a `WfVirtualCseExpression` with an original `expression` and generated `expandedExpression`. `observe` syntax is parsed as `WfObserveExpression`, with `WfObserveType::SimpleObserve` for `parent.observe(...)` and `WfObserveType::ExtendedObserve` for `parent.observe as name(...)`.

### Validation And Scope Preparation

Before expansion:

- `ValidateStructureExpressionVisitor::Visit(WfBindExpression*)` rejects nested `bind`.
- `ValidateStructureExpressionVisitor::Visit(WfObserveExpression*)` requires `observe` to appear under `bind`, rejects nested observe in observe-event expressions, checks simple observe shapes, and requires extended observe to have at least one event.
- `ValidateStructureExpressionVisitor::Visit(WfAttachEventExpression*)` and `Visit(WfDetachEventExpression*)` reject manual attach/detach inside `bind`.
- `BuildScopeForExpression::Visit(WfObserveExpression*)` creates a local scope for extended observe and adds the alias symbol.
- `ValidateSemanticExpressionVisitor::Visit(WfBindExpression*)` validates the bound expression and returns readonly `Ptr<IValueSubscription>`.

The runtime interface is `IValueSubscription` in `Import/VlppReflection.h`. It exposes `ValueChanged`, `Open`, `Update`, and `Close`.

### BindContext

`ExpandBindExpression` in `WfAnalyzer_ExpandBindExpression.cpp` begins by running `CreateBindContextVisitor` over the already-resolved original expression. The visitor builds `BindContext`, which contains:

- `observeParents`: observed expression to parent expression that must be cached.
- `observeEvents`: observed expression to reflected events that must be attached.
- `orderedObserves`: observes in discovery order.
- `cachedExprs`: parent expressions that become generated `<bind-cache>N` fields.
- `renames`: reference expression to expression substituted during dependency discovery and expression rewrite.
- `exprAffects` and `exprCauses`: expression-level dependency graph.
- `observeAffects` and `observeCauses`: observe-to-observe dependency graph.

`CreateBindContextVisitor::Visit(WfMemberExpression*)` detects implicit observable property access by checking `manager->expressionResolvings[node].propertyInfo`. If the property has `GetValueChangedEvent()` or an event named `<PropertyName>Changed`, the member expression becomes an observed expression and its parent is cached.

`CreateBindContextVisitor::Visit(WfObserveExpression*)` records explicit observes, caches their parent, visits the observed value expression, and records each resolved event expression.

`renames` is an expression substitution mechanism, not closure capture. It resolves references to `let` variables and extended-observe aliases back to their defining expressions, so dependency discovery can see through source-local names.

### Dependency Propagation

`CreateBindContextVisitor::DirectDepend(expr, depended)` propagates all observes that can change `depended` into the causes of `expr`.

`CreateBindContextVisitor::ObservableDepend(expr, parent)` records the observe, records its parent, ensures the parent is cached, and builds observe-to-observe edges when the parent itself depends on earlier observes.

The resulting graph drives efficient callback generation. When one event fires, the generated callback can compute the transitive downstream observes that depend on the changed observe, detach only those affected handlers, recompute only the affected cached parents, reattach handlers, and then call the activator.

### Generated Subscription

`ExpandBindExpression` replaces the bind expression with a generated `WfNewInterfaceExpression` typed as `Ptr<IValueSubscription>`.

The generated object contains:

- `var <bind-cache>N : ParentType = default;` for each cached parent expression.
- `var <bind-handler>I_J : EventHandler^ = null;` for each observed event.
- `var <bind-opened> : bool = false;`.
- `var <bind-closed> : bool = false;`.
- `func <bind-activator>()`.
- `func <bind-callback>I_J(...)` for each observed event.
- overrides for `Open`, `Update`, and `Close`.

`CreateWritableVariable` and `CreateDefaultValue` construct generated fields and initial values. `CreateDefaultValue` is also used by coroutine and state-machine lowering, so generated reset behavior is shared.

### Expression And Control-Flow Rewrite

`ExpandObserveExpressionVisitor` rewrites the original expression:

- Cached expressions become references to `<bind-cache>N`.
- `let` variables whose values are cached are removed from the generated `let`.
- Simple observe becomes member access on the rewritten parent.
- Extended observe becomes the rewritten inner expression.

The activator evaluates the rewritten original expression into `<bind-activator-result>` and raises `ValueChanged(<bind-activator-result>)`.

`CreateBindCacheAssignStatement` emits cache assignment using `WfBinaryOperator::FailedThen`, conceptually:

```workflow
<bind-cache>N = expanded-parent-expression ?? default(parent-type);
```

This keeps subscription maintenance resilient to failed parent evaluation by resetting the cache to the default type value.

`CreateBindOpenFunction` generates:

1. set `<bind-opened>` if it was false;
2. assign caches in dependency order;
3. attach all observed event handlers;
4. return true, or return false if already opened.

`CreateBindUpdateFunction` calls `<bind-activator>()` only when opened and not closed.

`CreateBindCloseFunction` sets `<bind-closed>`, detaches non-null handlers, resets caches and handler fields to defaults, and returns whether it actually closed.

Each generated callback:

1. starts from the observe whose event fired;
2. walks `observeAffects` to find affected downstream observes;
3. detaches affected downstream handlers in reverse order;
4. recomputes each affected parent cache once;
5. reattaches affected handlers;
6. calls `<bind-activator>()`.

`Test/Generated/Workflow64/Parsing.Codegen.BindLet.txt` demonstrates this printed shape: callbacks detach handlers under old cached child objects, recompute child caches such as `<bind-cache>5`, reattach, and invoke the activator.

### Variables And Captures

Bind has three variable mechanisms:

- observed parent expressions become generated cache fields;
- `let` variables and extended-observe aliases are handled through `BindContext::renames` and may disappear from the generated expression if their value is cached;
- outer function variables are captured by ordinary anonymous-interface capture machinery.

`BuildScopeForExpression::Visit(WfNewInterfaceExpression*)` creates a `WfLexicalCapture` for the generated interface implementation. During semantic resolution, references from generated subscription methods to outer function locals are recorded in `manager->lambdaCaptures`. Bytecode metadata names those as `<captured>x`, while bind-owned state uses names such as `<bind-cache>0` and `<bind-handler>0_0`.

`ExpandBindExpression` therefore does not convert surrounding locals into bind-specific fields. It relies on the same closure model used by user-written `new interface` expressions.

## Co-Provider To Raw Coroutine To Trivial Workflow

Coroutine-related syntax is lowered in two distinct layers:

1. Co-provider syntax is lowered to raw coroutine syntax. `WfCoProviderStatement` is rewritten to a provider `Create` or `CreateAndRun` call that receives an anonymous function returning `WfNewCoroutineExpression`.
2. Raw coroutine syntax is lowered to trivial Workflow code. `WfNewCoroutineExpression` is compiled into a generated `WfNewInterfaceExpression` implementing `ICoroutine`, with explicit fields, state integers, dispatch loops, and ordinary statements.

This layering is important for maintenance. Co-provider lowering does not directly build a coroutine flow chart. It generates a raw coroutine body containing `WfCoPauseStatement`, provider method calls, result checks, and normal statements. Then the raw coroutine expander owns all control-flow splitting, variable persistence, pause/resume handling, exception routing, and final `ICoroutine` object generation.

### Parsed Forms

The compiler does not have a hardcoded `$yield` node. `Source/Parser/Syntax/Lexer.txt` defines `COROUTINE_OPERATOR` as `$` followed by an uppercase name. Operators such as `$Yield`, `$Await`, and `$Join` all use the same internal shape.

The parser represents coroutine-related syntax as:

- `WfCoProviderStatement` for provider bodies such as `$ { ... }` or `$Async { ... }`.
- `WfCoOperatorStatement` for statement operators such as `$Yield i;`.
- `WfCoOperatorStatement` with `varName` for result-producing operators such as `var x = $Await async;`.
- `WfCoOperatorExpression` for context expressions such as `$.Context`.
- `WfNewCoroutineExpression` for raw `$coroutine{}`.

`WfCoProviderStatement`, `WfCoOperatorStatement`, and `WfCoOperatorExpression` are the high-level provider layer. `WfNewCoroutineExpression` is the raw coroutine layer.

### Semantic Resolution Before Lowering

`BuildScopeForStatement::Visit(WfCoProviderStatement*)` creates provider-scope symbols:

- `$PROVIDER`: selected provider type.
- `$IMPL`: provider implementation type passed to the generated coroutine creator.

`ValidateSemanticStatementVisitor::Visit(WfCoProviderStatement*)` resolves the provider:

- unnamed `$ { ... }` starts from the containing function return type and searches for `<ReturnTypeDescriptorName>Coroutine`, including base type descriptors;
- named `$Async { ... }` strips `$`, resolves `Async`, then searches for `AsyncCoroutine` or accepts `AsyncCoroutine` directly;
- void-like provider functions use static `CreateAndRun`;
- value-returning provider functions use static `Create`;
- the selected creator must accept `Ptr<IValueFunctionProxy<Ptr<ICoroutine>, ImplType>>`;
- the selected method is saved in `manager->coProviderResolvings`.

`ValidateSemanticStatementVisitor::Visit(WfCoOperatorStatement*)` resolves each operator against `$PROVIDER` and `$IMPL`:

- `$Yield` becomes operator name `Yield`;
- the provider is searched for static `YieldAndRead`;
- if there is no result variable, static `YieldAndPause` is also considered;
- candidates must take `$IMPL` as the first parameter;
- overload selection runs with a dummy `$IMPL` first argument followed by source arguments;
- the selected provider method is saved in `manager->coOperatorResolvings`.

For result-producing operators such as `var x = $Await async;`, semantic validation also finds a static `CastResult(Value)` helper from argument types. The helper's return type becomes the lexical type of `x`, and the helper is saved in `manager->coCastResultResolvings`.

Return statements inside provider bodies are provider-specific. `ValidateSemanticStatementVisitor::Visit(WfReturnStatement*)` resolves provider static `ReturnAndExit`, requiring `$IMPL` as the first parameter, and saves the method in `manager->coOperatorResolvings`.

`WfCoOperatorExpression`, the expression form used for `$.Context`, cannot be expanded at the outer provider level because it needs the generated `<co-impl>` variable. Its expansion is delayed until the generated raw coroutine body is being copied and validated under the scope containing `<co-impl>`.

### Layer 1: Co-Provider To Raw Coroutine

`ExpandCoProviderStatement` in `WfAnalyzer_ExpandStatement.cpp` rewrites `WfCoProviderStatement` into ordinary AST that calls the provider creator with a generated function. Conceptually, the generated shape is:

```workflow
Provider::Create-or-CreateAndRun(
    func(<co-impl> : ImplType) : Coroutine^
    {
        return $coroutine(<co-result>) { rewritten provider body };
    }
);
```

Internally this first layer is represented by:

- a `WfFunctionExpression`;
- an anonymous `WfFunctionDeclaration`;
- a generated `WfFunctionArgument` named `<co-impl>`, typed as the resolved `$IMPL`;
- a `WfNewCoroutineExpression` named `<co-result>`;
- a `WfCallExpression` to the selected provider `Create` or `CreateAndRun` method;
- a `WfBlockStatement` stored in `WfCoProviderStatement::expandedStatement`.

The important result is that the high-level provider statement has become a normal provider method call whose callback returns a raw coroutine. From this point on, the provider body is treated as a coroutine body.

`ExpandCoProviderStatementVisitor` copies the source provider body into the generated `WfNewCoroutineExpression` and rewrites provider-only statements during the copy.

For `$Yield i;`, the rewritten raw coroutine body contains:

- a `WfCoPauseStatement`;
- an inner pause block containing a `WfCallExpression` to the resolved provider method, such as `EnumerableCoroutine::YieldAndPause(<co-impl>, i)`;
- ordinary post-pause checks against `<co-result>`.

Conceptually:

```workflow
$pause
{
    EnumerableCoroutine::YieldAndPause(<co-impl>, i);
}
if (<co-result> is not null)
{
    if (<co-result>.Failure is not null) raise <co-result>.Failure;
}
```

For `var x = $Await async;`, the raw coroutine body is also a pause around the resolved provider call. After resume, it checks `<co-result>.Failure`, reads `<co-result>.Result`, converts it through the resolved `CastResult(Value)` method, and emits a normal `WfVariableStatement` for `x`.

For `return expr;`, the provider body is rewritten to call `Provider::ReturnAndExit(<co-impl>, ...)` and then emit a final `return`. If the provider result type exposes `StoreResult`, the source return expression is wrapped by that helper before being passed to `ReturnAndExit`.

After this first layer, the remaining provider-specific concepts are expressed through ordinary calls and through `WfCoPauseStatement`. The compiler no longer needs a special co-provider control-flow model.

### Layer 2: Raw Coroutine To Flow Chart

`ExpandNewCoroutineExpression` in `WfAnalyzer_ExpandNewCoroutineExpression.cpp` handles raw `WfNewCoroutineExpression`, regardless of whether it came from user-written `$coroutine{}` or from `ExpandCoProviderStatement`.

The expander builds a flow-chart model:

- `FlowChart` owns all nodes, `headNode`, `lastNode`, and temporary exception variables.
- `FlowChartNode` holds copied statements, conditional branches, default destination, exception destination, pause destination, action, exception variable, and an `embedInBranch` optimization flag.
- `FlowChartBranch` holds an optional condition and a destination.
- `GenerateFlowChartStatementVisitor::ScopeContext` models structured exits for functions, loops, labeled blocks, and try/finally.

The flow-chart pipeline is:

1. `FindCoroutineAwaredStatements` marks statements that affect suspension or control-flow splitting: virtual statements, coroutine statements, `break`, `continue`, `return`, `goto`, `if`, `while`, `try`, and blocks containing aware statements.
2. `FindCoroutineAwaredVariables` finds local variables inside aware regions.
3. `FindCoroutineReferenceRenaming` assigns generated field names for locals and temporary control variables that must survive.
4. `GenerateFlowChart` copies statements into flow-chart nodes and rewrites references to renamed symbols.
5. `RemoveUnnecessaryNodes` merges trivial nodes, removes the default destination after pause sentinels, and marks simple branch targets for embedding.
6. `ExpandNewCoroutineExpression` emits a generated `WfNewInterfaceExpression` implementing `ICoroutine`.

This second layer is where raw coroutine code stops being structured source code and becomes explicit resumable control flow.

### Variable Conversion In Raw Coroutine Lowering

Coroutine variable conversion is symbol-based, not text-based. The renaming map is keyed by `WfLexicalSymbol*`, avoiding collisions when generated or source variables reuse textual names in nested scopes.

Naming rules include:

- source local `x` can become `<coN>x`;
- generated temporaries such as `<for-begin>i` can become `<coN-for-begin>i`;
- catch variables, `if` expression variables, and try/finally temporary exception variables can also be renamed.

When a renamed `WfVariableStatement` appears in an aware region, `GenerateFlowChartStatementVisitor::Visit(WfVariableStatement*)` replaces the declaration with an assignment to the generated field. `ExpandNewCoroutineExpression` then emits one generated field declaration for each renamed symbol, using the original symbol type and `CreateDefaultValue`.

Variables not in the renaming map remain ordinary locals. Outer function variables are handled by normal function/lambda capture analysis around the generated function expression, not by coroutine field conversion.

For a co-provider body, this means variables introduced by source code and variables introduced by co-operator rewriting both participate in the same raw coroutine persistence algorithm. For example, a variable produced by `var x = $Await ...` is initially emitted as a normal `WfVariableStatement` after the pause. If later coroutine states need `x`, `FindCoroutineReferenceRenaming` can promote it to a generated coroutine field.

### Pause Conversion In Raw Coroutine Lowering

`GenerateFlowChartStatementVisitor::Visit(WfCoPauseStatement*)` turns a raw coroutine pause into three flow-chart nodes:

1. a node with `action = SetPause`;
2. a sentinel node containing an empty `WfCoPauseStatement`;
3. a resume-continuation node stored in `pauseDestination`.

If the pause has an inner statement, such as `EnumerableCoroutine::YieldAndPause(<co-impl>, i)`, the inner statement is copied into the `SetPause` node before the sentinel pause.

`ExpandFlowChartNode` converts a `SetPause` node into ordinary code that:

1. sets coroutine status to waiting;
2. records `<co-state-before-pause>`;
3. sets `<co-state>` to the state after the pause;
4. runs the provider operation or other inner pause statements;
5. returns from `Resume`.

On the next `Resume`, the generated coroutine:

- verifies it is waiting;
- sets status to executing;
- examines the resume input when `<co-state-before-pause> != -1`;
- clears the pre-pause state if there is no failure;
- routes failure to a catch node or raises it;
- dispatches through a generated loop using `<co-state>`.

In a co-provider-generated coroutine, the resume input is named `<co-result>`. That is why the first-layer co-provider rewrite emits checks against `<co-result>` after every operator pause: once the second layer has split the coroutine, those checks execute only after the provider runtime resumes the coroutine with a `CoroutineResult`.

### Layer 3: Flow Chart To Trivial Workflow Code

The final output of raw coroutine lowering is generated ordinary Workflow AST, not a special backend coroutine instruction.

`ExpandNewCoroutineExpression` emits a generated `WfNewInterfaceExpression` implementing `ICoroutine`. The generated object contains:

- generated fields for renamed locals and temporaries;
- `<co-state>`;
- `<co-state-before-pause>`;
- auto properties for `Failure` and `Status`;
- override `Resume(<raise-exception>, <coroutine-output>)`.

`Resume` is trivial Workflow control flow:

- it uses `if` checks and loops to dispatch on `<co-state>`;
- normal flow-chart edges assign `<co-state>` and continue;
- branch edges emit ordinary `if` statements, sometimes embedding small destination nodes;
- `break`, `continue`, `goto`, and `return` are represented as jumps to generated flow-chart destinations;
- `InlineScopeExitCode` inserts pending `finally` code while leaving scopes;
- nodes with `exceptionDestination` are wrapped in generated try/catch code that assigns the caught exception to a renamed exception field, sets `<co-state>` to the catch node, and continues.

This is the point where both user-written raw `$coroutine{}` and co-provider-generated raw coroutine bodies become the same kind of generated trivial Workflow code. Bytecode and C++ emitters then see ordinary interface implementation, fields, methods, properties, statements, and expressions through `expandedExpression` or `expandedStatement`.

## State Machine Lowering

State machines are context-sensitive declarations. The parser represents them as `WfStateMachineDeclaration`, with:

- `WfStateInput` for `$state_input`;
- `WfStateDeclaration` for `$state`;
- `WfStateSwitchStatement` for `$switch`;
- `WfStateInvokeStatement` for `$goto_state` and `$push_state`.

### Preparation Before Expansion

State machines participate in several `Rebuild` phases before `ExpandStateMachine` runs:

- `BuildGlobalNameFromModules::BuildClassMemberVisitor::Visit(WfStateMachineDeclaration*)` synthesizes class member placeholders for input methods, input argument fields, state argument fields, and `<state>CreateCoroutine`.
- `BuildScopeForDeclarationVisitor::Visit(WfStateMachineDeclaration*)` creates lexical symbols for state inputs, states, state arguments, switch-case arguments, and state bodies.
- `CompleteScopeForClassMemberVisitor::Visit(WfStateMachineDeclaration*)` fills input method signatures, generated field types, and `<state>CreateCoroutine(<state>startState:int):void`.
- `ValidateSemanticDeclarationVisitor::Visit(WfClassDeclaration*)` requires a class containing a state machine to inherit from `system::StateMachine`.
- `ValidateSemanticDeclarationVisitor::Visit(WfStateMachineDeclaration*)` requires a default state and validates each state body.
- `ValidateSemanticStatementVisitor::Visit(WfStateSwitchStatement*)` resolves switch cases to generated input methods and assigns case argument symbol types from input method parameters.
- `ValidateSemanticStatementVisitor::Visit(WfStateInvokeStatement*)` resolves target states and validates invocation arguments against target state argument types.

`WfStateMachineInfo` stores `createCoroutineMethod`, `inputIds`, and `stateIds`. `inputIds` and `stateIds` are filled by `ExpandStateMachine`.

### Generated Class Members

`ExpandStateMachine` turns a state-machine declaration into ordinary class members:

- private fields named like `<stateip-INPUT>ARG` for input arguments;
- private fields named like `<statesp-STATE>ARG` for state arguments;
- generated input methods;
- generated `<state>CreateCoroutine`.

Generated fields are added to `WfStateMachineDeclaration::expandedDeclarations` as `WfVariableDeclaration` nodes with `@cpp:Private`, and connected back to reflected field metadata through `manager->declarationMemberInfos`.

This is different from coroutine local conversion. Raw coroutine lowering hoists locals only when the flow chart proves they must survive a pause. State-machine lowering creates class fields up front for semantic state because input arguments and state invocation arguments must survive across input method calls, state transitions, and pushed child states.

### Generated Input Methods

For each `$state_input`, `ExpandStateMachine` emits a normal class method using the existing `WfClassMethod` from `stateInputMethods`.

Each input method:

1. initializes the state machine on first call by setting `stateMachineInitialized`, creating the default-state coroutine with `<state>CreateCoroutine(0)`, and calling `ResumeStateMachine`;
2. stores the input id in `stateMachineInput`;
3. copies method arguments into `<stateip-INPUT>ARG` fields;
4. calls `ResumeStateMachine` again to deliver the input.

The generated method is ordinary AST: `WfFunctionDeclaration`, `WfIfStatement`, `WfBinaryExpression`, `WfMemberExpression`, `WfCallExpression`, and `WfExpressionStatement`.

### Generated State Coroutine

`ExpandStateMachine` emits:

```workflow
func <state>CreateCoroutine(<state>startState : int) : void
```

The method creates a `WfNewCoroutineExpression` and assigns it to `stateMachineCoroutine`. It saves the previous coroutine in `<state>previousCoroutine`, creates a coroutine using `<state>stateMachineObject = this`, and restores the previous coroutine in a `finally` block.

The generated coroutine body contains:

- a protected block labeled `<state-label>OUT_OF_STATE_MACHINE`;
- a `finally` block restoring `stateMachineCoroutine`;
- local `<state>state = <state>startState`;
- `while(true)` labeled with `<state-label>OUT_OF_CURRENT_STATE`;
- local `<state>currentState = <state>state`;
- assignment `<state>state = -1`;
- a `WfSwitchStatement` over `<state>currentState`;
- one switch case for each declared state.

Each state case copies `<statesp-STATE>ARG` fields into local state arguments, copies the original state body through `ExpandStateMachineStatementVisitor`, and emits a goto to leave the state machine as default fall-through.

This generated code intentionally uses the same ordinary control-flow constructs understood by coroutine lowering: labels, gotos, loops, switches, try/finally, and `WfCoPauseStatement`.

### State Switch Lowering

`ExpandStateMachineStatementVisitor::Visit(WfStateSwitchStatement*)` rewrites `$switch` into a block containing:

- an `if` that pauses when `stateMachineInput == -1`;
- a `WfSwitchStatement` over `stateMachineInput`.

For each explicit case:

- the case expression is the numeric id from `smInfo->inputIds`;
- the generated case clears `stateMachineInput` to `-1`;
- input arguments are copied from `<stateip-INPUT>ARG` fields into local case variables;
- the original case body is copied.

If the switch type is `WfStateSwitchType::Default`, the expander also creates raising cases for every input not explicitly listed.

Default branch behavior depends on `WfStateSwitchType`:

- `Default`: no default branch; missing input cases were generated as raising cases.
- `Pass`: no default branch; unhandled input remains available.
- `PassAndReturn`: `goto <state-label>OUT_OF_STATE_MACHINE`.
- `Ignore`: clear `stateMachineInput`.
- `IgnoreAndReturn`: clear `stateMachineInput`, then leave the state machine.

The generated `WfSwitchStatement` is itself another context-sensitive virtual statement and is later expanded by the normal switch expander.

### State Invoke Lowering

`ExpandStateMachineStatementVisitor::Visit(WfStateInvokeStatement*)` handles `Goto` and `Push`.

Both forms first copy invocation arguments into generated state argument fields:

```workflow
<state>stateMachineObject.<statesp-STATE>ARG = copiedArgument;
```

`WfStateInvokeType::Goto` then assigns the target state id to `<state>state` and jumps to `<state-label>OUT_OF_CURRENT_STATE`. This is a domain-state transition inside the same generated coroutine.

`WfStateInvokeType::Push` calls `<state>CreateCoroutine(TargetStateId)` and emits `WfCoPauseStatement`. This replaces `stateMachineCoroutine` with a child coroutine and pauses the current coroutine. The stack-like behavior is encoded by the generated `previousCoroutine` local plus the `finally` block that restores `stateMachineCoroutine`.

`system::StateMachine::ResumeStateMachine` in `Source/Library/WfLibraryPredefined.cpp` drives this protocol. It detects whether a resume pushed a child coroutine, whether a child finished and restored the parent coroutine, and whether a child failure should be delivered back to the parent as a `CoroutineResult`.

### Shared And Different From Coroutine Lowering

State-machine lowering shares these constructions with raw coroutine lowering:

- it generates `WfNewCoroutineExpression`;
- it generates `WfCoPauseStatement`;
- it uses ordinary Workflow control-flow AST;
- it uses `CreateDefaultValue`;
- it relies on the same virtual-node recheck pipeline;
- it ultimately produces an `ICoroutine` implementation with `<co-state>`, `<co-state-before-pause>`, `Failure`, `Status`, and `Resume`;
- locals in the generated coroutine body can be renamed into coroutine fields by `FindCoroutineReferenceRenaming`.

The important differences are:

- raw coroutine lowering starts from a statement body and directly builds a `FlowChart`; state-machine lowering starts from a class declaration and first generates fields, methods, and a generated coroutine body;
- raw coroutine state ids are implementation states in `FlowChartNode` order; state-machine ids in `smInfo->stateIds` are domain states, and coroutine lowering later adds another implementation-state layer;
- raw coroutine persistence is discovered by `FindCoroutineAwaredVariables`; state-machine input and state arguments are stored in generated class fields before coroutine lowering;
- raw coroutine resumption receives `CoroutineResult` directly from a `Resume` caller; state-machine resumption goes through generated input methods and `StateMachine::ResumeStateMachine`;
- state-machine switch policies have no raw coroutine equivalent.

## Backend Contract And Maintenance Rules

The backend contract is simple: generated code must be ordinary Workflow AST by the time bytecode or C++ generation needs it.

For maintenance work:

- decide whether new syntax is context-free or context-sensitive;
- context-free syntax can expand before scopes;
- context-sensitive syntax must validate original nodes first, generate `expanded*`, and re-run desugar, scope building, scope checks, and semantic validation on generated AST;
- generated AST must be compatible with `BuildScope`, `CompleteScope`, semantic validation, bytecode emission, and C++ emission;
- new semantic side tables must be cleared by `WfLexicalScopeManager::Clear`;
- generated nodes should keep source code ranges from the construct that caused them;
- logging and callbacks should not be required for compiler correctness;
- raw coroutine and state-machine statements should remain frontend-only.

The generated samples under `Test/Generated/Workflow32` and `Test/Generated/Workflow64` are useful for auditing the printed desugared shape. For example, bind samples show generated subscriptions, co-provider samples show generated coroutine state fields, and state-machine samples show generated input methods plus coroutine-backed state loops.
