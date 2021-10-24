#include "WfAnalyzer.h"

namespace vl
{
	namespace workflow
	{
		namespace analyzer
		{
			using namespace collections;
			using namespace parsing;
			using namespace reflection::description;

/***********************************************************************
ValidateStructureContext
***********************************************************************/

			ValidateStructureContext::ValidateStructureContext()
				:currentBindExpression(0)
				, currentObserveExpression(0)
				, currentLoopStatement(0)
				, currentCatchStatement(0)
			{
			}

/***********************************************************************
ValidateStructure
***********************************************************************/

			void ValidateModuleStructure(WfLexicalScopeManager* manager, Ptr<WfModule> module)
			{
				for (auto path : module->paths)
				{
					for (auto [item, index] : indexed(path->items))
					{
						vint counter = 0;
						for (auto fragment : item->fragments)
						{
							if (fragment.Cast<WfModuleUsingWildCardFragment>())
							{
								if (index == path->items.Count() - 1)
								{
									counter++;
								}
								else
								{
									goto USING_PATH_INCORRECT;
								}
							}
						}

						if (index == path->items.Count() - 1 && counter != 1)
						{
							goto USING_PATH_INCORRECT;
						}
					}
					continue;
				USING_PATH_INCORRECT:
					manager->errors.Add(WfErrors::WrongUsingPathWildCard(path.Obj()));
				}
				for (vint i = 0; i < module->declarations.Count(); i++)
				{
					ValidateDeclarationStructure(manager, module->declarations[i]);
				}
			}
		}
	}
}
