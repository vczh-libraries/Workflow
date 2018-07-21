#include "WfAnalyzer.h"

namespace vl
{
	namespace workflow
	{
		namespace analyzer
		{
			using namespace collections;

/***********************************************************************
CheckScopes_SymbolType
***********************************************************************/

			bool CheckScopes_SymbolType(WfLexicalScopeManager* manager)
			{
				SortedList<WfLexicalScope*> analyzedScopes;
				vint errorCount = manager->errors.Count();
				FOREACH(Ptr<WfLexicalScope>, scope, manager->nodeScopes.Values())
				{
					if (!analyzedScopes.Contains(scope.Obj()))
					{
						analyzedScopes.Add(scope.Obj());

						for (vint i = 0; i < scope->symbols.Count(); i++)
						{
							FOREACH(Ptr<WfLexicalSymbol>, symbol, scope->symbols.GetByIndex(i))
							{
								if (symbol->type)
								{
									vint currentErrorCount = manager->errors.Count();
									symbol->typeInfo = CreateTypeInfoFromType(scope.Obj(), symbol->type);
									if (symbol->creatorNode)
									{
										for (vint i = currentErrorCount; i < manager->errors.Count(); i++)
										{
											manager->errors[i]->parsingTree = symbol->creatorNode.Obj();
										}
									}
								}
							}
						}
					}
				}
				return errorCount == manager->errors.Count();
			}
		}
	}
}
