#include "WfCpp.h"

namespace vl
{
	namespace workflow
	{
		namespace cppcodegen
		{
			using namespace collections;
			using namespace reflection;
			using namespace reflection::description;
			using namespace analyzer;

/***********************************************************************
WfGenerateClassMemberDeclVisitor
***********************************************************************/

			class WfGenerateClassMemberDeclVisitor : public Object, public WfDeclaration::IVisitor
			{
			public:
				WfCppConfig*				config;
				stream::StreamWriter&		writer;
				Ptr<WfClassDeclaration>		classDecl;
				Ptr<WfClassMember>			classMember;
				WString						prefix;

				WfGenerateClassMemberDeclVisitor(WfCppConfig* _config, stream::StreamWriter& _writer, Ptr<WfClassDeclaration> _classDecl, Ptr<WfClassMember> _classMember, const WString& _prefix)
					:config(_config)
					, writer(_writer)
					, classDecl(_classDecl)
					, classMember(_classMember)
					, prefix(_prefix)
				{
				}

				void Visit(WfNamespaceDeclaration* node)override
				{
				}

				void Visit(WfFunctionDeclaration* node)override
				{
				}

				void Visit(WfVariableDeclaration* node)override
				{
				}

				void Visit(WfEventDeclaration* node)override
				{
				}

				void Visit(WfPropertyDeclaration* node)override
				{
				}

				void Visit(WfConstructorDeclaration* node)override
				{
				}

				void Visit(WfDestructorDeclaration* node)override
				{
				}

				void Visit(WfClassDeclaration* node)override
				{
				}

				void Visit(WfEnumDeclaration* node)override
				{
				}

				void Visit(WfStructDeclaration* node)override
				{
				}
			};

			void GenerateClassMemberDecl(WfCppConfig* config, stream::StreamWriter& writer, Ptr<WfClassDeclaration> classDecl, Ptr<WfClassMember> member, const WString& prefix)
			{
				WfGenerateClassMemberDeclVisitor visitor(config, writer, classDecl, member, prefix);
				member->declaration->Accept(&visitor);
			}
		}
	}
}