#include "WfAnalyzer.h"

namespace vl
{
	namespace workflow
	{
		namespace analyzer
		{
			using namespace collections;

/***********************************************************************
CheckScopes_DuplicatedSymbol
***********************************************************************/

			bool CheckScopes_DuplicatedSymbol(WfLexicalScopeManager* manager)
			{
				vint errorCount = manager->errors.Count();
				for (auto scope : manager->nodeScopes.Values())
				{
					if (!manager->checkedScopes_DuplicatedSymbol.Contains(scope.Obj()))
					{
						manager->checkedScopes_DuplicatedSymbol.Add(scope.Obj());

						for (vint i = 0; i < scope->symbols.Count(); i++)
						{
							const auto& symbols = scope->symbols.GetByIndex(i);
							if (symbols.Count() > 1)
							{
								if (!scope->ownerNode.Cast<WfModule>() && !scope->ownerNode.Cast<WfNamespaceDeclaration>())
								{
									if (symbols.Count() > 1)
									{
										for (auto symbol : From(symbols))
										{
											if (auto decl = symbol->creatorNode.Cast<WfDeclaration>())
											{
												if (!decl.Cast<WfFunctionDeclaration>())
												{
													manager->errors.Add(WfErrors::DuplicatedSymbol(decl.Obj(), symbol));
												}
											}
											else if (auto arg = symbol->creatorNode.Cast<WfFunctionArgument>())
											{
												manager->errors.Add(WfErrors::DuplicatedSymbol(arg.Obj(), symbol));
											}
											else if (auto stat = symbol->creatorNode.Cast<WfStatement>())
											{
												manager->errors.Add(WfErrors::DuplicatedSymbol(stat.Obj(), symbol));
											}
											else if (auto expr = symbol->creatorNode.Cast<WfExpression>())
											{
												manager->errors.Add(WfErrors::DuplicatedSymbol(expr.Obj(), symbol));
											}
											else if (auto input = symbol->creatorNode.Cast<WfStateInput>())
											{
												if (symbols.Count() == 2)
												{
													// Ignore the generated function from the state input
													auto methodSymbol = symbols[1 - symbols.IndexOf(symbol.Obj())];
													auto funcDecl = methodSymbol->creatorNode.Cast<WfFunctionDeclaration>();
													vint index = manager->declarationMemberInfos.Keys().IndexOf(funcDecl.Obj());
													if (index != -1)
													{
														auto methodInfo = manager->declarationMemberInfos.Values()[index];
														if (manager->stateInputMethods[input.Obj()].Obj() == methodInfo.Obj())
														{
															goto NO_ERROR;
														}
													}
												}
												manager->errors.Add(WfErrors::DuplicatedSymbol(input.Obj(), symbol));
											NO_ERROR:;
											}
											else if (auto state = symbol->creatorNode.Cast<WfStateDeclaration>())
											{
												manager->errors.Add(WfErrors::DuplicatedSymbol(state.Obj(), symbol));
											}
											else if (auto sarg = symbol->creatorNode.Cast<WfStateSwitchArgument>())
											{
												manager->errors.Add(WfErrors::DuplicatedSymbol(sarg.Obj(), symbol));
											}
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
