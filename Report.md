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

## Workaround

The workaround (in commit `01663cd6b75f018d7405151a66820743c2115898`, which the latest commit reverts) avoids the bug by:

1. **Replacing `SetDebuggerForCurrentThread(nullptr)` with `ResetDebuggerForCurrentThread()`**: The new function calls `threadDebugger.Clear()` directly, avoiding the construction of a null `Ptr<WfDebugger>` parameter entirely. This sidesteps the corrupted XMM register issue.

2. **Splitting `GetDebuggerForCurrentThread()` from a ternary into if/else**: Changes `return threadDebugger.HasData() ? threadDebugger.Get() : nullptr;` to explicit if/else blocks, which may also avoid similar optimizer issues with XMM register reuse.

3. **Splitting `GetDebuggerCallback()` into two statements**: Changes `return GetDebuggerCallback(GetDebuggerForCurrentThread().Obj());` to use a local variable, preventing potential temporary lifetime issues under aggressive optimization.

## Conclusion

This is a **confirmed MSVC compiler bug** in the optimizer's register liveness analysis. The optimizer fails to recognize that `xmm0` (a volatile register) is dead at C++ exception recovery targets, incorrectly propagating a cached zero value across the try-catch boundary. The user's code is valid C++20 with no undefined behavior.
