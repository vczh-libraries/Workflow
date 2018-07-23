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
WfCppConfig::Collect
***********************************************************************/

			void WfCppConfig::AssignClassDeclsToFiles()
			{
				Group<Ptr<WfClassDeclaration>, Ptr<WfClassDeclaration>> expandedClassDecls;
				ExpandClassDeclGroup(nullptr, expandedClassDecls);
			}
		}
	}
}