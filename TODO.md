# TODO

## 2.0

- Generated C++ code try not to create IValueList if apply `{1 2 3}` on `List<int>` argument.
- Dump binary type metadata including only new types created in Workflow.

## With VlppParser2

- Move to new ParserGen.

## Issues

- Collection literal `{a b c}` should react to the "receiving type" if the receiving type is a known collection type.
  - So that it is able to do `DealWithObservableList({1 2 3});`
- `let X = Y in (F(X.a.b.c))` reports `X does not exist in the current scope` in binding property value
  - while `F((let X = Y.a.b).c)` works

## Optional

- Support `<category>name` identifier, make generated code re-parsable.
- Recognize collection interfaces instead of collection entities.
  - Reconsider how to implement collection creating code in Workflow.
  - Only if `Vlpp` decides to add collection interfaces.
- Virtual functions.
- Class implements interfaces.
