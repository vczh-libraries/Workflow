#include "WfAnalyzer.h"

namespace vl
{
	namespace workflow
	{
		namespace analyzer
		{
			using namespace collections;
			using namespace reflection;
			using namespace reflection::description;
			using namespace typeimpl;

/***********************************************************************
CheckBaseClass
***********************************************************************/

			class CheckCycleDependencyDeclarationVisitor
				: public empty_visitor::DeclarationVisitor
				, public empty_visitor::VirtualCseDeclarationVisitor
			{
			public:
				WfLexicalScopeManager*							manager;
				ITypeDescriptor*								tdDescriptableObject = description::GetTypeDescriptor<DescriptableObject>();
				ITypeDescriptor*								tdIDescriptable = description::GetTypeDescriptor<IDescriptable>();
				vint											step = 0;
				Dictionary<ITypeDescriptor*, WfDeclaration*>	depItems;
				Group<ITypeDescriptor*, ITypeDescriptor*>		depGroup;

				CheckCycleDependencyDeclarationVisitor(WfLexicalScopeManager* _manager)
					:manager(_manager)
				{
				}

				void Dispatch(WfVirtualCfeDeclaration* node)override
				{
					for (auto decl : node->expandedDeclarations)
					{
						decl->Accept(this);
					}
				}

				void Dispatch(WfVirtualCseDeclaration* node)override
				{
					node->Accept((WfVirtualCseDeclaration::IVisitor*)this);
				}

				void Visit(WfNamespaceDeclaration* node)override
				{
					for (auto decl : node->declarations)
					{
						decl->Accept(this);
					}
				}

				void Visit(WfStructDeclaration* node)override
				{
					auto scope = manager->nodeScopes[node];
					auto td = manager->declarationTypes[node].Obj();

					if (step == 1)
					{
						depItems.Add(td, node);
					}
					else if (step == 2)
					{
						vint count = td->GetPropertyCount();
						for (vint i = 0; i < count; i++)
						{
							auto propType = td->GetProperty(i)->GetReturn();
							if (propType->GetDecorator() == ITypeInfo::TypeDescriptor)
							{
								auto propTd = propType->GetTypeDescriptor();
								if (!depItems.Keys().Contains(propTd)) continue;

								if (td == propTd)
								{
									List<ITypeDescriptor*> tds;
									tds.Add(td);
									manager->errors.Add(WfErrors::StructRecursivelyIncludeItself(node, tds));
								}
								else if (propTd->GetTypeDescriptorFlags() == TypeDescriptorFlags::Struct)
								{
									if (!depGroup.Contains(td, propTd))
									{
										depGroup.Add(td, propTd);
									}
								}
							}
						}
					}
				}

				void Visit(WfClassDeclaration* node)override
				{
					auto scope = manager->nodeScopes[node];
					auto td = manager->declarationTypes[node].Obj();

					if (step == 1)
					{
						depItems.Add(td, node);
					}
					else if (step == 2)
					{
						vint count = td->GetBaseTypeDescriptorCount();
						for (vint i = 0; i < count; i++)
						{
							auto baseTd = td->GetBaseTypeDescriptor(i);

							bool isClass = baseTd->GetTypeDescriptorFlags() == TypeDescriptorFlags::Class;
							bool isInterface = baseTd->GetTypeDescriptorFlags() == TypeDescriptorFlags::Interface;

							switch (node->kind)
							{
							case WfClassKind::Class:
								{
									if (!isClass || !baseTd->IsAggregatable())
									{
										if (!dynamic_cast<WfClass*>(baseTd) && baseTd != tdDescriptableObject)
										{
											manager->errors.Add(WfErrors::WrongBaseTypeOfClass(node, baseTd));
										}
									}
								}
								break;
							case WfClassKind::Interface:
								{
									if (!isInterface && baseTd != tdIDescriptable)
									{
										manager->errors.Add(WfErrors::WrongBaseTypeOfInterface(node, baseTd));
									}
								}
								break;
							default:;
							}

							if (!depItems.Keys().Contains(baseTd)) continue;

							if (baseTd == td)
							{
								List<ITypeDescriptor*> tds;
								tds.Add(td);
								if (isClass)
								{
									manager->errors.Add(WfErrors::ClassRecursiveInheritance(node, tds));
								}
								else if (isInterface)
								{
									manager->errors.Add(WfErrors::InterfaceRecursiveInheritance(node, tds));
								}
							}
							else if (baseTd->GetTypeDescriptorFlags() == td->GetTypeDescriptorFlags())
							{
								if (!depGroup.Contains(td, baseTd))
								{
									depGroup.Add(td, baseTd);
								}
							}
						}
					}

					for (auto memberDecl : node->declarations)
					{
						memberDecl->Accept(this);
					}
				}

				void Execute(vint _step)
				{
					step = _step;
					for (auto module : manager->GetModules())
					{
						for (auto declaration : module->declarations)
						{
							declaration->Accept(this);
						}
					}
				}

				void ExecuteCollectTypes()
				{
					Execute(1);
				}

				void ExecuteCollectDependencies()
				{
					Execute(2);
				}
			};

/***********************************************************************
CheckScopes_CycleDependency
***********************************************************************/

			void CheckScopes_DuplicatedBaseClass(WfLexicalScopeManager* manager, WfClassDeclaration* node, ITypeDescriptor* td)
			{
				List<ITypeDescriptor*> baseTypes;
				SortedList<ITypeDescriptor*> duplicatedTypes;
				baseTypes.Add(td);

				for (vint i = 0; i < baseTypes.Count(); i++)
				{
					auto currentTd = baseTypes[i];
					vint count = currentTd->GetBaseTypeDescriptorCount();
					for (vint j = 0; j < count; j++)
					{
						auto baseTd = currentTd->GetBaseTypeDescriptor(j);
						if (baseTd->GetTypeDescriptorFlags() == TypeDescriptorFlags::Class && baseTd != description::GetTypeDescriptor<DescriptableObject>())
						{
							if (baseTypes.Contains(baseTd))
							{
								if (!duplicatedTypes.Contains(baseTd))
								{
									duplicatedTypes.Add(baseTd);
									manager->errors.Add(WfErrors::DuplicatedBaseClass(node, baseTd));
								}
							}
							else
							{
								baseTypes.Add(baseTd);
							}
						}
					}
				}
			}

			bool CheckScopes_CycleDependency(WfLexicalScopeManager* manager)
			{
				vint errorCount = manager->errors.Count();
				CheckCycleDependencyDeclarationVisitor visitor(manager);
				visitor.ExecuteCollectTypes();
				visitor.ExecuteCollectDependencies();

				{
					PartialOrderingProcessor pop;
					pop.InitWithGroup(visitor.depItems.Keys(), visitor.depGroup);
					pop.Sort();

					for (vint i = 0; i < pop.components.Count(); i++)
					{
						auto& component = pop.components[i];
						if (component.nodeCount > 1)
						{
							List<ITypeDescriptor*> tds;
							CopyFrom(
								tds,
								From(component.firstNode, component.firstNode + component.nodeCount)
									.Select([&](vint nodeIndex)
									{
										return visitor.depItems.Keys()[nodeIndex];
									})
								);

							switch (tds[0]->GetTypeDescriptorFlags())
							{
							case TypeDescriptorFlags::Struct:
								manager->errors.Add(WfErrors::StructRecursivelyIncludeItself(dynamic_cast<WfStructDeclaration*>(visitor.depItems[tds[0]]), tds));
								break;
							case TypeDescriptorFlags::Class:
								manager->errors.Add(WfErrors::ClassRecursiveInheritance(dynamic_cast<WfClassDeclaration*>(visitor.depItems[tds[0]]), tds));
								break;
							case TypeDescriptorFlags::Interface:
								manager->errors.Add(WfErrors::InterfaceRecursiveInheritance(dynamic_cast<WfClassDeclaration*>(visitor.depItems[tds[0]]), tds));
								break;
							default:;
							}
						}
					}
				}

				if (errorCount == manager->errors.Count())
				{
					for (vint i = 0; i < visitor.depItems.Count(); i++)
					{
						auto key = visitor.depItems.Keys()[i];
						auto value = dynamic_cast<WfClassDeclaration*>(visitor.depItems.Values()[i]);
						if (value && value->kind == WfClassKind::Class)
						{
							CheckScopes_DuplicatedBaseClass(manager, value, key);
						}
					}
				}
				return errorCount == manager->errors.Count();
			}
		}
	}
}
