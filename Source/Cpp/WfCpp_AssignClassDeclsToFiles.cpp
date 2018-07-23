#include "WfCpp.h"

namespace vl
{
	namespace workflow
	{
		namespace cppcodegen
		{
			using namespace collections;

#define ASSIGN_INDEX_KEY(INDEX_DECL, INDEX_KEY, STRING_KEY) \
			INDEX_DECL INDEX_KEY = allTds.Keys().IndexOf(STRING_KEY); \
			CHECK_ERROR(INDEX_KEY != -1, L"WfCppConfig::AssignClassDeclsToFiles()#Internal error.") \

/***********************************************************************
WfCppConfig::ExpandClassDeclGroup
***********************************************************************/

			void WfCppConfig::ExpandClassDeclGroup(Ptr<WfClassDeclaration> parent, collections::Dictionary<WString, ITypeDescriptor*>& allTds, collections::Group<vint, vint>& expandedClassDecls)
			{
				vint index = classDecls.Keys().IndexOf(parent.Obj());
				if (index == -1) return;

				FOREACH(Ptr<WfClassDeclaration>, subDecl, classDecls.GetByIndex(index))
				{
					ExpandClassDeclGroup(subDecl, allTds, expandedClassDecls);
				}

				auto directChildren =
					From(classDecls.GetByIndex(index))
						.Select([&](Ptr<WfClassDeclaration> decl)
						{
							auto stringKey = manager->declarationTypes[decl.Obj()]->GetTypeName();
							ASSIGN_INDEX_KEY(auto, indexKey, stringKey);
							return indexKey;
						});

				auto indirectChildren =
					From(classDecls.GetByIndex(index))
						.Select([&](Ptr<WfClassDeclaration> subDecl)
						{
							auto stringKey = manager->declarationTypes[subDecl.Obj()]->GetTypeName();
							ASSIGN_INDEX_KEY(auto, indexKey, stringKey);
							return expandedClassDecls.Keys().IndexOf(indexKey);
						})
						.Where([](vint index)
						{
							return index != -1;
						})
						.SelectMany([&](vint index)
						{
							return From(expandedClassDecls.GetByIndex(index));
						});

				vint indexKey = -1;
				if (parent)
				{
					auto stringKey = manager->declarationTypes[parent.Obj()]->GetTypeName();
					ASSIGN_INDEX_KEY(, indexKey, stringKey);
				}
				FOREACH(vint, subDecl, directChildren.Concat(indirectChildren))
				{
					expandedClassDecls.Add(indexKey, subDecl);
				}
			}

/***********************************************************************
WfCppConfig::GenerateClassDependencies
***********************************************************************/

			void WfCppConfig::GenerateClassDependencies(collections::Dictionary<WString, ITypeDescriptor*>& allTds, collections::Group<vint, vint>& dependencies)
			{
				FOREACH_INDEXER(ITypeDescriptor*, td, tdIndex, allTds.Values())
				{
					vint count = td->GetBaseTypeDescriptorCount();
					for (vint i = 0; i < count; i++)
					{
						auto baseTd = td->GetBaseTypeDescriptor(i);
						vint baseTdIndex = allTds.Keys().IndexOf(baseTd->GetTypeName());
						if (baseTdIndex != -1)
						{
							dependencies.Add(tdIndex, baseTdIndex);
						}
					}
				}
			}

/***********************************************************************
WfCppConfig::Collect
***********************************************************************/

			void WfCppConfig::AssignClassDeclsToFiles()
			{
				// Sort all classes by full name
				Dictionary<WString, ITypeDescriptor*> allTds;
				FOREACH_INDEXER(ITypeDescriptor*, td, index, tdDecls.Keys())
				{
					if (tdDecls.Values()[index].Cast<WfClassDeclaration>())
					{
						allTds.Add(td->GetTypeName(), td);
					}
				}

				// Calculate dependencies by indices of keys in allTds
				Group<vint, vint> expandedClassDecls;
				Group<vint, vint> dependencies;
				ExpandClassDeclGroup(nullptr, allTds, expandedClassDecls);
				GenerateClassDependencies(allTds, dependencies);

				for (vint i = classDecls.Count() - 1; i >= 0; i--)
				{
					auto parent = classDecls.Keys()[i];
					vint parentIndexKey;
					if (parent)
					{
						auto parentStringKey = manager->declarationTypes[parent.Obj()]->GetTypeName();
						ASSIGN_INDEX_KEY(, parentIndexKey, parentStringKey);
					}
					const auto& items = expandedClassDecls[parentIndexKey];
					Group<vint, vint> depGroup;
					Dictionary<vint, vint> subClass;

					FOREACH(vint, subDecl, items)
					{
						vint index = dependencies.Keys().IndexOf(subDecl);
						if (index != -1)
						{
							FOREACH(vint, dep, dependencies.GetByIndex(index))
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
						auto subDeclStringKey = manager->declarationTypes[subDecl.Obj()]->GetTypeName();
						ASSIGN_INDEX_KEY(auto, subDeclIndexKey, subDeclStringKey);
						subClass.Add(subDeclIndexKey, subDeclIndexKey);

						vint index = expandedClassDecls.Keys().IndexOf(subDeclIndexKey);
						if (index != -1)
						{
							FOREACH(vint, expandDecl, expandedClassDecls.GetByIndex(index))
							{
								subClass.Add(expandDecl, subDeclIndexKey);
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

#undef ASSIGN_INDEX_KEY
		}
	}
}