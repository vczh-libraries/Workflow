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

This is the disassembly of the caller function containing the try-catch and the subsequent `SetDebuggerForCurrentThread(nullptr)` call. All offsets are the low 4 hex digits of the absolute address. The function base is at offset `7000`.

**Prologue and setup (line 811):**

| Offset | Instruction | Explanation |
|--------|-------------|-------------|
| `7000` | `push rbx` | Save callee-saved registers |
| `7002` | `push rsi` | |
| `7003` | `push rdi` | |
| `7004` | `push r12` | |
| `7006` | `push r13` | |
| `7008` | `push r14` | |
| `700a` | `push r15` | |
| `700c` | `sub rsp,110h` | Allocate 272 bytes of stack space |
| `7013` | `mov rbx,rcx` | Save first argument (lambda capture pointer) into `rbx` |
| `7016` | `xor edi,edi` | Zero `edi` — used throughout for zero-initialization |
| `7018` | `xorps xmm0,xmm0` | Zero `xmm0` — used for bulk zero-initialization of `Ptr` fields |
| `701b` | `movups [rsp+0B8h],xmm0` | Zero-initialize `Ptr` stack slot (counter + reference) |
| `7023` | `movups [rsp+0C8h],xmm0` | Zero-initialize `Ptr` stack slot (originalReference + originalDestructor) |

*(Lines 812–831 omitted: `MultithreadDebugger` construction, `SetDebuggerForCurrentThread(debugger)`, `CreateThreadContextFromSample(L"RaiseException")`, and `LoadFunction<void()>(context, L"<initialize>")()` — setup code not relevant to the bug.)*

**Begin try block — `LoadFunction<vint()>(context, L"Main2")()` (line 834):**

| Offset | Instruction | Explanation |
|--------|-------------|-------------|
| `742d` | `lea rax,[ObjectString<wchar_t>::vftable]` | Begin `WString` construction for `L"Main2"` literal |
| ... | *(WString setup, operator new, memcpy)* | *(String literal construction — ~80 bytes of setup code)* |
| `74f4` | `lea r8,[rsp+48h]` | Arguments for `LoadFunction` |
| `74f9` | `lea rdx,[rsp+98h]` | |
| `7501` | `lea rcx,[rsp+20h]` | |
| `7506` | `call LoadFunction<vint()>` | Look up the `Main2` function |
| `750c` | `mov rcx,[rax+10h]` | Get functor from returned function object |
| `7510` | `mov rax,[rcx]` | Load vtable |
| `7513` | `call [rax+8]` | Invoke `Main2()` — this **throws** `WfRuntimeException` |

**Normal flow cleanup (never reached because Main2 throws) — still line 834:**

| Offset | Instruction | Explanation |
|--------|-------------|-------------|
| `7517` | `mov rcx,[rsp+28h]` | Destroy `LoadFunction` temporary (Dec refcount) |
| `751c` | `test rcx,rcx` | |
| `751f` | `je 7550` | Skip if null counter |
| `7521` | `mov rax,rbx` | `rbx` = `0xFFFFFFFFFFFFFFFF` (-1 for atomic decrement) |
| `7524` | `lock xadd [rcx],rax` | Atomic decrement counter |
| `7529` | `cmp rax,1` | |
| `752d` | `jne 7550` | Skip destruction if refcount > 0 |
| `752f` | `mov rdx,[rsp+38h]` | Load destructor argument |
| `7534` | `mov rcx,[rsp+28h]` | Load counter pointer |
| `7539` | `call [rsp+40h]` | Call destructor |
| `753d` | `xorps xmm0,xmm0` | **Zero `xmm0`** — clearing `Func` fields after destruction |
| `7540` | `movdqu [rsp+28h],xmm0` | Zero out counter + reference |
| `7546` | `mov [rsp+38h],rdi` | Zero out destructor argument |
| `754b` | `mov [rsp+40h],rdi` | Zero out destructor pointer |
| `7550` | `mov rdi,[rsp+58h]` | Destroy `WString` temporary (Dec refcount) |
| `7555` | `test rdi,rdi` | |
| `7558` | `je 757f` | Skip if null |
| `755a` | `lock xadd [rdi],rbx` | Atomic decrement |
| `755f` | `lea rax,[rbx-1]` | Check if refcount hit zero |
| `7563` | `test rax,rax` | |
| `7566` | `jne 757f` | Skip deletion if refcount > 0 |
| `7568` | `mov rcx,[rsp+50h]` | Delete `WString` character buffer |
| `756d` | `call operator delete[]` | |
| `7572` | `mov edx,8` | Delete counter allocation (size=8) |
| `7577` | `mov rcx,rdi` | |
| `757a` | `call operator delete[]` | |

**`TEST_ASSERT(false)` — line 835 (all branches throw, none reach `759e`):**

| Offset | Instruction | Explanation |
|--------|-------------|-------------|
| `757f` | `mov rax,[testContext]` | Load test context pointer |
| `7586` | `test rax,rax` | |
| `7589` | `je 7664` | → throw `UnitTestConfigError` if null |
| `758f` | `cmp dword ptr [rax+38h],2` | Check `testContext->state` |
| `7593` | `je 76b4` | → throw `UnitTestAssertError` (assertion fails with `false`) |
| `7599` | `jmp 768c` | → throw `UnitTestConfigError` (wrong state) |

> **Note**: `TEST_ASSERT(false)` always fails when reached, so all branches throw. Offset `759e` is **only reachable via exception recovery**, never by sequential execution from the try block.

**Exception recovery point (line 841) — reached via EH runtime after catch handler returns:**

| Offset | Instruction | Explanation |
|--------|-------------|-------------|
| `759e` | `movdqu [rsp+98h],xmm0` | **BUG**: Writes first 16 bytes of `Ptr` using **stale** `xmm0` (clobbered by catch funclet and EH runtime) |
| `75a7` | `xor edi,edi` | **Correctly** re-zeros `edi` (compiler recognizes it's volatile across EH boundary) |
| `75a9` | `mov [rsp+0A8h],rdi` | Writes 3rd field (originalReference) = 0 ✓ |
| `75b1` | `mov [rsp+0B0h],rdi` | Writes 4th field (originalDestructor) = 0 ✓ |
| `75b9` | `lea rcx,[rsp+98h]` | Address of the (corrupted) `Ptr` parameter |
| `75c1` | `call SetDebuggerForCurrentThread` | **CRASH** — passes `Ptr` with garbage counter/reference |

> **The single bug**: At `759e`, the compiler should have emitted `xorps xmm0,xmm0` (as it does at `75a7` for `edi`) before using `xmm0` to write the null `Ptr`. It correctly handles `edi` as volatile across the EH boundary, but omits the same treatment for `xmm0`.

**Post-call `Ptr` cleanup and epilogue (lines 841–842):**

| Offset | Instruction | Explanation |
|--------|-------------|-------------|
| `75c7` | `mov rcx,[rsp+78h]` | Destroy the `Ptr` passed to `SetDebuggerForCurrentThread` |
| `75cc` | `mov rbx,0FFFFFFFFFFFFFFFFh` | Load -1 for atomic decrement |
| `75d3` | `test rcx,rcx` | |
| `75d6` | `je 75f6` | Skip if null counter |
| `75d8` | `mov rax,rbx` | |
| `75db` | `lock xadd [rcx],rax` | Atomic decrement counter |
| `75e0` | `cmp rax,1` | |
| `75e4` | `jne 75f6` | Skip destruction if refcount > 0 |
| `75e6` | `mov rdx,[rsp+88h]` | |
| `75ee` | `call [rsp+90h]` | Call destructor |
| `75f6` | `mov rcx,[rsp+0B8h]` | Destroy `MultithreadDebugger` `Ptr` (Dec refcount) |
| ... | *(reference counting and conditional destruction)* | |
| `7651` | `add rsp,110h` | Deallocate stack |
| `7658` | `pop r15` | Restore callee-saved registers |
| `765a` | `pop r14` | |
| `765c` | `pop r13` | |
| `765e` | `pop r12` | |
| `7660` | `pop rdi` | |
| `7661` | `pop rsi` | |
| `7662` | `pop rbx` | |
| `7663` | `ret` | Return |

#### Catch Handler Funclet: `catch$35` (TestDebugger.cpp)

The catch handler is compiled as a separate **funclet** (at a non-contiguous address, offset `5fcc`) that receives the parent function's frame pointer in `rdx`. When the funclet returns, the C++ EH runtime (`_CxxCallCatchBlock`) resumes execution at the recovery point (`759e`) in the main function.

**Prologue (line 838: `catch (const WfRuntimeException& ex)`):**

| Offset | Instruction | Explanation |
|--------|-------------|-------------|
| `5fcc` | `mov [rsp+10h],rdx` | Save parent frame pointer (funclet convention) |
| `5fd1` | `push rbp` | Save frame pointer |
| `5fd2` | `sub rsp,20h` | Allocate 32 bytes of local space |
| `5fd6` | `mov rbp,rdx` | Establish frame — `rbp` points to parent's stack frame |

**`TEST_ASSERT(ex.Message() == L"...")` (line 839):**

| Offset | Instruction | Explanation |
|--------|-------------|-------------|
| `5fd9` | `mov rax,[testContext]` | Check test context |
| `5fe0` | `test rax,rax` | |
| `5fe3` | `jne 600c` | Continue if non-null |
| `5fe5` | `lea rdx,[string "..."]` | testContext null → throw `UnitTestConfigError` |
| ... | *(Error setup and CxxThrowException)* | |
| `600c` | `cmp dword ptr [rax+38h],2` | Check `testContext->state` |
| `6010` | `je 6039` | Continue if `state == 2` |
| `6012` | `lea rdx,[string "..."]` | state ≠ 2 → throw `UnitTestConfigError` |
| ... | *(Error setup and CxxThrowException)* | |
| `6039` | `mov rcx,[rbp+108h]` | Get exception object → pointer to `ex` |
| `6040` | `add rcx,8` | Skip vtable → point to `WString` message field |
| `6044` | `lea rax,[string L"Internal error:..."]` | Load expected message literal |
| `604b` | `xor edx,edx` | |
| `6050` | `lea rax,[rax+2]` | `strlen` loop — count characters in expected string |
| `6054` | `inc rdx` | |
| `6057` | `cmp word ptr [rax],0` | |
| `605b` | `jne 6050` | |
| `605d` | `lea rax,[ObjectString<wchar_t>::vftable]` | Construct temporary `WString` from literal (no heap allocation) |
| `6064` | `mov [rbp+48h],rax` | Store vtable |
| `6068` | `lea rax,[string L"Internal error:..."]` | |
| `606f` | `mov [rbp+50h],rax` | Store buffer pointer |
| `6073` | `xorps xmm0,xmm0` | Zero `xmm0` for temporary `WString` fields |
| `6076` | `movdqu [rbp+58h],xmm0` | Zero counter + dummy fields |
| `607b` | `mov [rbp+68h],rdx` | Store string length |
| `607f` | `mov [rbp+70h],rdx` | Store capacity |
| `6083` | `lea r8,[rbp+48h]` | Pointer to temporary `WString` (expected message) |
| `6087` | `lea rdx,[rbp+150h]` | Pointer to exception message `WString` |
| `608e` | `call ObjectString<wchar_t>::operator<=>` | C++20 three-way comparison — **clobbers `xmm0`** |
| `6093` | `cmp byte ptr [rax],0` | Check if result is "equal" (spaceship == 0) |
| `6096` | `je 60bf` | → assertion passes, go to epilogue |
| `6098` | `lea rdx,[string "TEST_ASSERT(...)"]` | Not equal → throw `UnitTestAssertError` |
| ... | *(Error setup and CxxThrowException)* | |

**Catch funclet epilogue (line 840: `}`):**

| Offset | Instruction | Explanation |
|--------|-------------|-------------|
| `60bf` | `mov rax,0` | Return value 0 → tells EH runtime to resume at recovery point (`759e`) |
| `60c9` | `add rsp,20h` | Deallocate local space |
| `60cd` | `pop rbp` | Restore frame pointer |
| `60ce` | `ret` | Return to `_CxxCallCatchBlock` in the EH runtime |

> After `ret` at `60ce`, the C++ EH runtime runs `_CxxCallCatchBlock` which destroys the caught exception object (`WfRuntimeException`), involving heap free operations. With the debug heap active, these operations clobber `xmm0` with non-zero fill patterns. The EH runtime then transfers control to the recovery point at `759e`, where the stale `xmm0` is used to construct the null `Ptr`.

#### Callee: `SetDebuggerForCurrentThread(Ptr<WfDebugger> debugger)`

This function receives a `Ptr<WfDebugger>` by value (passed via hidden pointer in `rcx`). The `threadDebugger.Set(debugger)` call is fully inlined: it clears the old TLS value, allocates a new `Ptr` copy, copies all fields, increments the reference count, and stores via `TlsSetValue`.

**Prologue (line 768):**

| Offset | Instruction | Explanation |
|--------|-------------|-------------|
| `d530` | `mov [rsp+8],rcx` | Save parameter pointer (home space) |
| `d535` | `push rbx` | Save callee-saved `rbx` |
| `d536` | `sub rsp,20h` | Allocate 32 bytes of stack space |
| `d53a` | `mov rbx,rcx` | `rbx` = pointer to `Ptr` parameter |

**`threadDebugger.Set(debugger)` — inlined (line 769):**

| Offset | Instruction | Explanation |
|--------|-------------|-------------|
| `d53d` | `lea rcx,[threadDebugger+8]` | Address of `ThreadLocalStorage` inside `threadDebugger` |
| `d544` | `call ThreadLocalStorage::Clear` | Clear old TLS value |
| `d549` | `mov ecx,20h` | Allocate 32 bytes (`sizeof(Ptr<WfDebugger>)`) |
| `d54e` | `call operator new` | Allocate new `Ptr` for TLS storage |
| `d553` | `mov rdx,rax` | `rdx` = new allocation |
| `d556` | `mov [rsp+38h],rax` | Save pointer on stack |
| `d55b` | `xorps xmm0,xmm0` | Zero-initialize the new `Ptr` |
| `d55e` | `movups [rax],xmm0` | |
| `d561` | `movups [rax+10h],xmm0` | |
| `d565` | `mov r8,[rbx]` | Load `debugger.counter` — **`0x50000163` (garbage)** when corrupted |
| `d568` | `mov [rax],r8` | Copy counter into new `Ptr` |
| `d56b` | `mov rcx,[rbx+8]` | Load `debugger.reference` |
| `d56f` | `mov [rax+8],rcx` | Copy reference |
| `d573` | `mov rcx,[rbx+10h]` | Load `debugger.originalReference` |
| `d577` | `mov [rax+10h],rcx` | Copy originalReference |
| `d57b` | `mov rax,[rbx+18h]` | Load `debugger.originalDestructor` |
| `d57f` | `mov [rdx+18h],rax` | Copy originalDestructor |
| `d583` | `test r8,r8` | Check if counter is non-null |
| `d586` | `je d58c` | Skip `Inc()` if counter is null |
| `d588` | `lock inc qword ptr [r8]` | **CRASH HERE**: Atomically increment `*counter` — `r8 = 0x50000163` (unmapped) → **Access Violation** |
| `d58c` | `movzx eax,byte ptr [threadDebugger+20h]` | Check if TLS slot is initialized |
| `d593` | `test al,al` | |
| `d595` | `jne d5df` | → throw `Error` if not initialized |
| `d597` | `mov ecx,[threadDebugger+10h]` | Load TLS slot index |
| `d59d` | `call [TlsSetValue]` | Store new `Ptr` into thread-local storage |

**Parameter `Ptr` cleanup and epilogue (line 770: `}`):**

| Offset | Instruction | Explanation |
|--------|-------------|-------------|
| `d5a4` | `mov rcx,[rbx]` | Load counter from parameter `Ptr` |
| `d5a7` | `test rcx,rcx` | |
| `d5aa` | `je d5d9` | Skip `Dec()` if null — jump to epilogue |
| `d5ac` | `mov rax,0FFFFFFFFFFFFFFFFh` | Load -1 for atomic decrement |
| `d5b3` | `lock xadd [rcx],rax` | Atomically decrement `*counter` |
| `d5b8` | `cmp rax,1` | |
| `d5bc` | `jne d5d9` | If refcount > 0 after dec, skip destruction — jump to epilogue |
| `d5be` | `mov rdx,[rbx+10h]` | Load `originalReference` for destructor |
| `d5c2` | `mov rcx,[rbx]` | Load counter pointer |
| `d5c5` | `call [rbx+18h]` | Call `originalDestructor` to destroy the object |
| `d5c8` | `xor eax,eax` | Zero `eax` |
| `d5ca` | `mov [rbx],rax` | Zero out parameter `Ptr` fields |
| `d5cd` | `mov [rbx+8],rax` | |
| `d5d1` | `mov [rbx+10h],rax` | |
| `d5d5` | `mov [rbx+18h],rax` | |
| `d5d9` | `add rsp,20h` | Deallocate stack |
| `d5dd` | `pop rbx` | Restore `rbx` |
| `d5de` | `ret` | Return |

> The callee is correct. It faithfully reads whatever the caller placed on the stack. The corruption is entirely in the caller's construction of the `Ptr` parameter.

## Workaround

The workaround (in commit `01663cd6b75f018d7405151a66820743c2115898`, which the latest commit reverts) avoids the bug by:

1. **Replacing `SetDebuggerForCurrentThread(nullptr)` with `ResetDebuggerForCurrentThread()`**: The new function calls `threadDebugger.Clear()` directly, avoiding the construction of a null `Ptr<WfDebugger>` parameter entirely. This sidesteps the corrupted XMM register issue.

2. **Splitting `GetDebuggerForCurrentThread()` from a ternary into if/else**: Changes `return threadDebugger.HasData() ? threadDebugger.Get() : nullptr;` to explicit if/else blocks, which may also avoid similar optimizer issues with XMM register reuse.

3. **Splitting `GetDebuggerCallback()` into two statements**: Changes `return GetDebuggerCallback(GetDebuggerForCurrentThread().Obj());` to use a local variable, preventing potential temporary lifetime issues under aggressive optimization.

## Conclusion

This is a **confirmed MSVC compiler bug** in the optimizer's register liveness analysis. The optimizer fails to recognize that `xmm0` (a volatile register) is dead at C++ exception recovery targets, incorrectly propagating a cached zero value across the try-catch boundary. The user's code is valid C++20 with no undefined behavior.
