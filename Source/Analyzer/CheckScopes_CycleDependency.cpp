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
				WfLexicalScopeManager*					manager;
				SortedList<ITypeDescriptor*>			checkedInterfaces;
				SortedList<ITypeDescriptor*>			traversedInterfaces;

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

				void CheckRecursiveStruct(WfStructDeclaration* node, ITypeDescriptor* target, ITypeDescriptor* current, List<WString>& path)
				{
					if (target == current)
					{
						manager->errors.Add(WfErrors::StructRecursivelyIncludeItself(
							node,
							From(path).Aggregate([](const WString& a, const WString& b) {return a + L"::" + b; })
							));
						return;
					}

					if (current == nullptr)
					{
						current = target;
					}

					vint count = current->GetPropertyCount();
					for (vint i = 0; i < count; i++)
					{
						auto prop = current->GetProperty(i);
						auto propType = prop->GetReturn();
						auto propTd = prop->GetReturn()->GetTypeDescriptor();
						if (propType->GetDecorator() == ITypeInfo::TypeDescriptor && propTd->GetTypeDescriptorFlags() == TypeDescriptorFlags::Struct)
						{
							vint index = path.Add(prop->GetName());
							CheckRecursiveStruct(node, target, propTd, path);
							path.RemoveAt(index);
						}
					}
				}

				void Visit(WfStructDeclaration* node)override
				{
					auto scope = manager->nodeScopes[node];
					auto td = manager->declarationTypes[node].Cast<WfStruct>();
					List<WString> path;
					path.Add(td->GetTypeName());
					CheckRecursiveStruct(node, td.Obj(), nullptr, path);
				}

				void CheckDuplicatedBaseClass(WfClassDeclaration* node, ITypeDescriptor* td)
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

				void CheckDuplicatedBaseInterface(WfClassDeclaration* node, ITypeDescriptor* td)
				{
					if (traversedInterfaces.Contains(td))
					{
						manager->errors.Add(WfErrors::DuplicatedBaseInterface(node, td));
					}
					else
					{
						if (checkedInterfaces.Contains(td))
						{
							return;
						}
						checkedInterfaces.Add(td);

						vint index = traversedInterfaces.Add(td);
						vint count = td->GetBaseTypeDescriptorCount();
						for (vint i = 0; i < count; i++)
						{
							CheckDuplicatedBaseInterface(node, td->GetBaseTypeDescriptor(i));
						}
						traversedInterfaces.RemoveAt(index);
					}
				}

				void Visit(WfClassDeclaration* node)override
				{
					auto scope = manager->nodeScopes[node];
					auto td = manager->declarationTypes[node].Obj();

					FOREACH(Ptr<WfType>, baseType, node->baseTypes)
					{
						if (auto scopeName = GetScopeNameFromReferenceType(scope->parentScope.Obj(), baseType))
						{
							if (auto baseTd = scopeName->typeDescriptor)
							{
								bool isClass = baseTd->GetTypeDescriptorFlags() == TypeDescriptorFlags::Class;
								bool isInterface = baseTd->GetTypeDescriptorFlags() == TypeDescriptorFlags::Interface;

								switch (node->kind)
								{
								case WfClassKind::Class:
									{
										if (!isClass || !baseTd->IsAggregatable())
										{
											if (!dynamic_cast<WfClass*>(baseTd))
											{
												manager->errors.Add(WfErrors::WrongBaseTypeOfClass(node, baseTd));
											}
										}
									}
									break;
								case WfClassKind::Interface:
									{
										if (!isInterface)
										{
											manager->errors.Add(WfErrors::WrongBaseTypeOfInterface(node, baseTd));
										}
									}
									break;
								}
							}
						}
					}

					if (node->kind == WfClassKind::Class)
					{
						CheckDuplicatedBaseClass(node, td);
					}
					else
					{
						CheckDuplicatedBaseInterface(node, td);
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
				return errorCount == manager->errors.Count();
			}
		}
	}
}
