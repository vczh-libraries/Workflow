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
				vint errorCount = manager->errors.Count();
				for (auto scope : manager->nodeScopes.Values())
				{
					if (!manager->checkedScopes_SymbolType.Contains(scope.Obj()))
					{
						manager->checkedScopes_SymbolType.Add(scope.Obj());

						for (vint i = 0; i < scope->symbols.Count(); i++)
						{
							for (auto symbol : scope->symbols.GetByIndex(i))
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
