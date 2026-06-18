# !!!KNOWLEDGE BASE!!!

# DESIGN REQUEST

this is a preparation of api explanation. workflow offers a lot of functions in the header files to create workflow AST nodes, they are supposed to be used when user need to generate workflow code. we recommended users to generate workflow AST instead of workflow in text form. consolidate all ast creating functions in header files, as well as a list of all ast creating functions in cpp files which are not externed in any header files, explain their usage, with a list of duplicated functions, and an research report of if any code could be improved to reduce duplication

# INSIGHT

## Scope

This topic belongs to the `Workflow` project. The nearest existing KB entry is `Compiler Driver and C++ Generation` in `.github/KnowledgeBase/Index_Workflow.md`, but that entry covers parsing/analyzing/generating C++ rather than the lower-level AST-construction helpers. A future document should likely be an API explanation under `### Choosing APIs`, because the user-facing problem is how to choose helper APIs when generating Workflow code as AST instead of text.

The core AST classes are generated in `Source/Parser/Generated/WorkflowAst.h`. The source repeatedly builds AST by `Ptr(new WfXxx)` and by filling public fields. That is the fundamental construction model; helper functions only wrap common shapes.

## Header-Level AST-Producing APIs

### Parser Text-to-AST APIs

`Source/Parser/WfExpression.h` declares text parsing APIs:

- `ParseType` returns a `WfType` AST.
- `ParseExpression` returns a `WfExpression` AST.
- `ParseStatement` returns a `WfStatement` AST.
- `ParseCoProviderStatement` returns a `WfCoProviderStatement` AST.
- `ParseDeclaration` returns a `WfDeclaration` AST.
- `ParseModule` returns a `WfModule` AST.

Their implementation in `Source/Parser/WfExpression.cpp` tokenizes with `Parser::Tokenize`, calls the generated parser entry such as `Parser::Parse_Type`, then runs `WorkflowUnescapeVisitor` so string and format-string AST nodes contain unescaped content. These APIs are for consuming existing Workflow text. They are not the preferred generator API when code can be emitted directly as AST, because they intentionally start from text.

`Source/Parser/WfExpression.h` also declares `WfPrint` overloads for attributes, types, expressions, statements, declarations, and modules. They do not create AST, but they are the matching inspection/serialization API after AST generation.

### Reflection-to-AST APIs

`Source/Analyzer/WfAnalyzer.h` declares these AST-producing conversion helpers, implemented in `Source/Analyzer/WfAnalyzer_TypeInfo.cpp`:

- `GetExpressionFromTypeDescriptor` builds a globally qualified type-name expression as `WfTopQualifiedExpression` followed by `WfChildExpression` nodes.
- `GetTypeFromTypeDescriptor` builds a globally qualified type-name type as `WfTopQualifiedType` followed by `WfChildType` nodes.
- `GetTypeFromTypeInfo` converts reflection type information to Workflow type syntax. It handles raw pointers as `WfRawPointerType`, shared pointers as `WfSharedPointerType` unless the element is generic, nullable values as `WfNullableType`, plain descriptors through `GetTypeFromTypeDescriptor`, and well-known generic collection/function descriptors as `WfEnumerableType`, `WfMapType`, `WfObservableListType`, or `WfFunctionType`.

Use these when generated AST needs a Workflow type for an existing C++ reflected type. The implementation is descriptor-aware; it is safer than manually spelling namespace fragments when the source of truth is reflection metadata.

### AST Copy APIs

`Source/Analyzer/WfAnalyzer.h` declares `CopyType`, `CopyExpression`, `CopyStatement`, `CopyDeclaration`, and `CopyModule`, each with `Ptr<T>` and raw-pointer overloads. They are implemented in `Source/Analyzer/WfAnalyzer_ExpandBindExpression.cpp` through `CopyWithExpandVirtualVisitor`.

The important usage distinction is the `expandVirtualExprStat` flag:

- For expressions, statements, declarations, and modules, `true` copies `expandedExpression`, `expandedStatement`, and expanded declaration lists from virtual AST nodes when available.
- `false` preserves the original virtual node shape.
- `CopyType` always uses the non-expanding path because Workflow types do not have the same virtual-expansion mechanism.

Use these when generated AST needs to preserve user-written fragments or copy compiler-generated fragments without aliasing the original nodes. The implementation also expands nested declaration lists inside `WfNamespaceDeclaration`, `WfClassDeclaration`, and `WfNewInterfaceExpression`.

### Default-Value Expression API

`CreateDefaultValue` is declared in `Source/Analyzer/WfAnalyzer.h` and implemented in `Source/Analyzer/WfAnalyzer_ExpandBindExpression.cpp`. It creates an expression that represents a default value for a reflection `ITypeInfo`:

- enum defaults become integer `0`, cast through `U8`, then cast to the enum type;
- strings become `WfStringExpression`;
- non-serializable structs become `{}` cast to the struct type;
- booleans become `false`;
- floating and integer primitives become `0` inferred as the target type;
- other reference-like values become `null` inferred as the target type.

Proof of intended sharing: `CreateDefaultValue` is used by bind cache reset in `Source/Analyzer/WfAnalyzer_ExpandBindExpression.cpp`, coroutine field initialization in `Source/Analyzer/WfAnalyzer_ExpandNewCoroutineExpression.cpp`, and state-machine generated field initialization in `Source/Analyzer/WfAnalyzer_ExpandStateMachine.cpp`.

### RPC Module Generation APIs

`Source/Analyzer/WfAnalyzer.h` declares AST-producing RPC APIs:

- `CopyAndClearRpcMetadata` clones a module with `CopyModule(module, false)` and removes declarations that already exist as reflected types. It is implemented in `Source/Analyzer/Rpc/WfAnalyzer_ValidateRPC.cpp` by `ClearExistingTypesVisitor`.
- `GenerateModuleRpc` creates a `WfModule` named `RpcMetadata` containing RPC ids, helper functions, generated ops interfaces, object ops, listeners, wrapper interfaces, wrapper factories, and type-id dispatch helpers. It is implemented in `Source/Analyzer/Rpc/WfAnalyzer_GenerateRpc.cpp`.
- `GenerateModuleRpcJson` creates the JSON-enabled RPC wrapper/serializer module. It is implemented in `Source/Analyzer/Rpc/WfAnalyzer_GenerateRpc_JsonSerialization.cpp`.

These are high-level module generators, not general AST factory helpers. Callers should use them after RPC validation has populated `WfLexicalScopeManager::rpcMetadata`.

The RPC implementation files are grouped under `Source/Analyzer/Rpc`. `Source/Analyzer/Rpc/WfAnalyzer_RpcAstHelpers.h` and `Source/Analyzer/Rpc/WfAnalyzer_GenerateRpc.h` are private implementation headers shared by RPC generator `.cpp` files. They are not user-facing AST-building header APIs. In particular, `WfAnalyzer_RpcAstHelpers.h` should not be counted in this section's list of header-level AST-producing APIs; it exists only to de-duplicate RPC implementation helpers.

## Implementation-Only AST-Producing Helpers

The following lists focus on functions that return `Ptr<Wf...>` and create or assemble AST nodes, excluding helpers that merely query metadata or return non-AST values. Private RPC helper declarations may live in headers under `Source/Analyzer/Rpc`, but they remain implementation helpers rather than public generator APIs.

### `Source/Analyzer/Rpc/WfAnalyzer_RpcAstHelpers.(h|cpp)`

This private helper layer contains the shared AST factories used by flat RPC generation, JSON RPC generation, and RPC metadata validation. Non-template functions are declared with explicit `extern`; `CreateTypeFromCpp<T>` and `CreateCall` stay in the header because their template bodies must be visible.

Basic type factories:

- `SplitTypeFullName`: splits `A::B` names for type and expression factory helpers.
- `CopyType(WfType*)`: RPC-local wrapper around the public `vl::workflow::analyzer::CopyType(type)` helper.
- `CreatePredefinedType`: creates `WfPredefinedType`.
- `CreateQualifiedType`: splits `A::B` and creates `WfReferenceType` plus `WfChildType`.
- `CreateTopQualifiedType`: splits `A::B` and creates `WfTopQualifiedType` plus `WfChildType`.
- `CreateSharedType`: wraps `CreateQualifiedType` in `WfSharedPointerType`.
- `CreateRawType`: wraps `CreateQualifiedType` in `WfRawPointerType`.
- `CreateNullableType`: wraps `CreateQualifiedType` in `WfNullableType`.
- `NormalizeRpcGeneratedType`: normalizes copied/generated RPC type AST by replacing `system::Boolean`, `system::String`, `system::Void`, and platform-sized integer names with predefined Workflow types, converting `WfTopQualifiedType` roots to `WfReferenceType`, and recursively normalizing compound types.
- `CreateTypeFromCpp<T>`: converts C++ type info to Workflow type AST through `GetTypeFromTypeInfo`, then normalizes it for generated RPC modules.

Basic expression factories:

- `CreateNull`: creates null `WfLiteralExpression`.
- `CreateIsNull`: creates `WfTypeTestingExpression` with `IsNull`.
- `CreateIsNotNull`: creates `WfTypeTestingExpression` with `IsNotNull`.
- `CreateIsType`: creates `WfTypeTestingExpression` with `IsType`.
- `CreateBool`: creates true/false `WfLiteralExpression`.
- `CreateInt`: creates `WfIntegerExpression`.
- `CreateString`: creates `WfStringExpression`.
- `CreateReference`: creates `WfReferenceExpression`.
- `CreateThis`: creates `WfThisExpression`.
- `CreateQualifiedExpression`: splits `A::B` and creates `WfReferenceExpression` plus `WfChildExpression`.
- `CreateMember`: creates `WfMemberExpression`.
- `CreateBinary`: creates `WfBinaryExpression`.
- `CreateUnary`: creates `WfUnaryExpression`.
- `CreateAssign`: creates assignment by calling `CreateBinary`.
- `CreateIndex`: creates index expression by calling `CreateBinary`.
- `CreateCast`: creates strong `WfTypeCastingExpression`.
- `CreateWeakCast`: creates weak `WfTypeCastingExpression`.
- `CreateInfer`: creates `WfInferExpression`.
- `CreateConstructorArgument`: creates keyed/value `WfConstructorArgument`.
- `CreateConstructor`: creates empty `WfConstructorExpression`.
- `CreateRpcExceptionExpression`: creates an inferred `RpcException` constructor expression with a `message` argument.
- `CreateRpcEventExceptionMapType`: creates the Workflow type for `Dictionary<vint, RpcException>`.
- `CreateNewClass`: creates `WfNewClassExpression`.
- `CreateNewInterface`: creates `WfNewInterfaceExpression`.

Basic statement/declaration factories:

- `CreateExpressionStatement`: wraps an expression in `WfExpressionStatement`.
- `CreateReturn`: creates `WfReturnStatement`.
- `CreateRaise(const WString&)`: creates `WfRaiseExceptionStatement` from a string.
- `CreateRaise(Ptr<WfExpression>)`: creates `WfRaiseExceptionStatement` from an expression.
- `CreateVariableDeclaration`: creates `WfVariableDeclaration`.
- `CreateVariableStatement`: creates `WfVariableStatement` around `CreateVariableDeclaration`.
- `CreateInferredVariableStatement`: creates a variable statement without an explicit type.
- `CreateTry`: creates `WfTryStatement`.
- `CreateTryCatch`: creates a try/catch statement through `CreateTry`.
- `CreateForEach`: creates normal-direction `WfForEachStatement`.
- `CreateWhile`: creates `WfWhileStatement`.
- `CreateBlock`: creates `WfBlockStatement`.
- `AddStatement`: appends a statement to a block.
- `AddRpcMethodExceptionRaise`: appends an RPC method exception check and raise.
- `AddRpcEventExceptionMapSet`: appends a generated assignment into the RPC event exception map.
- `AddRpcEventExceptionRaise`: appends an RPC event exception check and raise.
- `CreateFunctionArgument`: creates `WfFunctionArgument`.
- `CreateFunctionDeclaration`: creates `WfFunctionDeclaration`, fills name, return type, kind, anonymity, and initializes the body with `CreateBlock`.
- `CreateFunctionExpression`: creates `WfFunctionExpression`.
- `CreateClassDeclaration`: creates a class-shaped `WfClassDeclaration`.
- `CreateConstructorDeclaration`: creates `WfConstructorDeclaration` with a block body.
- `AddSwitchCase`: appends a case to an existing `WfSwitchStatement`.
- `CreateCall`: variadic template that creates `WfCallExpression`, sets the function expression, and appends all arguments.

### `Source/Analyzer/Rpc/WfAnalyzer_GenerateRpc.cpp`

This file now keeps RPC-domain builders and larger generated-subtree builders. Common AST factories used across RPC files are supplied by `WfAnalyzer_RpcAstHelpers.(h|cpp)`.

RPC-domain AST helpers:

- `CreateLifecycleHelperCall`: creates a call to a static `system::IRpcLifecycle` helper.
- `CreateRpcBoxExpression`: creates byref/byval boxing calls for shared interface values, otherwise returns the input expression.
- `CreateRpcCachedPropertyInitialValue`: creates initial values for cached properties, using null for byref and copied default/constructor expressions for byval.
- `CreateRpcUnboxExpression`: creates byref/byval unboxing/copy expressions and casts/inferences for RPC returns and parameters.
- `CreateRpcCopyByvalExpression`: creates lifecycle copy-byval calls.
- `CreateRpcConstantReference`: creates a reference to generated `rpctype_`, `rpcmethod_`, or `rpcevent_` constants.
- `CreatePropertyCacheAvailableRead`: creates a reference to the generated cached-property availability field.
- `CreatePropertyCacheValueRead`: creates a reference to the generated cached-property value field.
- `CreateAnonymousLambda`: creates an anonymous void `WfFunctionDeclaration` with copied RPC parameter arguments and the supplied body.
- `CreateRpcOpsFunctionDeclaration`: creates RPC ops method declarations, including standard `ref` argument setup.
- `CreateRpcOpsObjectOps`: creates the dispatcher call for object ops targeting `ref.clientId`.
- `CreateRpcOpsObjectInvoke`: creates object-method invoke calls, with and without an explicit object-ops expression.
- `CreateRpcOpsObjectEventInvoke`: creates object-event broadcast calls.

Larger generated-subtree builders:

- `BuildInvokeMethodBranch`: creates the branch body for generated `IRpcObjectOps::InvokeMethod`.
- `BuildInvokeEventBranch`: creates the branch body for generated `IRpcObjectEventOps::InvokeEvent`.
- `BuildDispatchChain`: creates switch-based method/event id dispatch.
- `GenerateIsInterfaceTypeIdHelper`: creates the `rpcwrapper_IsInterfaceTypeId` declaration.
- `GenerateIsCtorInterfaceTypeIdHelper`: creates the `rpcwrapper_IsCtorInterfaceTypeId` declaration.
- `GenerateObjectOpsFactory`: creates generated object ops implementation.
- `GenerateObjectEventOpsFactory`: creates generated object-event ops implementation.
- `GenerateWrapperInterface`: creates generated `IRpcWrapper_*` interface declarations.
- `GenerateListenerFactory`: creates generated event listener attachers for one interface.
- `GenerateListenerDispatcher`: creates generated listener dispatcher by type id.
- `GenerateRpcOpsMethodImplementation`: creates one generated RPC ops method implementation.
- `GenerateRpcOpsEventImplementation`: creates one generated RPC ops event implementation.
- `GenerateRpcOpsInterface`: creates the generated RPC ops interface declaration.
- `GenerateRpcOpsFactory`: creates the generated RPC ops factory implementation.
- `GenerateWrapperFactory`: creates wrapper factories for one RPC interface.
- `GenerateWrapperDispatcher`: creates generated wrapper dispatch by type id.
- `GenerateWrapperGetTypeId`: creates generated object-to-RPC-type-id helper.

### `Source/Analyzer/Rpc/WfAnalyzer_GenerateRpc_JsonSerialization.cpp`

This file includes the shared RPC helper headers instead of locally forward-declaring helpers from the flat RPC generator. The AST-producing helpers defined only in this file are:

- `CreateJsonArrayItem`: creates an index expression for `array.items[index]`.
- `CreateJsonArrayItemContent`: creates a casted JSON-array item content read.
- `CreateRpcJsonToken`: creates a JSON token constructor with a `value` field.
- `CreateWritableRpcJsonCollectionType`: converts readonly/lazy collection syntax into writable collection syntax for deserialization targets.
- `CreateRpcJsonSerializedArgument`: creates `arguments[index]`.
- `GenerateRpcJsonSerializeEnum`: creates enum serializer function declaration.
- `GenerateRpcJsonDeserializeEnum`: creates enum deserializer function declaration.
- `GenerateRpcJsonSerializeStruct`: creates struct serializer function declaration.
- `GenerateRpcJsonDeserializeStruct`: creates struct deserializer function declaration.
- `GenerateRpcJsonSerialize`: creates the generic `rpcjson_Serialize` declaration.
- `CreateUnknownDeserializeListCase`: creates the unknown-list deserialization branch.
- `CreateUnknownDeserializeMapCase`: creates the unknown-map deserialization branch.
- `GenerateRpcJsonDeserialize`: creates the generic `rpcjson_Deserialize` declaration.
- `GenerateRpcSerializerFactoryJson`: creates the JSON serializer factory declaration.
- `CreateRpcJsonTransferType`: creates the JSON transport type for an RPC parameter/return.
- `CreateRpcJsonTransferExpression(ITypeInfo*, bool, Ptr<WfExpression>, Ptr<WfExpression>)`: creates boxing/copying expression before JSON serialization.
- `CreateRpcJsonTransferExpression(const RpcParamModel&, Ptr<WfExpression>, Ptr<WfExpression>)`: parameter-model overload.
- `CreateJsonDispatchArgument`: creates deserialization/unboxing expression for generated JSON dispatch arguments.
- `BuildInvokeMethodBranchJson`: creates JSON object-method invoke branch.
- `BuildInvokeEventBranchJson`: creates JSON object-event invoke branch.
- `BuildDispatchChainJson`: creates JSON method/event dispatch chain.
- `GenerateObjectOpsFactoryJson`: creates JSON object ops implementation.
- `GenerateObjectEventOpsFactoryJson`: creates JSON object-event ops implementation.
- `GenerateRpcOpsMethodImplementationJson`: creates one JSON RPC ops method implementation.
- `GenerateRpcOpsEventImplementationJson`: creates one JSON RPC ops event implementation.
- `GenerateRpcOpsFactoryJson`: creates JSON RPC ops factory implementation.

The same file also has many `AddRpcJson*` helpers that return `WString` or `void` while appending generated AST to a block. They are not listed as AST-returning factory APIs, but they are part of the same code generation layer.

### `Source/Analyzer/WfAnalyzer_ExpandBindExpression.cpp`

Private AST helpers:

- `CreateReference`: creates `WfReferenceExpression`.
- `CreateWritableVariable`: creates `WfVariableDeclaration` using `GetTypeFromTypeInfo` and `CreateDefaultValue` when no initializer is supplied.
- `CreateWritableVariableStatement`: creates `WfVariableStatement` around `CreateWritableVariable`.
- `AssignNormalMember`: mutates and returns a function declaration with `WfFunctionKind::Normal`.
- `AssignOverrideMember`: mutates and returns a function declaration with `WfFunctionKind::Override`.
- `ExpandObserveEvent`: creates member expressions that refer to cached observe event sources.
- `CreateBindOpenFunction`: creates generated `Open` override for `IValueSubscription`.
- `CreateBindUpdateFunction`: creates generated `Update` override.
- `CreateBindCloseFunction`: creates generated `Close` override.

These helpers are tied to bind lowering. Only `CreateDefaultValue` and the copy helpers are exposed because they are shared outside bind expansion.

### `Source/Analyzer/WfAnalyzer_ExpandNewCoroutineExpression.cpp`

Private AST helpers:

- `GenerateCoroutineInvalidId`: creates a `-1` expression as unary negative over integer `1`.
- `GenerateSetStatus`: creates assignment to generated coroutine status.
- `GenerateSetCoStateBeforePause`: creates assignment from `<co-state>` to `<co-state-before-pause>`.
- `GenerateSetCoStateBeforePauseToInvalid`: creates assignment of invalid id to `<co-state-before-pause>`.
- `GenerateSetCoState`: creates assignment of a flow-chart node id to `<co-state>`.
- `ExpandExceptionDestination`: creates a block that stores caught exceptions, jumps to the catch node, and wraps continuation code.

These are not general-purpose APIs; they encode the flow-chart-to-coroutine lowering protocol.

### `Source/Analyzer/WfAnalyzer_ExpandStateMachine.cpp`

Private AST helpers:

- `GenerateStateMachineInvalidId`: creates a `-1` expression as unary negative over integer `1`.
- `GenerateIngoreInputStatement`: creates an assignment that resets `stateMachineInput` to the invalid id.

The misspelling in `GenerateIngoreInputStatement` is in source. Both helpers are state-machine lowering details.

### `Source/Analyzer/WfAnalyzer_ExpandStatement.cpp`

Private AST helper:

- `GenerateForEachStepStatement`: creates the increment/decrement assignment for range-based `foreach` lowering.

### `Source/Analyzer/Rpc/WfAnalyzer_ValidateRPC.cpp`

Private AST helpers:

- `FindFunctionArgument`: finds an argument declaration while validating or reconstructing reflected RPC declarations.
- `CreateRpcAttribute`: creates `@rpc:*` attributes.
- `CreateRpcAttribute` with value: creates `@rpc:*` attributes with expression values.
- `GenerateEnumDecl`: creates Workflow enum declarations from reflected enum descriptors, sorting items by value and reconstructing flag intersections.
- `GenerateStructDecl`: creates Workflow struct declarations from reflected serializable properties.
- `GenerateInterfaceDecl`: creates Workflow interface declarations from reflected RPC interface descriptors, including constructor type, attributes, bases, methods, properties, and events.
- `EnsureNamespace`: creates or returns nested `WfNamespaceDeclaration` nodes while assembling generated RPC metadata modules.

These helpers generate RPC metadata modules from reflection. They are intentionally closer to metadata reconstruction than ordinary user code generation. Literal-valued RPC attributes now use the shared `CreateString` and `CreateInt` factories from `WfAnalyzer_RpcAstHelpers.h`.

## Duplicated Or Near-Duplicated Helpers

RPC duplications resolved by the RPC helper refactor:

- The common RPC type/expression/statement factories are centralized in `Source/Analyzer/Rpc/WfAnalyzer_RpcAstHelpers.(h|cpp)`.
- `Source/Analyzer/Rpc/WfAnalyzer_GenerateRpc_JsonSerialization.cpp` no longer locally forward-declares helpers from the flat RPC generator.
- `CreateTypeFromCpp<T>`, `CreateWhile`, `CreateRpcExceptionExpression`, and `CreateRpcEventExceptionMapType` are shared by the RPC helper header/source instead of being duplicated or known only to one RPC `.cpp` file.
- RPC metadata reconstruction no longer has local `CreateRpcStringLiteral` and `CreateRpcIntegerLiteral`; it uses the shared `CreateString` and `CreateInt` factories.
- The RPC-local `CopyType(WfType*)` helper delegates to the public analyzer `CopyType(WfType*)` instead of using generated `copy_visitor::AstVisitor` directly.

Remaining clear duplications outside the RPC-local cleanup:

- `CreateReference` exists in both `Source/Analyzer/WfAnalyzer_ExpandBindExpression.cpp` and `Source/Analyzer/Rpc/WfAnalyzer_RpcAstHelpers.cpp`, with the same purpose: allocate `WfReferenceExpression` and fill `name`.
- `GenerateCoroutineInvalidId` and `GenerateStateMachineInvalidId` build the same `-1` expression shape in two files.

Near duplications:

- `CreateQualifiedType`, `CreateTopQualifiedType`, `GetTypeFromTypeDescriptor`, and `GetTypeFromTypeInfo` all construct qualified `WfType` trees, but their roots differ (`WfReferenceType` vs `WfTopQualifiedType`) and their inputs differ (string full name vs reflection descriptor/type info).
- `CreateQualifiedExpression` and `GetExpressionFromTypeDescriptor` both construct qualified expression trees, but one starts from text fragments and one starts from reflection descriptors, and the descriptor version uses `WfTopQualifiedExpression`.
- `CreateVariableDeclaration`, `CreateVariableStatement`, `CreateWritableVariable`, and `CreateWritableVariableStatement` repeat variable construction patterns. The bind-specific versions additionally use `ITypeInfo` and `CreateDefaultValue`, so they should not be blindly merged into the basic RPC factories.
- Flat RPC object ops and JSON RPC object ops share large declaration skeletons: generated lifecycle field, byval return slot dictionary, `InvokeMethod`, `EndInvokeMethod`, `ObjectHold`, event suppression, unknown-id handling, and dispatch-chain shape. JSON adds serialization/deserialization and transport-type conversions, so only the skeleton is duplicated, not the whole behavior.

## Improvement Report

The RPC-scoped low-risk cleanup has been executed:

- RPC implementation files moved from `Source/Analyzer` to `Source/Analyzer/Rpc`.
- `Source/Analyzer/Rpc/WfAnalyzer_RpcAstHelpers.(h|cpp)` now contains shared RPC AST factories and the RPC-only template helpers.
- `Source/Analyzer/Rpc/WfAnalyzer_GenerateRpc.h` now declares RPC-domain helpers shared by the flat and JSON generators, and includes the shared AST helper header.
- JSON RPC generation now relies on those headers instead of local forward declarations.
- RPC metadata reconstruction now relies on shared literal factories for RPC attribute values.

The broader recommendation remains not to expose every private generator helper as public API.

Recommended direction:

- Add a small shared AST builder layer, likely under analyzer or parser support, for common, domain-neutral factories: references, literals, qualified type/expression trees, member access, binary/unary expressions, casts, inference, constructors, calls, new class/interface expressions, expression/return/raise/variable/if/try/foreach/while/block/function/argument declarations, and switch-case append helpers.
- Keep domain-specific builders private: RPC boxing/unboxing, RPC wrapper factories, JSON schema serialization trees, bind subscription generation, coroutine flow-chart generation, and state-machine lowering should not become generic user-facing API.
- Replace `GenerateCoroutineInvalidId` and `GenerateStateMachineInvalidId` with a shared helper such as a negative-integer expression factory. It should preserve the current unary-negative AST shape instead of assuming a negative integer literal is accepted everywhere.
- Consider extracting a reusable skeleton for generated object ops factories only after comparing flat and JSON output carefully. The JSON path has real extra behavior, so a refactor should share only field/method shell construction and common `ObjectHold`/`EndInvokeMethod` pieces, not force serialization-specific branches into generic code.

Risk assessment:

- A broad public builder API would make user code easier to write and would directly support the recommendation to generate `WfModule` AST instead of text.
- Over-exposing RPC-specific helpers would create an unstable API surface around implementation details and make future RPC refactors harder.
- The new private RPC helper header reduced immediate duplication without committing RPC implementation helpers as public API.
- Future shared-builder work should compare generated Workflow and C++ output carefully, as the RPC refactor did, because the generator tests can pass while still revealing unwanted codegen churn through git diff.

# ASKS

# DRAFT

## DRAFT REQUEST
now the improvement plan has been executed and we can continue to create an api explanation dedicated for ast building helper functions:
- focus on what offered in header files
- exclude Source/Analyzer/Rpc/WfAnalyzer_RpcAstHelpers.h
- do not mention file names, mention functions

## IMPROVEMENTS

## API EXPLANATION (Workflow)

Add a new `#### AST Building Helper Functions` entry under `### Choosing APIs`, after `Compiler Driver and C++ Generation` and before `### Design Explanation`.

The index entry should explain that Workflow code generators should prefer producing `WfModule` AST and related AST nodes directly instead of assembling Workflow source text, and should choose the exported helper functions for parsing existing text, converting reflected types, copying AST, creating default-value expressions, generating complete RPC wrapper modules, and printing generated AST.

## DOCUMENT

# AST Building Helper Functions

Workflow generators should prefer building `WfModule` AST and its child nodes directly instead of composing Workflow text and parsing it back. Direct AST generation keeps structured information in C++, avoids source-formatting concerns, and makes it easier to reuse compiler helpers for reflected types, default values, and safe AST cloning.

The exported helpers do not cover every possible `Wf*` node shape. When no helper exists for the exact node being generated, create the specific AST node directly and fill its fields. Use the helpers below for the common cases where Workflow already provides a stable API.

## Build AST From Existing Workflow Text

Use the parsing helpers only when the input is already Workflow source text:

- Use `ParseType` to parse a textual type into a `WfType`.
- Use `ParseExpression` to parse a textual expression into a `WfExpression`.
- Use `ParseStatement` to parse a textual statement into a `WfStatement`.
- Use `ParseCoProviderStatement` to parse a textual co-provider statement into a `WfCoProviderStatement`.
- Use `ParseDeclaration` to parse a textual declaration into a `WfDeclaration`.
- Use `ParseModule` to parse a whole textual module into a `WfModule`.

These functions tokenize the input, invoke the corresponding generated parser entry, and unescape string-related AST content. They are useful at API boundaries where Workflow source text is the input format. They should not be the first choice for generated code when the generator already knows the intended structure.

## Convert Reflected Types To AST

Use reflection-to-AST helpers when generated Workflow code needs to refer to C++ reflected types:

- Use `GetExpressionFromTypeDescriptor` when a generated expression needs a globally qualified type-name expression for an `ITypeDescriptor`.
- Use `GetTypeFromTypeDescriptor` when a generated type annotation should name an `ITypeDescriptor` directly.
- Use `GetTypeFromTypeInfo` when the source of truth is an `ITypeInfo`, including decorated types.

`GetTypeFromTypeInfo` is the safest choice for generated function arguments, local variables, fields, and return types backed by reflection metadata. It handles raw pointers, shared pointers, nullable values, descriptors, and known generic shapes such as enumerable, map, observable-list, and function types. This avoids manually spelling namespace fragments or accidentally choosing the wrong Workflow type wrapper.

## Copy Existing AST

Use copy helpers when a generator needs to reuse an existing AST subtree without aliasing the original node:

- Use `CopyType` to clone a `WfType`.
- Use `CopyExpression` to clone a `WfExpression`.
- Use `CopyStatement` to clone a `WfStatement`.
- Use `CopyDeclaration` to clone a `WfDeclaration`.
- Use `CopyModule` to clone a `WfModule`.

Each copy helper has overloads accepting either `Ptr<T>` or raw `T*`, so callers can use whichever form they already have.

For `CopyExpression`, `CopyStatement`, `CopyDeclaration`, and `CopyModule`, the `expandVirtualExprStat` argument controls whether virtual frontend constructs are copied as written or copied through their expanded ordinary AST:

- Pass `false` when the generated output should preserve the original AST shape.
- Pass `true` when backend-ready AST is needed and expanded expressions, statements, or declarations should replace virtual nodes when available.

`CopyType` does not take `expandVirtualExprStat` because Workflow types do not use the same virtual expansion mechanism.

## Create Default-Value Expressions

Use `CreateDefaultValue` when generated code needs a default expression for an `ITypeInfo`.

`CreateDefaultValue` produces the Workflow expression shape expected by compiler-generated code:

- enum defaults are built as integer `0`, cast through `U8`, then cast to the target enum type;
- strings are built as empty string expressions;
- non-serializable structs are built as `{}` and cast to the struct type;
- booleans are built as `false`;
- numeric primitives are built as `0` inferred as the target type;
- other reference-like values are built as `null` inferred as the target type.

This helper is the right defaulting API for generated fields, cached values, and temporary variables whose types come from reflection metadata.

## Generate Complete RPC Wrapper Modules

Use the RPC module-generation helpers when the goal is a complete generated module rather than individual node construction:

- Use `CopyAndClearRpcMetadata` to clone RPC metadata input and remove declarations that already exist as reflected types.
- Use `GenerateModuleRpc` after RPC validation has populated metadata and the generator needs the normal RPC wrapper module.
- Use `GenerateModuleRpcJson` after RPC validation has populated metadata and the generator needs the JSON-enabled RPC wrapper and serializer module.

These functions return `WfModule` instances. They are high-level module generators, not general-purpose factories for arbitrary AST nodes. Choose them when generating the standard Workflow RPC support modules; choose the lower-level helpers or direct AST construction for other generated code.

## Print Generated AST

Use `WfPrint` after AST generation when generated Workflow needs to be inspected, logged, compared with baselines, or emitted as text. `WfPrint` has overloads for `WfAttribute`, `WfType`, `WfExpression`, `WfStatement`, `WfDeclaration`, and `WfModule`, and supports both parsing writers and text writers.

`WfPrint` is not an AST-creation API. It is the matching output and verification tool after code has already been represented as AST.

## Choosing The Right Helper

- Use direct `Wf*` node construction when generated code is already structured and no exported helper matches the needed node shape.
- Use `ParseType`, `ParseExpression`, `ParseStatement`, `ParseCoProviderStatement`, `ParseDeclaration`, or `ParseModule` only when the input is Workflow text.
- Use `GetExpressionFromTypeDescriptor`, `GetTypeFromTypeDescriptor`, or `GetTypeFromTypeInfo` when the generated AST should follow reflection metadata.
- Use `CopyType`, `CopyExpression`, `CopyStatement`, `CopyDeclaration`, or `CopyModule` when reusing existing AST without sharing nodes.
- Use `CreateDefaultValue` when a generated initializer should match Workflow compiler conventions for a reflected type.
- Use `CopyAndClearRpcMetadata`, `GenerateModuleRpc`, or `GenerateModuleRpcJson` only for complete generated RPC support modules.
- Use `WfPrint` to inspect or serialize generated AST after construction.
