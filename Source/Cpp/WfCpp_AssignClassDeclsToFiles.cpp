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

			void WfCppConfig::ExpandClassDeclGroup(Ptr<WfClassDeclaration> parent, collections::Group<Ptr<WfClassDeclaration>, Ptr<WfClassDeclaration>>& expandedClassDecls)
			{
				vint index = classDecls.Keys().IndexOf(parent.Obj());
				if (index == -1) return;

				FOREACH(Ptr<WfClassDeclaration>, subDecl, classDecls.GetByIndex(index))
				{
					ExpandClassDeclGroup(subDecl, expandedClassDecls);
				}

				auto expanded = From(classDecls.GetByIndex(index))
					.Concat(From(classDecls.GetByIndex(index))
						.Select([&](Ptr<WfClassDeclaration> subDecl)
						{
							return expandedClassDecls.Keys().IndexOf(subDecl.Obj());
						})
						.Where([](vint index)
						{
							return index != -1;
						})
						.SelectMany([&](vint index)
						{
							return From(expandedClassDecls.GetByIndex(index));
						})
					);

				FOREACH(Ptr<WfClassDeclaration>, subDecl, expanded)
				{
					expandedClassDecls.Add(parent, subDecl);
				}
			}

/***********************************************************************
WfCppConfig::GenerateClassDependencies
***********************************************************************/

			void WfCppConfig::GenerateClassDependencies(collections::Group<Ptr<WfClassDeclaration>, Ptr<WfClassDeclaration>>& dependencies)
			{
				FOREACH_INDEXER(ITypeDescriptor*, td, tdIndex, tdDecls.Keys())
				{
					if (auto decl = tdDecls.Values()[tdIndex].Cast<WfClassDeclaration>())
					{
						vint count = td->GetBaseTypeDescriptorCount();
						for (vint i = 0; i < count; i++)
						{
							auto baseTd = td->GetBaseTypeDescriptor(i);
							vint index = tdDecls.Keys().IndexOf(baseTd);
							if (index != -1)
							{
								dependencies.Add(decl, tdDecls.Values()[index].Cast<WfClassDeclaration>());
							}
						}
					}
				}
			}

/***********************************************************************
WfCppConfig::Collect
***********************************************************************/

			void WfCppConfig::AssignClassDeclsToFiles()
			{
				Group<Ptr<WfClassDeclaration>, Ptr<WfClassDeclaration>> expandedClassDecls;
				Group<Ptr<WfClassDeclaration>, Ptr<WfClassDeclaration>> dependencies;
				ExpandClassDeclGroup(nullptr, expandedClassDecls);
				GenerateClassDependencies(dependencies);

				for (vint i = classDecls.Count() - 1; i >= 0; i--)
				{
					auto parent = classDecls.Keys()[i];
					const auto& items = expandedClassDecls.GetByIndex(i);
					Group<Ptr<WfClassDeclaration>, Ptr<WfClassDeclaration>> depGroup;
					Dictionary<Ptr<WfClassDeclaration>, Ptr<WfClassDeclaration>> subClass;

					FOREACH(Ptr<WfClassDeclaration>, subDecl, items)
					{
						vint index = dependencies.Keys().IndexOf(subDecl.Obj());
						if (index != -1)
						{
							FOREACH(Ptr<WfClassDeclaration>, dep, dependencies.GetByIndex(index))
							{
								if (items.Contains(dep.Obj()))
								{
									depGroup.Add(subDecl, dep);
								}
							}
						}
					}

					FOREACH(Ptr<WfClassDeclaration>, subDecl, classDecls.GetByIndex(i))
					{
						subClass.Add(subDecl, subDecl);

						vint index = expandedClassDecls.Keys().IndexOf(subDecl.Obj());
						if (index != -1)
						{
							FOREACH(Ptr<WfClassDeclaration>, expandDecl, expandedClassDecls.GetByIndex(index))
							{
								subClass.Add(expandDecl, subDecl);
							}
						}
					}

					PartialOrderingProcessor pop;
					pop.InitWithSubClass(items, depGroup, subClass);

					classDecls.Remove(parent.Obj());
					for (vint j = 0; j < pop.components.Count(); j++)
					{
						auto& component = pop.components[j];
						CHECK_ERROR(component.nodeCount == 1, L"WfCppConfig::AssignClassDeclsToFiles()#Future error: Unexpected circle dependency found.");
						
						auto& node = pop.nodes[component.firstNode[0]];
						auto subDecl = subClass[items[node.firstSubClassItem[0]].Obj()];
						classDecls.Add(parent, subDecl);
					}
				}
			}
		}
	}
}