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
				Group<vint, vint> depGroup;
				Dictionary<vint, vint> subClass;

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

			void WfCppConfig::GenerateFileClassMaps(GlobalDep& globapDep)
			{
				vint index = customFilesClasses.Keys().IndexOf(L"");
				if (index != -1)
				{
					FOREACH(Ptr<WfClassDeclaration>, classDecl, customFilesClasses.GetByIndex(index))
					{
						headerFilesClasses.Add(0, classDecl);
					}
				}
			}

			void WfCppConfig::SortFileClassMaps(GlobalDep& globalDep)
			{
				// sort customFilesClasses and headerFilesClasses according to classDecls[nullptr]
				if (classDecls.Count() > 0)
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