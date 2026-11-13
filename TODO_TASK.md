In commit a12010e764ef33d867ebb53a388f9211cafa308f, `Source/Parser/Syntax/Ast/Expressions.txt` is changed to add `CallExpression::typedResult`.
This field is to cache semantic analysing result for a `CallExpression` node.
But such thing is not good, AST should only contain parsing result in order not to confuse users.

Remove that field, instead add a `Dictionary` to `WfLexicalScopeManager`, following the current pattern.
I would recommend `CallExpressionTypedResultMap` below `BaseConstructorCallResolvingMap` and `baseConstructorCallResolvings`.
Put the cache here.

Run unit test to make sure the change is working.
