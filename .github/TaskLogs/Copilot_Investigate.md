# !!!INVESTIGATE!!!

# PROBLEM DESCRIPTION

In ../xgSkins/suggestions/Workflow/ChainedObserve.md, it describes a bug, basically say
`x.observe as y(...).observe as z(...)`
causing the compiler to crash.

I think this is a valid expression. just like a simple chain binding `bind(a.b.c)` when `a.b` changed the `.c` binding will unbind from the previous value and then bind the current value. In this case it should be equivalent to `bind(a.observe as _a(_a.b on _a.bChanged).observe as _b(_b.c on _b.cChanged)`.

You are going to first read the current workflow language manual and see if the statement fits the description. If the manual is compatible with the above statement, then this is clearly a workflow compiler bug. Add simple but efficient test cases in `Codegen` category, repro the crash, and fix it.

# UPDATES

# TEST [CONFIRMED]

The manual allows any expression as the receiver of `observe as`, so the first observation can supply the receiver for the second. Its property-chain examples show downstream handlers moving to the new receiver when the upstream value changes. [Explicit observations](https://gaclib.net/doc/current/workflow/lang/bind_observe.html) and [data bindings](https://gaclib.net/doc/current/workflow/lang/bind.html).

Run the same changes through a property chain and a chain of two explicit observations in `<Workflow repo>/Test/Resources/Codegen/BindChainedObserve.txt`. Replace the child, fire both events on the old and new children, clear the child, restore the first child, then close the subscription. Both bindings must publish each new value exactly once, including the fallback while the child is null. Only the explicit binding should publish an extra value for the separately listed refresh event. Changes to former children and changes after close must leave both traces unchanged.

Reproduce the compiler failure before changing the analyzer. After the fix, check the exact traces in the Workflow VM and generated C++ against `<Workflow repo>/Test/Resources/IndexCodegen.txt`.

Before the fix, CDB stopped on an unhandled C++ exception in `BindChainedObserve` during the x86 code-generation pass. The stack showed the outer alias calling `CreateBindContextVisitor::DirectDepend`, revisiting the inner observation and reaching its alias registration a second time. `Dictionary::Add` rejected the duplicate key in `<Workflow repo>/Source/Analyzer/WfAnalyzer_ExpandBindExpression.cpp` before the compiler could emit the sample.

# PROPOSALS

- No.1 Reuse the receiver's dependencies when resolving an observe alias [CONFIRMED]

## No.1 Reuse the receiver's dependencies when resolving an observe alias

`CreateBindContextVisitor::Visit(WfObserveExpression*)` processes the receiver before the observed value and its event list. Each reference to the alias can therefore propagate the receiver's existing causes. Traversing the receiver again registers its inner aliases and events a second time. The `let` reference path already uses the same dependency propagation without a second traversal.

### CODE CHANGE

In `<Workflow repo>/Source/Analyzer/WfAnalyzer_ExpandBindExpression.cpp`, pass `false` for `processDepended` when calling `DirectDepend` for an observe alias. Keep recording the alias and copying dependency edges so that upstream changes still refresh the downstream receiver and its handlers.

Register the sample's generated C++ and reflection files in the `Generated_Cpp` and `Generated_Reflection` shared projects so each native test configuration executes it.

### CONFIRMED

Both target architectures passed all 874 compiler cases. In the VM and all three generated-C++ configurations on both Win32 and x64, the property chain published `[1][2][4][-1][3][6]`. The explicit chain published `[1][2][4][4][-1][3][6]`, with the extra `4` coming from the current child's `Refreshed` event. Firing that event on the former child produced no update. The `-1` and subsequent `3` verify clearing the receiver and rebinding to the first child. The final mutation after close produced no update.

Both Debug builds completed with zero warnings and errors. On each platform the VM passed 286 cases, each generated-C++ suite passed 252 cases, and the library, metadata and RPC stdio checks passed. The TypeScript package passed `tsc --noEmit`.

The full `Build.ps1 Workflow` run passed for Win32 and x64 Release and regenerated `<Workflow repo>/Release/VlppWorkflowCompiler.cpp` with the same analyzer fix.
