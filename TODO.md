# TODO

## Progressing

- Fix `CompareValue` instruction.

## 2.0

- Generated C++ code try not to create IValueList if apply `{1 2 3}` on `List<int>` argument.
- Dump binary type metadata including only new types created in Workflow.
- Type inferencing on all methods in generic collection intertfaces.
  - Consider adding a compile-time general generic mechanism.
- In generated C++ code, when a collection instance is required from an interface:
  - Try to cast to the collection instance directly using the pointer inside the implementation of the interface.
  - Create the collection instance and copy all the data if the above step failed.

## Issues

- `let X = Y in (F(X.a.b.c))` reports `X does not exist in the current scope` in binding property value
  - while `F((let X = Y.a.b).c)` works

## Optional

- Support `<category>name` identifier, make generated code re-parsable.
- Recognize collection interfaces instead of collection entities.
  - Reconsider how to implement collection creating code in Workflow.
  - Only if `Vlpp` decides to add collection interfaces.
- Virtual functions.
- Class implements interfaces.
