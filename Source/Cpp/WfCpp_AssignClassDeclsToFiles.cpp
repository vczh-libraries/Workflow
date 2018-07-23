#include "WfCpp.h"

namespace vl
{
	namespace workflow
	{
		namespace cppcodegen
		{
			using namespace collections;

/***********************************************************************
WfCppConfig::ExpandClassDeclGroup
***********************************************************************/

			void WfCppConfig::ExpandClassDeclGroup(Ptr<WfClassDeclaration> parent, collections::Group<WString, WString>& expandedClassDecls)
			{
				vint index = classDecls.Keys().IndexOf(parent.Obj());
				if (index == -1) return;

				FOREACH(Ptr<WfClassDeclaration>, subDecl, classDecls.GetByIndex(index))
				{
					ExpandClassDeclGroup(subDecl, expandedClassDecls);
				}

				auto directChildren =
					From(classDecls.GetByIndex(index))
						.Select([&](Ptr<WfClassDeclaration> decl)
						{
							return manager->declarationTypes[decl.Obj()]->GetTypeName();
						});

				auto indirectChildren =
					From(classDecls.GetByIndex(index))
						.Select([&](Ptr<WfClassDeclaration> subDecl)
						{
							auto key = manager->declarationTypes[subDecl.Obj()]->GetTypeName();
							return expandedClassDecls.Keys().IndexOf(key);
						})
						.Where([](vint index)
						{
							return index != -1;
						})
						.SelectMany([&](vint index)
						{
							return From(expandedClassDecls.GetByIndex(index));
						});

				WString key;
				if (parent)
				{
					key = manager->declarationTypes[parent.Obj()]->GetTypeName();
				}
				FOREACH(WString, subDecl, directChildren.Concat(indirectChildren))
				{
					expandedClassDecls.Add(key, subDecl);
				}
			}

/***********************************************************************
WfCppConfig::GenerateClassDependencies
***********************************************************************/

			void WfCppConfig::GenerateClassDependencies(collections::Dictionary<WString, ITypeDescriptor*>& allTds, collections::Group<WString, WString>& dependencies)
			{
				FOREACH_INDEXER(ITypeDescriptor*, td, tdIndex, allTds.Values())
				{
					vint count = td->GetBaseTypeDescriptorCount();
					for (vint i = 0; i < count; i++)
					{
						auto baseTd = td->GetBaseTypeDescriptor(i);
						vint index = allTds.Keys().IndexOf(baseTd->GetTypeName());
						if (index != -1)
						{
							dependencies.Add(td->GetTypeName(), baseTd->GetTypeName());
						}
					}
				}
			}

/***********************************************************************
WfCppConfig::Collect
***********************************************************************/

			void WfCppConfig::AssignClassDeclsToFiles()
			{
				Dictionary<WString, ITypeDescriptor*> allTds;
				FOREACH_INDEXER(ITypeDescriptor*, td, index, tdDecls.Keys())
				{
					if (tdDecls.Values()[index].Cast<WfClassDeclaration>())
					{
						allTds.Add(td->GetTypeName(), td);
					}
				}

				Group<WString, WString> expandedClassDecls;
				Group<WString, WString> dependencies;
				ExpandClassDeclGroup(nullptr, expandedClassDecls);
				GenerateClassDependencies(allTds, dependencies);

				for (vint i = classDecls.Count() - 1; i >= 0; i--)
				{
					auto parent = classDecls.Keys()[i];
					auto parentKey = manager->declarationTypes[parent.Obj()]->GetTypeName();
					const auto& items = expandedClassDecls[parentKey];
					Group<WString, WString> depGroup;
					Dictionary<WString, WString> subClass;

					FOREACH(WString, subDecl, items)
					{
						vint index = dependencies.Keys().IndexOf(subDecl);
						if (index != -1)
						{
							FOREACH(WString, dep, dependencies.GetByIndex(index))
							{
								if (items.Contains(dep))
								{
									depGroup.Add(subDecl, dep);
								}
							}
						}
					}

					FOREACH(Ptr<WfClassDeclaration>, subDecl, classDecls.GetByIndex(i))
					{
						auto subDeclKey = manager->declarationTypes[subDecl.Obj()]->GetTypeName();
						subClass.Add(subDeclKey, subDeclKey);

						vint index = expandedClassDecls.Keys().IndexOf(subDeclKey);
						if (index != -1)
						{
							FOREACH(WString, expandDecl, expandedClassDecls.GetByIndex(index))
							{
								subClass.Add(expandDecl, subDeclKey);
							}
						}
					}

					PartialOrderingProcessor pop;
					pop.InitWithSubClass(items, depGroup, subClass);
					pop.Sort();

					classDecls.Remove(parent.Obj());
					for (vint j = 0; j < pop.components.Count(); j++)
					{
						auto& component = pop.components[j];
						CHECK_ERROR(component.nodeCount == 1, L"WfCppConfig::AssignClassDeclsToFiles()#Future error: Unexpected circle dependency found.");
						
						auto& node = pop.nodes[component.firstNode[0]];
						auto subDeclKey = subClass[items[node.firstSubClassItem[0]]];
						auto subDecl = tdDecls[allTds[subDeclKey]].Cast<WfClassDeclaration>();
						classDecls.Add(parent, subDecl);
					}
				}
			}
		}
	}
}