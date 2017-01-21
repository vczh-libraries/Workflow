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
				WString						classFullName;
				Ptr<WfClassMember>			classMember;
				WString						prefix;
				bool						printableMember = false;

				WfGenerateClassMemberImplVisitor(WfCppConfig* _config, stream::StreamWriter& _writer, const WString& _classBaseName, const WString& _className, const WString& _classFullName, Ptr<WfClassMember> _classMember, const WString& _prefix)
					:config(_config)
					, writer(_writer)
					, classBaseName(_classBaseName)
					, className(_className)
					, classFullName(_classFullName)
					, classMember(_classMember)
					, prefix(_prefix)
				{
				}

				void WriteNotImplemented()
				{
					writer.WriteString(prefix);
					writer.WriteLine(L"{");
					writer.WriteString(prefix);
					writer.WriteLine(L"\tthrow ::vl::Exception(L\"You should implement this function.\");");
					writer.WriteString(prefix);
					writer.WriteLine(L"}");
				}

				void Visit(WfNamespaceDeclaration* node)override
				{
				}

				void Visit(WfFunctionDeclaration* node)override
				{
					if (!IsVirtual(config, node))
					{
						printableMember = true;

						bool userImpl = config->manager->GetAttribute(node->attributes, L"cpp", L"UserImpl");
						if (userImpl)
						{
							writer.WriteString(prefix);
							writer.WriteString(L"USERIMPL(");
							config->WriteFunctionHeader(writer, node, classFullName + L"::" + config->ConvertName(node->name.value), false);
							writer.WriteLine(L")");
						}

						writer.WriteString(prefix);
						auto returnType = config->WriteFunctionHeader(writer, node, classBaseName + L"::" + config->ConvertName(node->name.value), true);
						writer.WriteLine(L"");

						if (userImpl)
						{
							WriteNotImplemented();
						}
						else
						{
							config->WriteFunctionBody(writer, node->statement, prefix, returnType);
						}
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
					auto scope = config->manager->nodeScopes[node].Obj();
					printableMember = true;
					auto methodInfo = dynamic_cast<IMethodInfo*>(config->manager->declarationMemberInfos[node].Obj());

					List<WString> arguments;
					FOREACH(Ptr<WfFunctionArgument>, argument, node->arguments)
					{
						arguments.Add(config->ConvertName(argument->name.value));
					}

					bool userImpl = config->manager->GetAttribute(node->attributes, L"cpp", L"UserImpl");
					if (userImpl)
					{
						writer.WriteString(prefix);
						writer.WriteString(L"USERIMPL(");
						config->WriteFunctionHeader(writer, methodInfo, arguments, classFullName + L"::" + className, false);
						writer.WriteLine(L")");
					}

					writer.WriteString(prefix);
					config->WriteFunctionHeader(writer, methodInfo, arguments, classBaseName + L"::" + className, false);
					writer.WriteLine(L"");
					FOREACH_INDEXER(Ptr<WfBaseConstructorCall>, call, callIndex, node->baseConstructorCalls)
					{
						auto callType = CreateTypeInfoFromType(scope, call->type);
						auto callCtor = config->manager->baseConstructorCallResolvings[{node, callType->GetTypeDescriptor()}].value;

						writer.WriteString(prefix);
						if (callIndex == 0)
						{
							writer.WriteString(L"\t: ");
						}
						else
						{
							writer.WriteString(L"\t, ");
						}

						writer.WriteString(config->ConvertType(callType->GetTypeDescriptor()));
						writer.WriteString(L"(");
						FOREACH_INDEXER(Ptr<WfExpression>, argument, argumentIndex, call->arguments)
						{
							if (argumentIndex) writer.WriteString(L", ");
							GenerateExpression(config, writer, argument, callCtor->GetParameter(argumentIndex)->GetType());
						}
						writer.WriteLine(L")");
					}

					if (userImpl)
					{
						WriteNotImplemented();
					}
					else
					{
						config->WriteFunctionBody(writer, node->statement, prefix, nullptr);
					}
				}

				void Visit(WfDestructorDeclaration* node)override
				{
					printableMember = true;

					bool userImpl = config->manager->GetAttribute(node->attributes, L"cpp", L"UserImpl");
					if (userImpl)
					{
						writer.WriteString(prefix);
						writer.WriteString(L"USERIMPL(");
						writer.WriteLine(prefix + classFullName + L"::~" + className + L"()");
						writer.WriteLine(L")");
					}

					writer.WriteLine(prefix + classBaseName + L"::~" + className + L"()");

					if (userImpl)
					{
						WriteNotImplemented();
					}
					else
					{
						config->WriteFunctionBody(writer, node->statement, prefix, nullptr);
					}
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

			bool GenerateClassMemberImpl(WfCppConfig* config, stream::StreamWriter& writer, const WString& classBaseName, const WString& className, const WString& classFullName, Ptr<WfClassMember> member, const WString& prefix)
			{
				WfGenerateClassMemberImplVisitor visitor(config, writer, classBaseName, className, classFullName, member, prefix);
				member->declaration->Accept(&visitor);
				return visitor.printableMember;
			}
		}
	}
}