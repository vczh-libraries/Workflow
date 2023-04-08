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
					.Select([](auto&& pair) -> Pair<glr::ParsingAstBase*, Ptr<WfLexicalScope>> { return pair; })
					.OrderBy([](auto&& a, auto&& b)
					{
						auto rangeA = a.key->codeRange;
						auto rangeB = b.key->codeRange;

						std::strong_ordering
						result = rangeA.codeIndex <=> rangeB.codeIndex; if (result != 0) return result;
						result = rangeA.start <=> rangeB.start; if (result != 0) return result;
						return result;
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
