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
				WString						prefix;
				bool						forClassExpr;

				WfGenerateClassMemberDeclVisitor(WfCppConfig* _config, stream::StreamWriter& _writer, const WString& _className, const WString& _prefix, bool _forClassExpr)
					:config(_config)
					, writer(_writer)
					, className(_className)
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
					else if (node->functionKind == WfFunctionKind::Static)
					{
						writer.WriteString(L"static ");
					}
					config->WriteFunctionHeader(writer, Ptr(node), config->ConvertName(node->name.value), true);
					if (isVirtual)
					{
						writer.WriteString(L" = 0");
					}
					else if (node->functionKind == WfFunctionKind::Override)
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
					if (forClassExpr)
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
					auto typeInfo = eventInfo->GetHandlerType()->GetElementType();
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

				void Visit(WfVirtualCfeDeclaration* node)override
				{
					for (auto decl : node->expandedDeclarations)
					{
						decl->Accept(this);
					}
				}

				void Visit(WfVirtualCseDeclaration* node)override
				{
					for (auto decl : node->expandedDeclarations)
					{
						decl->Accept(this);
					}
				}
			};

			class WfGenerateClassMemberInitVisitor : public empty_visitor::DeclarationVisitor
			{
			public:
				WfCppConfig * config;
				stream::StreamWriter&		writer;
				WString						prefix;
				vint&						callIndex;

				WfGenerateClassMemberInitVisitor(WfCppConfig* _config, stream::StreamWriter& _writer, const WString& _prefix, vint& _callIndex)
					:config(_config)
					, writer(_writer)
					, prefix(_prefix)
					, callIndex(_callIndex)
				{
				}

				void WriteVariableHeader(WfVariableDeclaration* node)
				{
					writer.WriteString(prefix);
					if (callIndex++ == 0)
					{
						writer.WriteString(L"\t: ");
					}
					else
					{
						writer.WriteString(L"\t, ");
					}
					writer.WriteString(config->ConvertName(node->name.value));
				}

				void Visit(WfVariableDeclaration* node)override
				{
					auto scope = config->manager->nodeScopes[node].Obj();
					auto symbol = scope->symbols[node->name.value][0];
					auto typeInfo = symbol->typeInfo;
					if (node->expression)
					{
						WriteVariableHeader(node);
						writer.WriteString(L"(");
						GenerateExpression(config, writer, node->expression, typeInfo.Obj());
						writer.WriteLine(L")");
					}
					else
					{
						auto defaultValue = config->DefaultValue(typeInfo.Obj());
						if (defaultValue != L"")
						{
							WriteVariableHeader(node);
							writer.WriteString(L"(");
							writer.WriteString(defaultValue);
							writer.WriteLine(L")");
						}
					}
				}

				void Dispatch(WfVirtualCfeDeclaration* node)override
				{
					for (auto decl : node->expandedDeclarations)
					{
						decl->Accept(this);
					}
				}

				void Dispatch(WfVirtualCseDeclaration* node)override
				{
					for (auto decl : node->expandedDeclarations)
					{
						decl->Accept(this);
					}
				}
			};

			void GenerateClassMemberDecl(WfCppConfig* config, stream::StreamWriter& writer, const WString& className, Ptr<WfDeclaration> memberDecl, const WString& prefix, bool forClassExpr)
			{
				WfGenerateClassMemberDeclVisitor visitor(config, writer, className, prefix, forClassExpr);
				memberDecl->Accept(&visitor);
			}

/***********************************************************************
WfGenerateClassMemberImplVisitor
***********************************************************************/

			class WfGenerateClassMemberImplVisitor : public Object, public WfDeclaration::IVisitor
			{
			public:
				WfCppConfig*				config;
				stream::StreamWriter&		writer;
				WfClassDeclaration*			classDef;
				WString						classBaseName;
				WString						className;
				WString						classFullName;
				WString						prefix;
				bool						printableMember = false;

				WfGenerateClassMemberImplVisitor(WfCppConfig* _config, stream::StreamWriter& _writer, WfClassDeclaration* _classDef, const WString& _classBaseName, const WString& _className, const WString& _classFullName, const WString& _prefix)
					:config(_config)
					, writer(_writer)
					, classDef(_classDef)
					, classBaseName(_classBaseName)
					, className(_className)
					, classFullName(_classFullName)
					, prefix(_prefix)
				{
				}

				void WriteNotImplemented(const WString& classFullName)
				{
					writer.WriteString(prefix);
					writer.WriteLine(L"{/* USER_CONTENT_BEGIN(" + classFullName + L") */");
					writer.WriteString(prefix);
					writer.WriteLine(L"\tthrow ::vl::Exception(L\"You should implement this function.\");");
					writer.WriteString(prefix);
					writer.WriteLine(L"}/* USER_CONTENT_END() */");
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
						auto returnType = config->WriteFunctionHeader(writer, Ptr(node), classBaseName + L"::" + config->ConvertName(node->name.value), true);
						writer.WriteLine(L"");

						bool userImpl = config->attributeEvaluator->GetAttribute(node->attributes, L"cpp", L"UserImpl");
						if (userImpl)
						{
							WriteNotImplemented(classFullName);
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
					for (auto argument : node->arguments)
					{
						arguments.Add(config->ConvertName(argument->name.value));
					}

					writer.WriteString(prefix);
					config->WriteFunctionHeader(writer, methodInfo, arguments, classBaseName + L"::" + className, false);
					writer.WriteLine(L"");

					vint callIndex = 0;
					for (auto call : node->baseConstructorCalls)
					{
						auto callType = CreateTypeInfoFromType(scope, call->type, false);
						auto callCtor = config->manager->baseConstructorCallResolvings[{node, callType->GetTypeDescriptor()}].value;

						writer.WriteString(prefix);
						if (callIndex++ == 0)
						{
							writer.WriteString(L"\t: ");
						}
						else
						{
							writer.WriteString(L"\t, ");
						}

						writer.WriteString(config->ConvertType(callType->GetTypeDescriptor()));
						writer.WriteString(L"(");
						for (auto [argument, argumentIndex] : indexed(call->arguments))
						{
							if (argumentIndex) writer.WriteString(L", ");
							GenerateExpression(config, writer, argument, callCtor->GetParameter(argumentIndex)->GetType());
						}
						writer.WriteLine(L")");
					}

					if (classDef)
					{
						WfGenerateClassMemberInitVisitor visitor(config, writer, prefix, callIndex);
						for (auto member : classDef->declarations)
						{
							member->Accept(&visitor);
						}
					}

					bool userImpl = config->attributeEvaluator->GetAttribute(node->attributes, L"cpp", L"UserImpl");
					if (userImpl)
					{
						WriteNotImplemented(classFullName);
					}
					else
					{
						config->WriteFunctionBody(writer, node->statement, prefix, nullptr);
					}
				}

				void Visit(WfDestructorDeclaration* node)override
				{
					printableMember = true;

					writer.WriteLine(prefix + classBaseName + L"::~" + className + L"()");

					bool userImpl = config->attributeEvaluator->GetAttribute(node->attributes, L"cpp", L"UserImpl");
					if (userImpl)
					{
						WriteNotImplemented(classFullName);
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

				void Visit(WfVirtualCfeDeclaration* node)override
				{
					for (auto decl : node->expandedDeclarations)
					{
						decl->Accept(this);
					}
				}

				void Visit(WfVirtualCseDeclaration* node)override
				{
					for (auto decl : node->expandedDeclarations)
					{
						decl->Accept(this);
					}
				}
			};

			bool GenerateClassMemberImpl(WfCppConfig* config, stream::StreamWriter& writer, WfClassDeclaration* classDef, const WString& classBaseName, const WString& className, const WString& classFullName, Ptr<WfDeclaration> memberDecl, const WString& prefix)
			{
				WfGenerateClassMemberImplVisitor visitor(config, writer, classDef, classBaseName, className, classFullName, prefix);
				memberDecl->Accept(&visitor);
				return visitor.printableMember;
			}
		}
	}
}