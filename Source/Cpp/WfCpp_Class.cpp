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

			bool IsVirtual(WfCppConfig* config, WfFunctionDeclaration* node)
			{
				vint index = config->manager->declarationMemberInfos.Keys().IndexOf(node);
				if (index != -1)
				{
					auto methodInfo = dynamic_cast<IMethodInfo*>(config->manager->declarationMemberInfos.Values()[index].Obj());
					if (methodInfo->IsStatic())
					{
						return false;
					}
					auto td = methodInfo->GetOwnerTypeDescriptor();
					return td->GetTypeDescriptorFlags() == TypeDescriptorFlags::Interface;
				}
				return false;
			}

/***********************************************************************
WfGenerateClassMemberDeclVisitor
***********************************************************************/

			class WfGenerateClassMemberDeclVisitor : public Object, public WfDeclaration::IVisitor
			{
			public:
				WfCppConfig*				config;
				stream::StreamWriter&		writer;
				WString						className;
				Ptr<WfClassMember>			classMember;
				WString						prefix;
				bool						forClassExpr;

				WfGenerateClassMemberDeclVisitor(WfCppConfig* _config, stream::StreamWriter& _writer, const WString& _className, Ptr<WfClassMember> _classMember, const WString& _prefix, bool _forClassExpr)
					:config(_config)
					, writer(_writer)
					, className(_className)
					, classMember(_classMember)
					, prefix(_prefix)
					, forClassExpr(_forClassExpr)
				{
				}

				void Visit(WfNamespaceDeclaration* node)override
				{
				}

				void Visit(WfFunctionDeclaration* node)override
				{
					bool isVirtual = IsVirtual(config, node);

					writer.WriteString(prefix);
					if (isVirtual)
					{
						writer.WriteString(L"virtual ");
					}
					else if (classMember->kind == WfClassMemberKind::Static)
					{
						writer.WriteString(L"static ");
					}
					config->WriteFunctionHeader(writer, node, config->ConvertName(node->name.value), true);
					if (isVirtual)
					{
						writer.WriteString(L" = 0");
					}
					else if (classMember->kind == WfClassMemberKind::Override)
					{
						writer.WriteString(L" override");
					}
					writer.WriteLine(L";");
				}

				void Visit(WfVariableDeclaration* node)override
				{
					auto scope = config->manager->nodeScopes[node].Obj();
					auto symbol = scope->symbols[node->name.value][0];
					auto typeInfo = symbol->typeInfo;
					writer.WriteString(prefix + config->ConvertType(typeInfo.Obj()) + L" " + config->ConvertName(node->name.value));
					if (!forClassExpr && node->expression)
					{
						writer.WriteString(L" = ");
						GenerateExpression(config, writer, node->expression, typeInfo.Obj());
					}
					else
					{
						auto defaultValue = config->DefaultValue(typeInfo.Obj());
						if (defaultValue != L"")
						{
							writer.WriteString(L" = ");
							writer.WriteString(defaultValue);
						}
					}
					writer.WriteLine(L";");
				}

				void Visit(WfEventDeclaration* node)override
				{
					auto eventInfo = dynamic_cast<IEventInfo*>(config->manager->declarationMemberInfos[node].Obj());
					auto typeInfo = eventInfo->GetHandlerType();
					writer.WriteString(prefix);

					writer.WriteString(L"::vl::Event<void(");
					vint count = typeInfo->GetGenericArgumentCount();
					for (vint i = 1; i < count; i++)
					{
						if (i > 1) writer.WriteString(L", ");
						writer.WriteString(config->ConvertType(typeInfo->GetGenericArgument(i)));
					}
					writer.WriteLine(L")> " + config->ConvertName(node->name.value) + L";");
				}

				void Visit(WfPropertyDeclaration* node)override
				{
				}

				void Visit(WfConstructorDeclaration* node)override
				{
					auto methodInfo = dynamic_cast<IMethodInfo*>(config->manager->declarationMemberInfos[node].Obj());
					writer.WriteString(prefix);
					config->WriteFunctionHeader(writer, methodInfo, className, false);
					writer.WriteLine(L";");
				}

				void Visit(WfDestructorDeclaration* node)override
				{
					writer.WriteLine(prefix + L"~" + className + L"();");
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

			void GenerateClassMemberDecl(WfCppConfig* config, stream::StreamWriter& writer, const WString& className, Ptr<WfClassMember> member, const WString& prefix, bool forClassExpr)
			{
				WfGenerateClassMemberDeclVisitor visitor(config, writer, className, member, prefix, forClassExpr);
				member->declaration->Accept(&visitor);
			}

/***********************************************************************
WfGenerateClassMemberImplVisitor
***********************************************************************/

			class WfGenerateClassMemberImplVisitor : public Object, public WfDeclaration::IVisitor
			{
			public:
				WfCppConfig*				config;
				stream::StreamWriter&		writer;
				WString						classBaseName;
				WString						className;
				Ptr<WfClassMember>			classMember;
				WString						prefix;
				bool						printableMember = false;

				WfGenerateClassMemberImplVisitor(WfCppConfig* _config, stream::StreamWriter& _writer, const WString& _classBaseName, const WString& _className, Ptr<WfClassMember> _classMember, const WString& _prefix)
					:config(_config)
					, writer(_writer)
					, classBaseName(_classBaseName)
					, className(_className)
					, classMember(_classMember)
					, prefix(_prefix)
				{
				}

				void Visit(WfNamespaceDeclaration* node)override
				{
				}

				void Visit(WfFunctionDeclaration* node)override
				{
					if (!IsVirtual(config, node))
					{
						printableMember = true;
						writer.WriteString(prefix);
						auto returnType = config->WriteFunctionHeader(writer, node, classBaseName + L"::" + config->ConvertName(node->name.value), true);
						writer.WriteLine(L"");

						writer.WriteLine(prefix + L"{");
						config->WriteFunctionBody(writer, node->statement, prefix + L"\t", returnType);
						writer.WriteLine(prefix + L"}");
					}
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
					printableMember = true;
					auto methodInfo = dynamic_cast<IMethodInfo*>(config->manager->declarationMemberInfos[node].Obj());
					writer.WriteString(prefix);
					config->WriteFunctionHeader(writer, methodInfo, classBaseName + L"::" + className, false);
					writer.WriteLine(L"");

					writer.WriteLine(prefix + L"{");
					config->WriteFunctionBody(writer, node->statement, prefix + L"\t", nullptr);
					writer.WriteLine(prefix + L"}");
				}

				void Visit(WfDestructorDeclaration* node)override
				{
					printableMember = true;
					writer.WriteLine(prefix + classBaseName + L"::~" + className + L"()");

					writer.WriteLine(prefix + L"{");
					config->WriteFunctionBody(writer, node->statement, prefix + L"\t", nullptr);
					writer.WriteLine(prefix + L"}");
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

			bool GenerateClassMemberImpl(WfCppConfig* config, stream::StreamWriter& writer, const WString& classBaseName, const WString& className, Ptr<WfClassMember> member, const WString& prefix)
			{
				WfGenerateClassMemberImplVisitor visitor(config, writer, classBaseName, className, member, prefix);
				member->declaration->Accept(&visitor);
				return visitor.printableMember;
			}
		}
	}
}