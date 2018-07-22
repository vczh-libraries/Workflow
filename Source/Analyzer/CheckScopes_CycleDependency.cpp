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
				Dictionary<ITypeDescriptor*, WfDeclaration*>	depItems;
				Group<ITypeDescriptor*, ITypeDescriptor*>		depGroup;

				CheckCycleDependencyDeclarationVisitor(WfLexicalScopeManager* _manager)
					:manager(_manager)
				{
				}

				void Dispatch(WfVirtualCfeDeclaration* node)override
				{
					FOREACH(Ptr<WfDeclaration>, decl, node->expandedDeclarations)
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
					FOREACH(Ptr<WfDeclaration>, decl, node->declarations)
					{
						decl->Accept(this);
					}
				}

				void Visit(WfStructDeclaration* node)override
				{
					auto scope = manager->nodeScopes[node];
					auto td = manager->declarationTypes[node].Obj();
					depItems.Add(td, node);

					vint count = td->GetPropertyCount();
					for (vint i = 0; i < count; i++)
					{
						auto propTd = td->GetProperty(i)->GetReturn()->GetTypeDescriptor();
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

				void Visit(WfClassDeclaration* node)override
				{
					auto scope = manager->nodeScopes[node];
					auto td = manager->declarationTypes[node].Obj();
					depItems.Add(td, node);

					vint count = td->GetBaseTypeDescriptorCount();
					for (vint i = 0; i < count; i++)
					{
						auto baseTd = td->GetBaseTypeDescriptor(i);
						if (baseTd == td)
						{
							List<ITypeDescriptor*> tds;
							tds.Add(td);
							if (td->GetTypeDescriptorFlags() == TypeDescriptorFlags::Class)
							{
								manager->errors.Add(WfErrors::ClassRecursiveInheritance(node, tds));
							}
							else if (td->GetTypeDescriptorFlags() == TypeDescriptorFlags::Interface)
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

					FOREACH(Ptr<WfDeclaration>, memberDecl, node->declarations)
					{
						memberDecl->Accept(this);
					}
				}
			};

/***********************************************************************
CheckScopes_CycleDependency
***********************************************************************/

			void CheckScopes_DuplicatedBaseClass(WfLexicalScopeManager* manager, WfClassDeclaration* node, ITypeDescriptor* td)
			{
				List<ITypeDescriptor*> baseTypes;
				SortedList<ITypeDescriptor*> duplicatedTypes;

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
				FOREACH(Ptr<WfModule>, module, manager->GetModules())
				{
					FOREACH(Ptr<WfDeclaration>, declaration, module->declarations)
					{
						declaration->Accept(&visitor);
					}
				}

				for (vint i = visitor.depGroup.Count() - 1; i >= 0; i--)
				{
					auto key = visitor.depGroup.Keys()[i];
					const auto& values = visitor.depGroup.GetByIndex(i);
					for (vint j = values.Count() - 1; j >= 0; j--)
					{
						auto value = values[j];
						if (!visitor.depItems.Keys().Contains(value))
						{
							visitor.depGroup.Remove(key, value);
						}
					}
				}

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
