investigate repro

Since all `IRpcList(Event)?Ops` implementations are redirection of `IRpcObject(Event)?Ops`,
so there should not be any necessity of invoking `IRpcDispatcher`'s `BroadcastFromClient_ListEventOps` and `SendToClient_ListOps`.
Delete these two methods from this interface and do all clean ups.
Follow `REPO-ROOT/.github/Rules/verify-and-commit.md` to finish the work.
The last work of refactoring for letting `IRpcList(Event)?Ops` being redirection of `IRpcObject(Event)?Ops` didn't update the document.
You are going to read documents and figure out if anything mis-aligned.
