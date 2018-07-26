#include "WfCpp.h"

namespace vl
{
	namespace workflow
	{
		namespace cppcodegen
		{
			using namespace collections;

#define ASSIGN_INDEX_KEY(INDEX_DECL, INDEX_KEY, STRING_KEY) \
			INDEX_DECL INDEX_KEY = globalDep.allTds.Keys().IndexOf(STRING_KEY); \
			CHECK_ERROR(INDEX_KEY != -1, L"WfCppConfig::AssignClassDeclsToFiles()#Internal error.") \

/***********************************************************************
WfCppConfig::GenerateGlobalDep
***********************************************************************/

			void WfCppConfig::ExpandClassDeclGroup(Ptr<WfClassDeclaration> parent, GlobalDep& globalDep)
			{
				vint index = classDecls.Keys().IndexOf(parent.Obj());
				if (index == -1) return;

				FOREACH(Ptr<WfClassDeclaration>, subDecl, classDecls.GetByIndex(index))
				{
					ExpandClassDeclGroup(subDecl, globalDep);
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
							return globalDep.expandedClassDecls.Keys().IndexOf(indexKey);
						})
						.Where([](vint index)
						{
							return index != -1;
						})
						.SelectMany([&](vint index)
						{
							return From(globalDep.expandedClassDecls.GetByIndex(index));
						});

				vint indexKey = -1;
				if (parent)
				{
					auto stringKey = manager->declarationTypes[parent.Obj()]->GetTypeName();
					ASSIGN_INDEX_KEY(, indexKey, stringKey);
				}
				FOREACH(vint, subDecl, directChildren.Concat(indirectChildren))
				{
					globalDep.expandedClassDecls.Add(indexKey, subDecl);
				}
			}

			void WfCppConfig::GenerateClassDependencies(GlobalDep& globalDep)
			{
				FOREACH_INDEXER(ITypeDescriptor*, td, tdIndex, globalDep.allTds.Values())
				{
					vint count = td->GetBaseTypeDescriptorCount();
					for (vint i = 0; i < count; i++)
					{
						auto baseTd = td->GetBaseTypeDescriptor(i);
						vint baseTdIndex = globalDep.allTds.Keys().IndexOf(baseTd->GetTypeName());
						if (baseTdIndex != -1)
						{
							globalDep.dependencies.Add(tdIndex, baseTdIndex);
						}
					}
				}
			}

			void WfCppConfig::GenerateGlobalDep(GlobalDep& globalDep)
			{
				FOREACH_INDEXER(ITypeDescriptor*, td, index, tdDecls.Keys())
				{
					if (tdDecls.Values()[index].Cast<WfClassDeclaration>())
					{
						globalDep.allTds.Add(td->GetTypeName(), td);
					}
				}

				ExpandClassDeclGroup(nullptr, globalDep);
				GenerateClassDependencies(globalDep);
			}

/***********************************************************************
WfCppConfig::GenerateClassLevelDep
***********************************************************************/

			void WfCppConfig::CollectExpandedDepGroup(vint parentIndexKey, GlobalDep& globalDep, ClassLevelDep& classLevelDep)
			{
				const auto& items = globalDep.expandedClassDecls[parentIndexKey];
				FOREACH(vint, subDecl, items)
				{
					vint index = globalDep.dependencies.Keys().IndexOf(subDecl);
					if (index != -1)
					{
						FOREACH(vint, dep, globalDep.dependencies.GetByIndex(index))
						{
							if (items.Contains(dep))
							{
								classLevelDep.depGroup.Add(subDecl, dep);
							}
						}
					}
				}
			}

			void WfCppConfig::CollectExpandedSubClass(vint subDeclIndexKey, GlobalDep& globalDep, ClassLevelDep& classLevelDep)
			{
				classLevelDep.subClass.Add(subDeclIndexKey, subDeclIndexKey);

				vint index = globalDep.expandedClassDecls.Keys().IndexOf(subDeclIndexKey);
				if (index != -1)
				{
					FOREACH(vint, expandDecl, globalDep.expandedClassDecls.GetByIndex(index))
					{
						classLevelDep.subClass.Add(expandDecl, subDeclIndexKey);
					}
				}
			}

			void WfCppConfig::GenerateClassLevelDep(Ptr<WfClassDeclaration> parent, GlobalDep& globalDep, ClassLevelDep& classLevelDep)
			{
				classLevelDep.parentClass = parent;
				if (parent)
				{
					auto parentStringKey = manager->declarationTypes[parent.Obj()]->GetTypeName();
					ASSIGN_INDEX_KEY(, classLevelDep.parentIndexKey, parentStringKey);
				}
				const auto& items = globalDep.expandedClassDecls[classLevelDep.parentIndexKey];

				// for any specified class (or top level if nullptr)
				// find all direct and indirect internal classes
				// copy their dependencies, and generate sub classes by grouping them using the second level of classes
				CollectExpandedDepGroup(classLevelDep.parentIndexKey, globalDep, classLevelDep);
				FOREACH(Ptr<WfClassDeclaration>, subDecl, classDecls.Get(parent.Obj()))
				{
					auto subDeclStringKey = manager->declarationTypes[subDecl.Obj()]->GetTypeName();
					ASSIGN_INDEX_KEY(auto, subDeclIndexKey, subDeclStringKey);
					CollectExpandedSubClass(subDeclIndexKey, globalDep, classLevelDep);
				}
			}

/***********************************************************************
WfCppConfig::Collect
***********************************************************************/

			void WfCppConfig::SortClassDecls(GlobalDep& globalDep)
			{
				// sort top level classes and all internal classes inside any classes
				for (vint i = classDecls.Count() - 1; i >= 0; i--)
				{
					// for any specified class (or top level if nullptr)
					// find all direct and indirect internal classes
					// copy their dependencies, and generate sub classes by grouping them using the second level of classes
					auto parent = classDecls.Keys()[i];
					ClassLevelDep classLevelDep;
					GenerateClassLevelDep(parent, globalDep, classLevelDep);

					// sort them
					PartialOrderingProcessor pop;
					const auto& items = globalDep.expandedClassDecls[classLevelDep.parentIndexKey];
					pop.InitWithSubClass(items, classLevelDep.depGroup, classLevelDep.subClass);
					pop.Sort();

					// using the partial ordering result to sort the corresponding value list in classDecls
					{
						auto& values = const_cast<List<Ptr<WfClassDeclaration>>&>(classDecls.GetByIndex(i));
						for (vint j = 0; j < pop.components.Count(); j++)
						{
							auto& component = pop.components[j];
							CHECK_ERROR(component.nodeCount == 1, L"WfCppConfig::AssignClassDeclsToFiles()#Future error: Unexpected circle dependency found.");

							auto& node = pop.nodes[component.firstNode[0]];
							auto subDeclIndexKey = classLevelDep.subClass[items[node.firstSubClassItem[0]]];
							auto subDecl = tdDecls[globalDep.allTds.Values()[subDeclIndexKey]].Cast<WfClassDeclaration>();
							values[j] = subDecl;
						}
					}
				}
			}

			void WfCppConfig::GenerateFileClassMaps(GlobalDep& globalDep)
			{
				if (classDecls.Keys().Contains(nullptr))
				{
					// get dependency information for top level classes
					ClassLevelDep classLevelDep;
					GenerateClassLevelDep(nullptr, globalDep, classLevelDep);
					const auto& items = globalDep.expandedClassDecls[classLevelDep.parentIndexKey];

					PartialOrderingProcessor popSubClass;
					Array<vint> customFirstItems;		// popSubClass.nodes's index
					Array<vint> nonCustomFirstItems;	// popSubClass.nodes's index
					Array<bool> isCustomItems;			// popSubClass.nodes's index to boolean (true means custom item)
					Group<vint, vint> subClassDepGroup;	// popSubClass.nodes's index to index
					{
						// calculate dependency for top level classes
						// globalDep.allTds.Keys()'s index to index
						Group<vint, vint> depGroup;
						{
							PartialOrderingProcessor pop;
							pop.InitWithSubClass(items, classLevelDep.depGroup, classLevelDep.subClass);
							for (vint i = 0; i < pop.nodes.Count(); i++)
							{
								auto& keyNode = pop.nodes[i];
								vint keyIndex = classLevelDep.subClass[keyNode.firstSubClassItem[0]];
								for (vint j = 0; j < keyNode.ins->Count(); j++)
								{
									auto& valueNode = pop.nodes[keyNode.ins->Get(j)];
									vint valueIndex = classLevelDep.subClass[valueNode.firstSubClassItem[0]];
									depGroup.Add(keyIndex, valueIndex);
								}
							}
						}

						// generate sub class using @cpp:File
						// globalDep.allTds.Keys()'s index to file name
						Dictionary<vint, WString> subClass;
						for (vint i = 0; i < customFilesClasses.Count(); i++)
						{
							WString key = customFilesClasses.Keys()[i];
							if (key != L"")
							{
								FOREACH(Ptr<WfClassDeclaration>, decl, customFilesClasses.GetByIndex(i))
								{
									auto stringKey = manager->declarationTypes[decl.Obj()]->GetTypeName();
									ASSIGN_INDEX_KEY(auto, indexKey, stringKey);
									subClass.Add(indexKey, key);
								}
							}
						}

						// check if all components contains either all classes of the same @cpp:File or a single non-@cpp:File class
						popSubClass.InitWithSubClass(items, depGroup, subClass);
						popSubClass.Sort();

						for (vint i = 0; i < popSubClass.components.Count(); i++)
						{
							auto& component = popSubClass.components[i];
							CHECK_ERROR(component.nodeCount == 1, L"WfCppConfig::AssignClassDeclsToFiles()#Future error: Unexpected circle dependency found.");
						}

						// generate two item list, one have all @cpp:File classes put in front, one have all non-@cpp:File classes put in front
						// popSubClass.nodes's index
						{
							List<vint> customItems;
							List<vint> nonCustomItems;

							// categorize popSubClass.nodes's index to customItems and nonCustomItems
							for (vint i = 0; i < popSubClass.nodes.Count(); i++)
							{
								auto& node = popSubClass.nodes[i];
								if (subClass.Keys().Contains(node.firstSubClassItem[0]))
								{
									customItems.Add(i);
								}
								else
								{
									nonCustomItems.Add(i);
								}
							}

							// sort items using allTds.Keys()'s index
							auto SortNodes = [&](List<vint>& items)
							{
								if (items.Count() > 0)
								{
									Sort<vint>(&items[0], items.Count(), [&](vint a, vint b)
									{
										auto& nodeA = popSubClass.nodes[a];
										auto& nodeB = popSubClass.nodes[b];
										vint indexA = From(nodeA.firstSubClassItem, nodeA.firstSubClassItem + nodeA.subClassItemCount).Min();
										vint indexB = From(nodeB.firstSubClassItem, nodeB.firstSubClassItem + nodeB.subClassItemCount).Min();
										return indexA - indexB;
									});
								}
							};
							SortNodes(customItems);
							SortNodes(nonCustomItems);

							// prepare customFirstItems, nonCustomFirstItems and isCustomItems
							isCustomItems.Resize(customItems.Count() + nonCustomItems.Count());
							for (vint i = 0; i < customItems.Count(); i++)
							{
								isCustomItems[customItems[i]] = true;
							}
							for (vint i = 0; i < nonCustomItems.Count(); i++)
							{
								isCustomItems[nonCustomItems[i]] = false;
							}

							CopyFrom(customFirstItems, customItems);
							CopyFrom(nonCustomFirstItems, nonCustomItems);

							CopyFrom(nonCustomFirstItems, customItems, true);
							CopyFrom(customFirstItems, nonCustomItems, true);
						}
					}

					// copy popSubClass's sub class dependencies to subClassDepGroup
					for (vint i = 0; i < popSubClass.nodes.Count(); i++)
					{
						auto& node = popSubClass.nodes[i];
						for (vint j = 0; j < node.ins->Count(); j++)
						{
							subClassDepGroup.Add(i, node.ins->Get(j));
						}
					}

					// sort using inputs of two orders
					PartialOrderingProcessor popCustomFirst;
					popCustomFirst.InitWithGroup(customFirstItems, subClassDepGroup);
					popCustomFirst.Sort();

					PartialOrderingProcessor popNonCustomFirst;
					popNonCustomFirst.InitWithGroup(nonCustomFirstItems, subClassDepGroup);
					popNonCustomFirst.Sort();

					CHECK_ERROR(popCustomFirst.components.Count() == customFirstItems.Count(), L"WfCppConfig::AssignClassDeclsToFiles()#Future error: Unexpected circle dependency found.");
					CHECK_ERROR(popNonCustomFirst.components.Count() == nonCustomFirstItems.Count(), L"WfCppConfig::AssignClassDeclsToFiles()#Future error: Unexpected circle dependency found.");
					CHECK_ERROR(popCustomFirst.components.Count() == popNonCustomFirst.components.Count(), L"WfCppConfig::AssignClassDeclsToFiles()#Future error: Unexpected circle dependency found.");

					// translate popCustomFirst's sorting result
					// popSubClass.nodes's index
					Array<vint> customFirstOrder(popCustomFirst.components.Count());
					for (vint i = 0; i < popCustomFirst.components.Count(); i++)
					{
						auto& component = popCustomFirst.components[i];
						customFirstOrder[i] = customFirstItems[component.firstNode[0]];
					}

					// translate popNonCustomFirst's sorting result
					// popSubClass.nodes's index
					Array<vint> nonCustomFirstOrder(popNonCustomFirst.components.Count());
					for (vint i = 0; i < popNonCustomFirst.components.Count(); i++)
					{
						auto& component = popNonCustomFirst.components[i];
						nonCustomFirstOrder[i] = nonCustomFirstItems[component.firstNode[0]];
					}

					// dispatch non-@cpp:File classes to non-@cpp:File headers
					vint currentHeaderIndex = 0;
					Array<bool> visited(customFirstOrder.Count());
					for (vint i = 0; i < visited.Count(); i++)
					{
						visited[i] = false;
					}

					while (nonCustomFirstOrder.Count() > 0 && customFirstOrder.Count() > 0)
					{

					}
				}
			}

			void WfCppConfig::SortFileClassMaps(GlobalDep& globalDep)
			{
				// sort customFilesClasses and headerFilesClasses according to classDecls[nullptr]
				if (classDecls.Keys().Contains(nullptr))
				{
					SortedList<Ptr<WfClassDeclaration>> ordered;
					CopyFrom(ordered, classDecls[nullptr]);

					for (vint i = 0; i < customFilesClasses.Count(); i++)
					{
						auto& values = const_cast<List<Ptr<WfClassDeclaration>>&>(customFilesClasses.GetByIndex(i));
						Sort<Ptr<WfClassDeclaration>>(&values[0], values.Count(), [&](Ptr<WfClassDeclaration> a, Ptr<WfClassDeclaration> b)
						{
							vint aIndex = ordered.IndexOf(a.Obj());
							vint bIndex = ordered.IndexOf(b.Obj());
							return aIndex - bIndex;
						});
					}

					for (vint i = 0; i < headerFilesClasses.Count(); i++)
					{
						auto& values = const_cast<List<Ptr<WfClassDeclaration>>&>(headerFilesClasses.GetByIndex(i));
						Sort<Ptr<WfClassDeclaration>>(&values[0], values.Count(), [&](Ptr<WfClassDeclaration> a, Ptr<WfClassDeclaration> b)
						{
							vint aIndex = ordered.IndexOf(a.Obj());
							vint bIndex = ordered.IndexOf(b.Obj());
							return aIndex - bIndex;
						});
					}
				}
			}

			void WfCppConfig::AssignClassDeclsToFiles()
			{
				GlobalDep globalDep;
				GenerateGlobalDep(globalDep);

				if (manager->errors.Count() == 0) SortClassDecls(globalDep);
				if (manager->errors.Count() == 0) GenerateFileClassMaps(globalDep);
				if (manager->errors.Count() == 0) SortFileClassMaps(globalDep);
			}

#undef ASSIGN_INDEX_KEY
		}
	}
}