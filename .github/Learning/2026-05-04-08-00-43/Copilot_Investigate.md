# !!!INVESTIGATE!!!

# PROBLEM DESCRIPTION

The RPC byval return path releases collections of interfaces too early when JSON serialization is enabled. `CppTest*` crashes for `Rpc:Collection_Interface_InByref_OutByval` because JSON serialization converts returned interface elements into transport values before the caller has created wrappers. `RuntimeTest` does not hit the crash because JSON serialization is not enabled for that path.

The requested fix is to address this at the interface/protocol level:

- Add `system::RpcByvalReturnValue` with `value : object` and `slot : int`.
- Only use `RpcByvalReturnValue` when a method return value is marked with `@rpc:Byval`.
- Have generated `rpcops_IRpcObjectOps` and `rpcops_IRpcObjectOpsJson` allocate incremental slots, cache recursive copies of byval return collections in `_byvalReturnValues`, and return `RpcByvalReturnValue`.
- Add `IRpcObjectOps::EndInvokeMethod(slot : int)` so caller-side generated wrappers can release the cached copied collection after deserializing and unboxing the return value.
- Add `IRpcLifecycle::RpcCopyByval` to recursively copy nested collections before boxing or JSON serialization.
- Remove the old `RpcByvalKeepAliveProperty` and `RpcByvalKeepAlive` construction.
- Update outdated RPC design notes, especially the generated-wrapper and JSON return descriptions, and add a byval return lifecycle section to `TODO_RPC_Definition.md`.

# UPDATES

- Reproduced the JSON-enabled crash in `CppTest` x64 at `Rpc:Collection_Interface_InByref_OutByval` with `RpcLifecycleBase::RefToPtr(RpcObjectReference)#Object not registered.`
- Added reflected `system::RpcByvalReturnValue`, `IRpcObjectOps::EndInvokeMethod`, and `IRpcLifecycle::RpcCopyByval`.
- Reworked native byval copying/boxing so `RpcCopyByval` recursively copies collection layers and `RpcBoxByval` recursively boxes the copied collection without any keep-alive property.
- Updated generated non-JSON and JSON object ops to cache copied byval return collections by slot and return `RpcByvalReturnValue`.
- Updated generated caller-side ops to cast byval return results directly to `RpcByvalReturnValue^`, deserialize/unbox `value`, call `EndInvokeMethod(slot)`, and return the local result.
- Regenerated reflection metadata and RPC wrapper outputs after changing the reflected API.
- Updated the RPC documentation to describe slot-based byval return cleanup.

# TEST

- [x] [CONFIRMED] Reproduced the original x64 `CppTest` failure in `Rpc:Collection_Interface_InByref_OutByval`: `RpcLifecycleBase::RefToPtr(RpcObjectReference)#Object not registered.`
- [x] [CONFIRMED] `.\.github\Scripts\copilotBuild.ps1 -Configuration Debug -Platform x64` completed successfully after regenerated outputs and source fixes.
- [x] [CONFIRMED] `.\.github\Scripts\copilotBuild.ps1 -Configuration Debug -Platform Win32` completed successfully after regenerated outputs and source fixes.
- [x] [CONFIRMED] `.\.github\Scripts\copilotExecute.ps1 -Mode UnitTest -Executable LibraryTest -Configuration Debug -Platform x64` completed successfully with 2/2 files and 14/14 test cases passed.
- [x] [CONFIRMED] `.\.github\Scripts\copilotExecute.ps1 -Mode UnitTest -Executable LibraryTest -Configuration Debug -Platform Win32` completed successfully with 2/2 files and 14/14 test cases passed.
- [x] [CONFIRMED] `.\.github\Scripts\copilotExecute.ps1 -Mode UnitTest -Executable CompilerTest_GenerateMetadata -Configuration Debug -Platform x64` completed successfully.
- [x] [CONFIRMED] `.\.github\Scripts\copilotExecute.ps1 -Mode UnitTest -Executable CompilerTest_GenerateMetadata -Configuration Debug -Platform Win32` completed successfully.
- [x] [CONFIRMED] `.\.github\Scripts\copilotExecute.ps1 -Mode UnitTest -Executable CompilerTest_LoadAndCompile -Configuration Debug -Platform x64` completed successfully after both metadata architectures were regenerated.
- [x] [CONFIRMED] `.\.github\Scripts\copilotExecute.ps1 -Mode UnitTest -Executable RuntimeTest -Configuration Debug -Platform x64` completed successfully with 4/4 files and 257/257 test cases passed.
- [x] [CONFIRMED] `.\.github\Scripts\copilotExecute.ps1 -Mode UnitTest -Executable RuntimeTest -Configuration Debug -Platform Win32` completed successfully with 4/4 files and 257/257 test cases passed.
- [x] [CONFIRMED] `.\.github\Scripts\copilotExecute.ps1 -Mode UnitTest -Executable CppTest -Configuration Debug -Platform x64` completed successfully with 2/2 files and 223/223 test cases passed.
- [x] [CONFIRMED] `.\.github\Scripts\copilotExecute.ps1 -Mode UnitTest -Executable CppTest -Configuration Debug -Platform Win32` completed successfully with 2/2 files and 223/223 test cases passed.
- [x] [CONFIRMED] `.\.github\Scripts\copilotExecute.ps1 -Mode UnitTest -Executable CppTest_Metaonly -Configuration Debug -Platform x64` completed successfully with 2/2 files and 223/223 test cases passed.
- [x] [CONFIRMED] `.\.github\Scripts\copilotExecute.ps1 -Mode UnitTest -Executable CppTest_Metaonly -Configuration Debug -Platform Win32` completed successfully with 2/2 files and 223/223 test cases passed.
- [x] [CONFIRMED] `.\.github\Scripts\copilotExecute.ps1 -Mode UnitTest -Executable CppTest_Reflection -Configuration Debug -Platform x64` completed successfully with 2/2 files and 223/223 test cases passed.
- [x] [CONFIRMED] `.\.github\Scripts\copilotExecute.ps1 -Mode UnitTest -Executable CppTest_Reflection -Configuration Debug -Platform Win32` completed successfully with 2/2 files and 223/223 test cases passed.
- [x] [CONFIRMED] `git grep -n "RpcByvalKeepAlive\|ByvalKeepAlive\|RpcTransferByvalKeepAlive" -- Source Test/Generated Test/SourceCppGenRpc Test/Resources TODO_RPC_Json.md TODO_RPC_GeneratedWrappers.md TODO_RPC_Definition.md` found no tracked references.

# PROPOSALS

## COMPLETED

The root cause is fixed by making byval collection returns a two-step protocol. The callee returns transport data plus a slot, while the callee-side object ops keeps a recursive copy of the real returned collection alive under that slot. The caller converts the transport data into the real return value first, then explicitly calls `EndInvokeMethod(slot)` to release the callee cache. This preserves interface elements across JSON serialization and avoids special keep-alive attachment on serialized values.
