# !!!INVESTIGATE!!!

# PROBLEM DESCRIPTION

Workflow/Test/StartRpcStdio.sh core dumped, figure out why and fix it

# UPDATES

# TEST [CONFIRMED]

Run `Test/StartRpcStdio.sh` from the Workflow repository after building the Linux RPC stdio driver and service projects. The failure is confirmed if the driver or one of its service processes dumps core. Success requires the script to finish without a crash and all selected RPC stdio cases to complete. A debugger backtrace should identify the failing ownership or protocol path before any fix is accepted.

Running the launcher without an explicit skip file reproduced the failure. `Collection_Default` completed but returned `[123][][12345]`, then `Collection_InByref_OutByref` terminated with an uncaught `vl::Exception` and Bash reported that `RpcStdioTest_Driver` aborted with a core dump.

The failing sample assigns `xsService` only from `serviceMain`, which runs in the child service process. `clientMain` runs in the driver process and therefore sees its own empty `xsService`; generated `Rpc_Collection_InByref_OutByref::Print5` indexes elements 0 through 4 and throws on that empty list. The archived CDB trace for the same implementation placed the exception in `Rpc_Collection_InByref_OutByref::Print5` and `clientMain`, after the remote RPC method returned. The current generated C++ has the same call path.

Isolating every `IndexRpc.txt` entry confirmed the established compatibility boundary: 70 cases complete across separate processes, while 56 collection cases abort because their client-side result formatting indexes module globals populated only in the service process. This is fixture state coupling, not a stdio transport or RPC dispatch failure.

# PROPOSALS
