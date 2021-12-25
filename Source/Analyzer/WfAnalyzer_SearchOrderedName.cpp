#include "WfAnalyzer.h"

namespace vl
{
	namespace workflow
	{
		namespace analyzer
		{
			using namespace collections;

/***********************************************************************
SearchOrderedName
***********************************************************************/

			class SearchOrderedNameVisitor : public traverse_visitor::AstVisitor
			{
			public:
				WfLexicalScope*							scope;
				SortedList<vint>&						names;

				SearchOrderedNameVisitor(WfLexicalScope* _scope, SortedList<vint>& _names)
					:scope(_scope)
					, names(_names)
				{
				}

				void Traverse(WfOrderedNameExpression* node)override
				{
					vint name = wtoi(node->name.value.Sub(1, node->name.value.Length() - 1));
					if (!names.Contains(name))
					{
						WfLexicalScope* currentScope = scope;
						while (currentScope)
						{
							if (currentScope->symbols.Keys().Contains(node->name.value))
							{
								return;
							}
							currentScope = currentScope->parentScope.Obj();
						}
						names.Add(name);
					}
				}

				void Visit(WfOrderedLambdaExpression* node)override
				{
					// names in nested ordered lambda expression is not counted
				}

				static void Execute(WfLexicalScope* scope, Ptr<WfExpression> expression, SortedList<vint>& names)
				{
					SearchOrderedNameVisitor visitor(scope, names);
					expression->Accept(&visitor);
				}
			};

			void SearchOrderedName(WfLexicalScope* scope, Ptr<WfExpression> expression, collections::SortedList<vint>& names)
			{
				SearchOrderedNameVisitor::Execute(scope, expression, names);
			}
		}
	}
}