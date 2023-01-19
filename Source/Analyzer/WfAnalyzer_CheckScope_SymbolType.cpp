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
				for (auto scope : From(manager->nodeScopes)
					.OrderBy([](auto&& a, auto&& b)
					{
						auto rangeA = a.key->codeRange;
						auto rangeB = b.key->codeRange;
						if (rangeA.codeIndex != rangeB.codeIndex)
						{
							return rangeA.codeIndex - rangeB.codeIndex;
						}
						else
						{
							return glr::ParsingTextPos::Compare(rangeA.start, rangeB.start);
						}
					})
					.Select([](auto&& pair) { return pair.value; })
					)
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
									symbol->typeInfo = CreateTypeInfoFromType(scope.Obj(), symbol->type);
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
