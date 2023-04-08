#include "WfCpp.h"

namespace vl
{
	namespace workflow
	{
		namespace cppcodegen
		{
			using namespace analyzer;
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

				for (auto subDecl : classDecls.GetByIndex(index))
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
				for (auto subDecl : directChildren.Concat(indirectChildren))
				{
					globalDep.expandedClassDecls.Add(indexKey, subDecl);
				}
			}

			void WfCppConfig::GenerateClassDependencies(GlobalDep& globalDep)
			{
				for (auto [td, tdIndex] : indexed(globalDep.allTds.Values()))
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
				for (auto [td, index] : indexed(tdDecls.Keys()))
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
				for (auto subDecl : items)
				{
					vint index = globalDep.dependencies.Keys().IndexOf(subDecl);
					if (index != -1)
					{
						for (auto dep : globalDep.dependencies.GetByIndex(index))
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
					for (auto expandDecl : globalDep.expandedClassDecls.GetByIndex(index))
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
				for (auto subDecl : classDecls.Get(parent.Obj()))
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

							// check error
							if (component.nodeCount > 1)
							{
								List<ITypeDescriptor*> tds;
								for (vint k = 0; k < component.nodeCount; k++)
								{
									auto& node = pop.nodes[component.firstNode[k]];
									auto indexKey = classLevelDep.subClass[items[node.firstSubClassItem[0]]];
									tds.Add(globalDep.allTds.Values()[indexKey]);
								}

								SortLambda<ITypeDescriptor*>(&tds[0], tds.Count(), [](ITypeDescriptor* a, ITypeDescriptor* b)
								{
									return a->GetTypeName() <=> b->GetTypeName();
								});
								manager->errors.Add(WfErrors::CppUnableToDecideClassOrder(tdDecls[tds[0]].Cast<WfClassDeclaration>().Obj(), tds));
							}

							auto& node = pop.nodes[component.firstNode[0]];
							auto subDeclIndexKey = classLevelDep.subClass[items[node.firstSubClassItem[0]]];
							auto subDecl = tdDecls[globalDep.allTds.Values()[subDeclIndexKey]].Cast<WfClassDeclaration>();
							values[j] = subDecl;
						}
					}

					if (!parent && manager->errors.Count() == 0)
					{
						for (vint i = 0; i < classLevelDep.subClass.Count(); i++)
						{
							vint index = classLevelDep.subClass.Values()[i];
							if (!globalDep.topLevelClasses.Contains(index))
							{
								globalDep.topLevelClasses.Add(index);
							}
						}

						for (vint i = 0; i < pop.nodes.Count(); i++)
						{
							auto& keyNode = pop.nodes[i];
							vint keyIndex = classLevelDep.subClass[items[keyNode.firstSubClassItem[0]]];
							for (vint j = 0; j < keyNode.ins->Count(); j++)
							{
								auto& valueNode = pop.nodes[keyNode.ins->Get(j)];
								vint valueIndex = classLevelDep.subClass[items[valueNode.firstSubClassItem[0]]];
								if (!globalDep.topLevelClassDep.Contains(keyIndex, valueIndex))
								{
									globalDep.topLevelClassDep.Add(keyIndex, valueIndex);
								}
							}
						}
					}
				}
			}

			void WfCppConfig::GenerateFileClassMaps(GlobalDep& globalDep)
			{
				if (classDecls.Keys().Contains(nullptr))
				{
					PartialOrderingProcessor popSubClass;
					Array<vint> customFirstItems;		// popSubClass.nodes's index
					Array<vint> nonCustomFirstItems;	// popSubClass.nodes's index
					Array<bool> isCustomItems;			// popSubClass.nodes's index to boolean (true means custom item)
					Group<vint, vint> subClassDepGroup;	// popSubClass.nodes's index to index
					{
						// generate sub class using @cpp:File
						// globalDep.allTds.Keys()'s index to file name
						Dictionary<vint, WString> subClass;
						for (vint i = 0; i < customFilesClasses.Count(); i++)
						{
							WString key = customFilesClasses.Keys()[i];
							if (key != L"")
							{
								for (auto decl : customFilesClasses.GetByIndex(i))
								{
									auto stringKey = manager->declarationTypes[decl.Obj()]->GetTypeName();
									ASSIGN_INDEX_KEY(auto, indexKey, stringKey);
									subClass.Add(indexKey, key);
								}
							}
						}

						// check if all components contains either all classes of the same @cpp:File or a single non-@cpp:File class
						popSubClass.InitWithSubClass(globalDep.topLevelClasses, globalDep.topLevelClassDep, subClass);
						popSubClass.Sort();

						for (vint i = 0; i < popSubClass.components.Count(); i++)
						{
							auto& component = popSubClass.components[i];

							// check error
							if (component.nodeCount > 1)
							{
								List<ITypeDescriptor*> tds;
								for (vint j = 0; j < component.nodeCount; j++)
								{
									auto& node = popSubClass.nodes[component.firstNode[j]];
									for (vint k = 0; k < node.subClassItemCount; k++)
									{
										auto indexKey = globalDep.topLevelClasses[node.firstSubClassItem[k]];
										tds.Add(globalDep.allTds.Values()[indexKey]);
									}
								}

								SortLambda<ITypeDescriptor*>(&tds[0], tds.Count(), [](ITypeDescriptor* a, ITypeDescriptor* b)
								{
									return a->GetTypeName() <=> b->GetTypeName();
								});
								manager->errors.Add(WfErrors::CppUnableToSeparateCustomFile(tdDecls[tds[0]].Cast<WfClassDeclaration>().Obj(), tds));
							}
						}

						if (manager->errors.Count() != 0)
						{
							return;
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
								if (subClass.Keys().Contains(globalDep.topLevelClasses[node.firstSubClassItem[0]]))
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
										vint indexA = From(nodeA.firstSubClassItem, nodeA.firstSubClassItem + nodeA.subClassItemCount)
											.Select([&](vint index) {return globalDep.topLevelClasses[index]; })
											.Min();
										vint indexB = From(nodeB.firstSubClassItem, nodeB.firstSubClassItem + nodeB.subClassItemCount)
											.Select([&](vint index) {return globalDep.topLevelClasses[index]; })
											.Min();
										return indexA <=> indexB;
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

					CHECK_ERROR(popCustomFirst.components.Count() == customFirstItems.Count(), L"WfCppConfig::AssignClassDeclsToFiles()#Internal error: Unexpected circle dependency found, this should have been caught by code above.");
					CHECK_ERROR(popNonCustomFirst.components.Count() == nonCustomFirstItems.Count(), L"WfCppConfig::AssignClassDeclsToFiles()#Future error: Unexpected circle dependency found, this should have been caught by code above.");
					CHECK_ERROR(popCustomFirst.components.Count() == popNonCustomFirst.components.Count(), L"WfCppConfig::AssignClassDeclsToFiles()#Future error: Unexpected circle dependency found, this should have been caught by code above.");

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
					vint nonCustomFirstOrderPicked = 0;
					vint customFirstOrderPicked = 0;
					Array<bool> visited(customFirstOrder.Count());
					for (vint i = 0; i < visited.Count(); i++)
					{
						visited[i] = false;
					}

					auto pickItems = [&](Array<vint>& order, vint& picked, bool customItem)
					{
						while (picked < order.Count())
						{
							vint item = order[picked];
							if (visited[item])
							{
								picked++;
							}
							else if (isCustomItems[item] == customItem)
							{
								visited[item] = true;
								picked++;

								if(!customItem)
								{
									auto& node = popSubClass.nodes[item];
									for (vint i = 0; i < node.subClassItemCount; i++)
									{
										auto indexKey = globalDep.topLevelClasses[node.firstSubClassItem[i]];
										auto td = globalDep.allTds.Values()[indexKey];
										headerFilesClasses.Add(currentHeaderIndex, tdDecls[td].Cast<WfClassDeclaration>());
									}
								}
							}
							else
							{
								break;
							}
						}
					};

					while (nonCustomFirstOrderPicked < nonCustomFirstOrder.Count() || customFirstOrderPicked < customFirstOrder.Count())
					{
						pickItems(nonCustomFirstOrder, nonCustomFirstOrderPicked, false);
						pickItems(customFirstOrder, customFirstOrderPicked, false);

						pickItems(nonCustomFirstOrder, nonCustomFirstOrderPicked, true);
						pickItems(customFirstOrder, customFirstOrderPicked, true);

						currentHeaderIndex++;
					}

					// calculate header includes
					// globalDep.allTds.keys()'s index to header index
					Dictionary<vint, vint> headers;

					auto addToHeaders = [&](const List<Ptr<WfClassDeclaration>>& decls, vint headerIndex)
					{
						for (auto decl : decls)
						{
							auto stringKey = manager->declarationTypes[decl.Obj()]->GetTypeName();
							ASSIGN_INDEX_KEY(auto, indexKey, stringKey);
							headers.Add(indexKey, headerIndex);
						}
					};

					auto calculateIncludes = [&](const List<Ptr<WfClassDeclaration>>& decls, SortedList<vint>& includes)
					{
						for (auto decl : decls)
						{
							auto stringKey = manager->declarationTypes[decl.Obj()]->GetTypeName();
							ASSIGN_INDEX_KEY(auto, indexKey, stringKey);

							vint index = globalDep.topLevelClassDep.Keys().IndexOf(indexKey);
							if (index != -1)
							{
								const auto& values = globalDep.topLevelClassDep.GetByIndex(index);
								for (vint i = 0; i < values.Count(); i++)
								{
									vint header = headers[values[i]];
									if (header != 0 && !includes.Contains(header))
									{
										includes.Add(header);
									}
								}
							}
						}
					};

					for (vint i = 0; i < customFilesClasses.Count(); i++)
					{
						if (customFilesClasses.Keys()[i] != L"")
						{
							addToHeaders(customFilesClasses.GetByIndex(i), i);
						}
					}

					for (vint i = 0; i < headerFilesClasses.Count(); i++)
					{
						addToHeaders(headerFilesClasses.GetByIndex(i), -headerFilesClasses.Keys()[i]);
					}

					for (vint i = 0; i < customFilesClasses.Count(); i++)
					{
						if (customFilesClasses.Keys()[i] != L"")
						{
							SortedList<vint> includes;
							calculateIncludes(customFilesClasses.GetByIndex(i), includes);
							for (vint j = 0; j < includes.Count(); j++)
							{
								headerIncludes.Add(i, includes[j]);
							}
						}
					}

					for (vint i = 0; i < headerFilesClasses.Count(); i++)
					{
						if (headerFilesClasses.Keys()[i] != 0)
						{
							SortedList<vint> includes;
							calculateIncludes(headerFilesClasses.GetByIndex(i), includes);
							for (vint j = 0; j < includes.Count(); j++)
							{
								headerIncludes.Add(-headerFilesClasses.Keys()[i], includes[j]);
							}
						}
					}
				}
			}

			void WfCppConfig::SortFileClassMaps(GlobalDep& globalDep)
			{
				// sort customFilesClasses and headerFilesClasses according to classDecls[nullptr]
				if (classDecls.Keys().Contains(nullptr))
				{
					List<Ptr<WfClassDeclaration>> ordered;
					CopyFrom(ordered, classDecls[nullptr]);

					for (vint i = 0; i < customFilesClasses.Count(); i++)
					{
						auto& values = const_cast<List<Ptr<WfClassDeclaration>>&>(customFilesClasses.GetByIndex(i));
						Sort<Ptr<WfClassDeclaration>>(&values[0], values.Count(), [&](Ptr<WfClassDeclaration> a, Ptr<WfClassDeclaration> b)
						{
							vint aIndex = ordered.IndexOf(a.Obj());
							vint bIndex = ordered.IndexOf(b.Obj());
							return aIndex <=> bIndex;
						});
					}

					for (vint i = 0; i < headerFilesClasses.Count(); i++)
					{
						auto& values = const_cast<List<Ptr<WfClassDeclaration>>&>(headerFilesClasses.GetByIndex(i));
						Sort<Ptr<WfClassDeclaration>>(&values[0], values.Count(), [&](Ptr<WfClassDeclaration> a, Ptr<WfClassDeclaration> b)
						{
							vint aIndex = ordered.IndexOf(a.Obj());
							vint bIndex = ordered.IndexOf(b.Obj());
							return aIndex <=> bIndex;
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