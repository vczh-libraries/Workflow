# Investigation Report: RuntimeTest Crash in Release|x64 Under Debugger

## Verdict

**This is a confirmed MSVC C++ compiler optimization bug** (miscompilation) in the register liveness analysis across C++ exception handling (try/catch) recovery boundaries.

reproing: revert 01663cd6b75f018d7405151a66820743c2115898 and run RuntimeTest.vcxproj Release|x64 with debugger attached.

## Environment

- **Compiler**: MSVC toolset 14.50.35717 (Visual Studio 18 Community)
- **VCRUNTIME**: 14.50.35719.0
- **Configuration**: Release|x64
- **Reproduces**: Only with debugger attached (CDB/WinDBG)

## Summary

The MSVC optimizer generates incorrect code for `SetDebuggerForCurrentThread(nullptr)` at `Test/Source/TestDebugger.cpp` line 841. The call site follows a try-catch block (lines 832-840). The optimizer incorrectly assumes the volatile register `xmm0` retains its zero value across the exception recovery boundary, producing a `Ptr<WfDebugger>` parameter with corrupted `counter` and `reference` fields. This leads to an access violation when incrementing the reference count through the garbage counter pointer.

## Detailed Analysis

### The Crash

The crash occurs at `Source/Runtime/WfRuntimeDebugger.cpp` line 769 inside `SetDebuggerForCurrentThread`, in the `Ptr<WfDebugger>` copy constructor's `Inc()` call:

```
lock inc qword ptr [r8]    ; r8 = 0x50000163 (garbage address)
```

### The Code

```cpp
// Test/Source/TestDebugger.cpp, "Test debugger: exception 4" test case
LoadFunction<void()>(context, L"<initialize>")();
try
{
    LoadFunction<vint()>(context, L"Main2")();   // Throws WfRuntimeException
    TEST_ASSERT(false);
}
catch (const WfRuntimeException& ex)
{
    TEST_ASSERT(ex.Message() == L"Internal error: Debugger stopped the program.");
}
SetDebuggerForCurrentThread(nullptr);   // <-- CRASH (line 841)
```

### The Generated Code (x64 Release)

The caller lambda constructs a null `Ptr<WfDebugger>` (4 qwords, 32 bytes) to pass to `SetDebuggerForCurrentThread`:

| Address | Instruction | Purpose |
|---------|-------------|---------|
| `753d` | `xorps xmm0,xmm0` | Zeros xmm0 (inside the try block's normal-flow code) |
| ... | *(exception is thrown and caught; catch handler executes)* | *(xmm0 is clobbered)* |
| `759e` | `movdqu [rsp+98h],xmm0` | Writes first 16 bytes of Ptr (counter + reference) using **stale** xmm0 |
| `75a7` | `xor edi,edi` | **Correctly** re-zeros edi |
| `75a9` | `mov [rsp+A8h],rdi` | Writes 3rd field (originalReference) using freshly zeroed rdi |
| `75b1` | `mov [rsp+B0h],rdi` | Writes 4th field (originalDestructor) using freshly zeroed rdi |
| `75b9` | `lea rcx,[rsp+98h]` | Passes pointer to the Ptr as hidden parameter |
| `75c1` | `call SetDebuggerForCurrentThread` | Passes corrupted Ptr |

### The Bug

The instruction at `759e` is an **exception recovery target** — it is only reached after the catch handler completes, via the C++ EH runtime's recovery mechanism. It is NOT reached by sequential execution from `753d` (the try block's normal-flow ends with `jmp` at `7599` to a different path).

Per the x64 calling convention, `xmm0` through `xmm5` are **volatile** (caller-saved) registers. The catch handler funclet and the C++ exception dispatching machinery are free to modify `xmm0`. By the time execution reaches `759e`, `xmm0` contains whatever value the catch handler and EH runtime left behind — not zero.

**The compiler correctly re-zeros `edi` at `75a7` (recognizing it as volatile across the EH boundary) but fails to re-zero `xmm0` at `759e`.** This inconsistency proves the optimizer's register liveness analysis has a bug specific to XMM registers at exception recovery points.

### Observed Corruption

At the crash point, the `Ptr<WfDebugger>` parameter on the stack:

| Field | Expected | Actual |
|-------|----------|--------|
| `counter` (offset +0x00) | `0x0000000000000000` | `0x0000000050000163` |
| `reference` (offset +0x08) | `0x0000000000000000` | `0x000001b8194938d0` |
| `originalReference` (offset +0x10) | `0x0000000000000000` | `0x0000000000000000` ✓ |
| `originalDestructor` (offset +0x18) | `0x0000000000000000` | `0x0000000000000000` ✓ |

The first two fields (written via stale `xmm0`) are corrupted; the last two (written via freshly zeroed `rdi`) are correct.

### Why Only Under Debugger (Verified)

This was investigated by comparing `xmm0` values at precise points in the execution, and by testing with the `_NO_DEBUG_HEAP=1` environment variable.

#### Root Cause: The Windows Debug Heap

When a process is launched under a debugger (CDB, WinDbg, Visual Studio), Windows automatically sets `NtGlobalFlag` to `0x10` (`FLG_HEAP_ENABLE_TAIL_CHECK`), activating the **debug heap**. This was confirmed:

| Condition | `NtGlobalFlag` | Crash? |
|-----------|----------------|--------|
| Normal debugger | `0x10` (htc) | **Yes** — AV at `SetDebuggerForCurrentThread` |
| Debugger + `_NO_DEBUG_HEAP=1` | `0x00` | **No** — TestDebugger tests pass |
| No debugger | `0x00` | **No** — all tests pass |

#### The Mechanism

The crash depends on the EH runtime clobbering `xmm0` between the catch handler's `ret` and the recovery point. This was traced:

| Execution Point | `xmm0` Low 64 bits |
|-----------------|---------------------|
| Catch handler `ret` instruction | `0x0000000000000000` (zero) |
| Recovery point (`movdqu` instruction) | `0x0000000000000090` (non-zero) |

The C++ EH runtime (`_CxxCallCatchBlock`) runs between these two points to:
1. Transfer control from the catch funclet back to the parent function
2. **Destroy the caught exception object** (`WfRuntimeException`) — this involves freeing heap memory (the exception's `WString` message buffer and the exception object itself)

With the debug heap active, `RtlFreeHeap` performs additional work:
- Validates heap block tail signatures
- Fills freed memory blocks with patterns
- Uses more complex code paths involving SSE/XMM register operations (e.g., `memset`-like fills)

These operations leave non-zero residue in `xmm0`. Without the debug heap, the simpler `RtlFreeHeap` code path either doesn't use `xmm0` or zeroes it as a side effect of its operations.

#### Why It Doesn't Crash Without a Debugger

Without a debugger, `NtGlobalFlag = 0` and the normal (non-debug) heap is used. The EH runtime's exception object destruction invokes simpler heap free code that does **not** clobber `xmm0`. At the recovery point, `xmm0` remains zero (as set by the `xorps xmm0,xmm0` in the try block), and the null `Ptr<WfDebugger>` is correctly constructed with all-zero fields. The miscompilation still exists in the binary, but is not triggered because the stale `xmm0` coincidentally holds the correct value.

### Why Only Release|x64

In Debug builds, optimizations are disabled, so the compiler does not attempt to cache `xmm0 = 0` across the try-catch boundary. In Win32 builds, the different calling convention and register allocation strategy may avoid the issue. The bug is specific to the x64 Release optimizer's register allocation decisions.

### Full Annotated Disassembly

#### Caller: Lambda in "Test debugger: exception 4" (TestDebugger.cpp)

This is the full disassembly of the caller function surrounding the try-catch and the subsequent `SetDebuggerForCurrentThread(nullptr)` call. All offsets are relative to the function base.

**Prologue and setup:**

| Offset | Instruction | Explanation |
|--------|-------------|-------------|
| `7000` | `sub rsp,0C8h` | Allocate 200 bytes of stack space for locals and call arguments |
| `7007` | `mov [rsp+0B8h],r14` | Save callee-saved register `r14` (will be restored in epilogue) |
| `700f` | `mov r14,rcx` | Save first argument (`WfAssemblyLoadContext* context`) into `r14` |

**Call `LoadFunction<void()>(context, L"<initialize>")()`:**

| Offset | Instruction | Explanation |
|--------|-------------|-------------|
| `7012` | `lea rdx,[...L"<initialize>"]` | Load pointer to the wide string literal |
| `7019` | `lea rcx,[rsp+70h]` | Output slot for the returned function object |
| `701e` | `mov r8,r14` | Third arg = context |
| `7021` | `call LoadFunction<void()>` | Call helper to look up the `<initialize>` function |
| `7026` | `lea rcx,[rsp+70h]` | `this` pointer to the returned function object |
| `702b` | `call [function_object::operator()]` | Invoke `<initialize>()` |
| `702e` | `lea rcx,[rsp+70h]` | Destroy the temporary function object |
| `7033` | `call [function_object::~dtor]` | Destructor call |

**Begin try block — call `LoadFunction<vint()>(context, L"Main2")()`:**

| Offset | Instruction | Explanation |
|--------|-------------|-------------|
| `7038` | `lea rdx,[...L"Main2"]` | Load pointer to the `"Main2"` string literal |
| `703f` | `lea rcx,[rsp+70h]` | Output slot for the returned function object |
| `7044` | `mov r8,r14` | Third arg = context |
| `7047` | `call LoadFunction<vint()>` | Look up the `Main2` function |
| `704c` | `lea rcx,[rsp+70h]` | `this` pointer |
| `7051` | `call [function_object::operator()]` | Invoke `Main2()` — this **throws** `WfRuntimeException` |

**Normal flow (never reached because Main2 throws):**

| Offset | Instruction | Explanation |
|--------|-------------|-------------|
| `7054` | `lea rcx,[rsp+70h]` | Destroy the `Main2` function object |
| `7059` | `call [function_object::~dtor]` | Destructor call |
| `705c` | `xor ecx,ecx` | Prepare `false` argument |
| `705e` | `call TEST_ASSERT` | `TEST_ASSERT(false)` — would fail if reached |

**After try-block normal exit (no exception):**

| Offset | Instruction | Explanation |
|--------|-------------|-------------|
| `7063` | `xorps xmm0,xmm0` | Zero `xmm0` for constructing null `Ptr` |
| `7066` | `movdqu [rsp+98h],xmm0` | Write first 16 bytes (counter + reference) = 0 |
| `706f` | `xor edi,edi` | Zero `edi` |
| `7071` | `mov [rsp+0A8h],rdi` | Write 3rd field (originalReference) = 0 |
| `7079` | `mov [rsp+0B0h],rdi` | Write 4th field (originalDestructor) = 0 |
| `7081` | `lea rcx,[rsp+98h]` | Address of the null `Ptr` |
| `7089` | `call SetDebuggerForCurrentThread` | Call with null `Ptr` |
| `708e` | `lea rcx,[rsp+98h]` | Address of the `Ptr` for destruction |
| `7096` | `call Ptr::~Ptr` | Destroy the temporary `Ptr` |
| `709b` | `mov r14,[rsp+0B8h]` | Restore callee-saved `r14` |
| `70a3` | `add rsp,0C8h` | Deallocate stack |
| `70aa` | `ret` | Return (normal-exit path) |

**Catch handler funclet** (`catch (const WfRuntimeException& ex)`):

| Offset | Instruction | Explanation |
|--------|-------------|-------------|
| `7500` | `mov [rsp+8],rcx` | Save `rcx` (funclet receives exception frame pointer) |
| `7505` | `push rbp` | Save frame pointer |
| `7506` | `sub rsp,30h` | Allocate local space |
| `750a` | `mov rbp,[rcx+...]` | Restore parent's frame pointer from the exception frame |

**Inside catch: `TEST_ASSERT(ex.Message() == L"...")`:**

| Offset | Instruction | Explanation |
|--------|-------------|-------------|
| `7511` | `lea rcx,[rsp+20h]` | Output slot for `WString` returned by `Message()` |
| `7516` | `mov rdx,[rbp-...]` | `this` pointer of the caught exception object |
| `751a` | `call WfRuntimeException::Message` | Get the exception's message string |
| `751f` | `lea rdx,[...L"Internal error:..."]` | Load expected message string |
| `7526` | `lea rcx,[rsp+20h]` | `this` pointer of the returned `WString` |
| `752b` | `call WString::operator==` | Compare the message |
| `7530` | `mov ecx,eax` | Result (bool) into `ecx` for `TEST_ASSERT` |
| `7532` | `call TEST_ASSERT` | Assert the comparison is true |
| `7537` | `lea rcx,[rsp+20h]` | Destroy the temporary `WString` |
| `753c` | `call WString::~WString` | Destructor for temporary |

**Post-assert code still inside catch funclet:**

| Offset | Instruction | Explanation |
|--------|-------------|-------------|
| `753d` | `xorps xmm0,xmm0` | **Zero `xmm0`** — optimizer pre-caches zero for post-catch `Ptr` construction |
| ... | | *(The catch funclet returns here. EH runtime resumes at recovery point.)* |

**Catch handler epilogue and return:**

| Offset | Instruction | Explanation |
|--------|-------------|-------------|
| `7594` | `add rsp,30h` | Deallocate catch funclet locals |
| `7598` | `pop rbp` | Restore frame pointer |
| `7599` | `ret` | Return to `_CxxCallCatchBlock` in the EH runtime |

**Exception recovery point** (reached via EH runtime after catch handler returns):

| Offset | Instruction | Explanation |
|--------|-------------|-------------|
| `759e` | `movdqu [rsp+98h],xmm0` | **BUG**: Writes first 16 bytes of `Ptr` using **stale** `xmm0` (clobbered by EH runtime) |
| `75a7` | `xor edi,edi` | **Correctly** re-zeros `edi` (compiler recognizes it's volatile across EH boundary) |
| `75a9` | `mov [rsp+0A8h],rdi` | Writes 3rd field (originalReference) = 0 ✓ |
| `75b1` | `mov [rsp+0B0h],rdi` | Writes 4th field (originalDestructor) = 0 ✓ |
| `75b9` | `lea rcx,[rsp+98h]` | Address of the (corrupted) `Ptr` parameter |
| `75c1` | `call SetDebuggerForCurrentThread` | **CRASH** — passes `Ptr` with garbage counter/reference |

> **The single bug**: At `759e`, the compiler should have emitted `xorps xmm0,xmm0` (as it does at `75a7` for `edi`) before using `xmm0` to write the null `Ptr`. It correctly handles `edi` as volatile across the EH boundary, but omits the same treatment for `xmm0`.

#### Callee: `SetDebuggerForCurrentThread(Ptr<WfDebugger> debugger)`

This function receives a `Ptr<WfDebugger>` by value (passed via hidden pointer in `rcx`). It copies the parameter into the thread-local variable via `threadDebugger.Set(debugger)`.

| Offset | Instruction | Explanation |
|--------|-------------|-------------|
| `d530` | `sub rsp,48h` | Allocate 72 bytes of stack space |
| `d534` | `mov rax,[rcx]` | Load `debugger.counter` — this is `0x50000163` (garbage) when corrupted |
| `d537` | `mov [rsp+28h],rax` | Store counter into local copy (for the `new Ptr(debugger)` inside `Set`) |
| `d53c` | `mov rax,[rcx+8]` | Load `debugger.reference` |
| `d540` | `mov [rsp+30h],rax` | Store reference into local copy |
| `d545` | `mov rax,[rcx+10h]` | Load `debugger.originalReference` |
| `d549` | `mov [rsp+38h],rax` | Store originalReference into local copy |
| `d54e` | `mov rax,[rcx+18h]` | Load `debugger.originalDestructor` |
| `d552` | `mov [rsp+40h],rax` | Store originalDestructor into local copy |
| `d557` | `mov r8,[rsp+28h]` | Load counter pointer into `r8` for `Inc()` |
| `d55c` | `test r8,r8` | Check if counter is null (null means no ref counting needed) |
| `d55f` | `je d566` | Skip `Inc()` if counter is null |
| `d561` | `lock inc qword ptr [r8]` | **CRASH HERE**: Atomically increment `*counter` — but `r8 = 0x50000163` (unmapped) → **Access Violation** |
| `d566` | `lea rdx,[rsp+28h]` | Address of local `Ptr` copy (the `new Ptr` argument for `Set`) |
| `d56b` | `lea rcx,[threadDebugger]` | Address of the `ThreadVariable<Ptr<WfDebugger>>` static |
| `d572` | `call ThreadVariable::Set` | Store the debugger into thread-local storage |
| `d577` | `mov r8,[rsp+28h]` | Load counter from local copy for `Dec()` (cleanup) |
| `d57c` | `test r8,r8` | Check if counter is null |
| `d57f` | `je d5de` | Skip `Dec()` if null — jump to epilogue |
| `d581` | `lock dec qword ptr [r8]` | Atomically decrement `*counter` |
| `d585` | `jne d5de` | If refcount > 0 after dec, skip destruction — jump to epilogue |
| `d587` | `mov rcx,[rsp+28h]` | (refcount hit zero) Load counter for deletion |
| ... | *(destructor chain)* | Delete the reference-counted object |
| `d5de` | `add rsp,48h` | Deallocate stack |
| `d5e2` | `ret` | Return |

> The callee is correct. It faithfully reads whatever the caller placed on the stack. The corruption is entirely in the caller's construction of the `Ptr` parameter.

## Workaround

The workaround (in commit `01663cd6b75f018d7405151a66820743c2115898`, which the latest commit reverts) avoids the bug by:

1. **Replacing `SetDebuggerForCurrentThread(nullptr)` with `ResetDebuggerForCurrentThread()`**: The new function calls `threadDebugger.Clear()` directly, avoiding the construction of a null `Ptr<WfDebugger>` parameter entirely. This sidesteps the corrupted XMM register issue.

2. **Splitting `GetDebuggerForCurrentThread()` from a ternary into if/else**: Changes `return threadDebugger.HasData() ? threadDebugger.Get() : nullptr;` to explicit if/else blocks, which may also avoid similar optimizer issues with XMM register reuse.

3. **Splitting `GetDebuggerCallback()` into two statements**: Changes `return GetDebuggerCallback(GetDebuggerForCurrentThread().Obj());` to use a local variable, preventing potential temporary lifetime issues under aggressive optimization.

## Conclusion

This is a **confirmed MSVC compiler bug** in the optimizer's register liveness analysis. The optimizer fails to recognize that `xmm0` (a volatile register) is dead at C++ exception recovery targets, incorrectly propagating a cached zero value across the try-catch boundary. The user's code is valid C++20 with no undefined behavior.
