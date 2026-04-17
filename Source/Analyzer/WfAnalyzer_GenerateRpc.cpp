#include "WfAnalyzer.h"

namespace vl
{
	namespace workflow
	{
		namespace analyzer
		{
			using namespace collections;

			namespace rpc_generating
			{
				using namespace reflection::description;

				void WriteIndent(stream::TextWriter& writer, vint indent)
				{
					for (vint i = 0; i < indent; i++)
					{
						writer.WriteString(L"\t");
					}
				}

				WString PrintType(Ptr<WfType> type)
				{
					return stream::GenerateToStream([&](stream::TextWriter& writer)
					{
						WfPrint(type, L"", writer);
					});
				}

				WString EscapeStringLiteral(const WString& input)
				{
					WString output;
					for (vint i = 0; i < input.Length(); i++)
					{
						auto c = input[i];
						switch (c)
						{
						case L'\\':
							output = output + L"\\\\";
							break;
						case L'"':
							output = output + L"\\\"";
							break;
						default:
							output = output + WString::FromChar(c);
							break;
						}
					}
					return output;
				}

				void SplitTypeFullName(const WString& typeFullName, List<WString>& fragments)
				{
					vint start = 0;
					while (start <= typeFullName.Length())
					{
						vint separator = -1;
						for (vint i = start; i + 1 < typeFullName.Length(); i++)
						{
							if (typeFullName[i] == L':' && typeFullName[i + 1] == L':')
							{
								separator = i;
								break;
							}
						}
						if (separator == -1)
						{
							fragments.Add(typeFullName.Sub(start, typeFullName.Length() - start));
							break;
						}

						fragments.Add(typeFullName.Sub(start, separator - start));
						start = separator + 2;
					}
				}

				WString MangleRpcFullName(const WString& fullName)
				{
					WString mangled;
					for (vint i = 0; i < fullName.Length(); i++)
					{
						if (i + 1 < fullName.Length() && fullName[i] == L':' && fullName[i + 1] == L':')
						{
							mangled += L"__";
							i++;
						}
						else if (fullName[i] == L'.')
						{
							mangled += L"_";
						}
						else
						{
							mangled += WString::FromChar(fullName[i]);
						}
					}
					return mangled;
				}

				void WriteReturnStatement(stream::TextWriter& writer, Ptr<WfType> type, vint indent)
				{
					WriteIndent(writer, indent);
					if (!type)
					{
						writer.WriteLine(L"return null;");
						return;
					}

					if (auto predefinedType = type.Cast<WfPredefinedType>())
					{
						switch (predefinedType->name)
						{
						case WfPredefinedTypeName::Void:
							writer.WriteLine(L"return;");
							return;
						case WfPredefinedTypeName::Bool:
							writer.WriteLine(L"return false;");
							return;
						case WfPredefinedTypeName::String:
							writer.WriteLine(L"return \"\";");
							return;
						case WfPredefinedTypeName::Char:
							writer.WriteLine(L"return 0;");
							return;
						case WfPredefinedTypeName::Int:
						case WfPredefinedTypeName::UInt:
						case WfPredefinedTypeName::Float:
						case WfPredefinedTypeName::Double:
							writer.WriteLine(L"return 0;");
							return;
						default:
							break;
						}
					}

					if (type.Cast<WfRawPointerType>() || type.Cast<WfSharedPointerType>() || type.Cast<WfNullableType>())
					{
						writer.WriteLine(L"return null;");
					}
					else if (type.Cast<WfEnumerableType>() || type.Cast<WfMapType>() || type.Cast<WfObservableListType>())
					{
						writer.WriteLine(L"return {};");
					}
					else
					{
						writer.WriteLine(L"return {} of " + PrintType(type) + L";");
					}
				}

				void WriteMethod(stream::TextWriter& writer, WfFunctionDeclaration* methodDecl, vint indent)
				{
					WriteIndent(writer, indent);
					writer.WriteString(L"override func " + methodDecl->name.value + L"(");
					for (vint i = 0; i < methodDecl->arguments.Count(); i++)
					{
						if (i > 0)
						{
							writer.WriteString(L", ");
						}
						auto argument = methodDecl->arguments[i];
						writer.WriteString(argument->name.value + L" : " + PrintType(argument->type));
					}
					writer.WriteLine(L") : " + PrintType(methodDecl->returnType));
					WriteIndent(writer, indent);
					writer.WriteLine(L"{");
					WriteReturnStatement(writer, methodDecl->returnType, indent + 1);
					WriteIndent(writer, indent);
					writer.WriteLine(L"}");
				}

				bool CanGenerateWrapperObject(WfClassDeclaration* interfaceDecl)
				{
					for (auto decl : interfaceDecl->declarations)
					{
						if (decl.Cast<WfEventDeclaration>())
						{
							return false;
						}
					}
					return true;
				}

				WfDeclaration* FindRpcDeclaration(WfLexicalScopeManager* manager, const WString& fullName)
				{
					if (auto index = manager->rpcMetadata->typeNames.Keys().IndexOf(fullName); index != -1)
					{
						return manager->rpcMetadata->typeNames.Values()[index];
					}
					if (auto index = manager->rpcMetadata->methodNames.Keys().IndexOf(fullName); index != -1)
					{
						return manager->rpcMetadata->methodNames.Values()[index];
					}
					if (auto index = manager->rpcMetadata->eventNames.Keys().IndexOf(fullName); index != -1)
					{
						return manager->rpcMetadata->eventNames.Values()[index];
					}
					return nullptr;
				}
			}

			Ptr<WfModule> GenerateModuleRpc(WfLexicalScopeManager* manager)
			{
				using namespace rpc_generating;

				if (!manager || !manager->rpcMetadata || !manager->rpcMetadata->metadataModule)
				{
					return nullptr;
				}

				Dictionary<WString, WString> mangledNames;
				for (auto fullName : manager->rpcMetadata->typeFullNames)
				{
					auto mangled = MangleRpcFullName(fullName);
					auto index = mangledNames.Keys().IndexOf(mangled);
					if (index != -1 && mangledNames.Values()[index] != fullName)
					{
						manager->errors.Add(WfErrors::RpcMangledNameConflict(FindRpcDeclaration(manager, fullName), mangled, mangledNames.Values()[index], fullName));
						return nullptr;
					}
					if (index == -1)
					{
						mangledNames.Add(mangled, fullName);
					}
				}

				for (auto fullName : manager->rpcMetadata->methodFullNames)
				{
					auto mangled = MangleRpcFullName(fullName);
					auto index = mangledNames.Keys().IndexOf(mangled);
					if (index != -1 && mangledNames.Values()[index] != fullName)
					{
						manager->errors.Add(WfErrors::RpcMangledNameConflict(FindRpcDeclaration(manager, fullName), mangled, mangledNames.Values()[index], fullName));
						return nullptr;
					}
					if (index == -1)
					{
						mangledNames.Add(mangled, fullName);
					}
				}

				for (auto fullName : manager->rpcMetadata->eventFullNames)
				{
					auto mangled = MangleRpcFullName(fullName);
					auto index = mangledNames.Keys().IndexOf(mangled);
					if (index != -1 && mangledNames.Values()[index] != fullName)
					{
						manager->errors.Add(WfErrors::RpcMangledNameConflict(FindRpcDeclaration(manager, fullName), mangled, mangledNames.Values()[index], fullName));
						return nullptr;
					}
					if (index == -1)
					{
						mangledNames.Add(mangled, fullName);
					}
				}

				auto source = stream::GenerateToStream([&](stream::TextWriter& writer)
				{
					writer.WriteLine(L"module RpcMetadata;");
					writer.WriteLine(L"");

					vint id = 0;
					for (auto fullName : manager->rpcMetadata->typeFullNames)
					{
						writer.WriteLine(L"var rpctype_" + MangleRpcFullName(fullName) + L" : int = " + itow(id++) + L";");
					}
					for (auto fullName : manager->rpcMetadata->methodFullNames)
					{
						writer.WriteLine(L"var rpcmethod_" + MangleRpcFullName(fullName) + L" : int = " + itow(id++) + L";");
					}
					for (auto fullName : manager->rpcMetadata->eventFullNames)
					{
						writer.WriteLine(L"var rpcevent_" + MangleRpcFullName(fullName) + L" : int = " + itow(id++) + L";");
					}
					writer.WriteLine(L"");

					writer.WriteLine(L"var rpcNameToId : int[string] =");
					writer.WriteLine(L"{");
					id = 0;
					for (auto fullName : manager->rpcMetadata->typeFullNames)
					{
						WriteIndent(writer, 1);
						writer.WriteLine(L"\"" + EscapeStringLiteral(fullName) + L"\": " + itow(id++));
					}
					for (auto fullName : manager->rpcMetadata->methodFullNames)
					{
						WriteIndent(writer, 1);
						writer.WriteLine(L"\"" + EscapeStringLiteral(fullName) + L"\": " + itow(id++));
					}
					for (auto fullName : manager->rpcMetadata->eventFullNames)
					{
						WriteIndent(writer, 1);
						writer.WriteLine(L"\"" + EscapeStringLiteral(fullName) + L"\": " + itow(id++));
					}
					writer.WriteLine(L"};");
					writer.WriteLine(L"");

					writer.WriteLine(L"var rpcIdToName : string[int] =");
					writer.WriteLine(L"{");
					id = 0;
					for (auto fullName : manager->rpcMetadata->typeFullNames)
					{
						WriteIndent(writer, 1);
						writer.WriteLine(itow(id++) + L": \"" + EscapeStringLiteral(fullName) + L"\"");
					}
					for (auto fullName : manager->rpcMetadata->methodFullNames)
					{
						WriteIndent(writer, 1);
						writer.WriteLine(itow(id++) + L": \"" + EscapeStringLiteral(fullName) + L"\"");
					}
					for (auto fullName : manager->rpcMetadata->eventFullNames)
					{
						WriteIndent(writer, 1);
						writer.WriteLine(itow(id++) + L": \"" + EscapeStringLiteral(fullName) + L"\"");
					}
					writer.WriteLine(L"};");
					writer.WriteLine(L"");

					writer.WriteLine(L"func rpc_IRpcObjectOps(lc : system::IRpcLifeCycle*) : system::IRpcObjectOps^");
					writer.WriteLine(L"{");
					writer.WriteLine(L"\treturn null;");
					writer.WriteLine(L"}");
					writer.WriteLine(L"");
					writer.WriteLine(L"func rpc_IRpcObjectEventOps(lc : system::IRpcLifeCycle*) : system::IRpcObjectEventOps^");
					writer.WriteLine(L"{");
					writer.WriteLine(L"\treturn null;");
					writer.WriteLine(L"}");
					writer.WriteLine(L"");

					for (auto typeFullName : manager->rpcMetadata->typeFullNames)
					{
						auto interfaceDecl = manager->rpcMetadata->typeNames.Values()[manager->rpcMetadata->typeNames.Keys().IndexOf(typeFullName)];
						List<WString> fragments;
						SplitTypeFullName(typeFullName, fragments);
						for (vint i = 0; i < fragments.Count() - 1; i++)
						{
							WriteIndent(writer, i);
							writer.WriteLine(L"namespace " + fragments[i]);
							WriteIndent(writer, i);
							writer.WriteLine(L"{");
						}

						auto indent = fragments.Count() - 1;
						auto interfaceName = fragments[fragments.Count() - 1];
						WriteIndent(writer, indent);
						writer.WriteLine(L"func rpcwrapper_" + interfaceName + L"(lc : system::IRpcLifeCycle*) : (" + interfaceName + L"^)");
						WriteIndent(writer, indent);
						writer.WriteLine(L"{");
						WriteIndent(writer, indent + 1);
						writer.WriteLine(L"return null;");
						WriteIndent(writer, indent);
						writer.WriteLine(L"}");

						for (vint i = fragments.Count() - 2; i >= 0; i--)
						{
							WriteIndent(writer, i);
							writer.WriteLine(L"}");
						}
						writer.WriteLine(L"");
					}
				});

				return ParseModule(source, manager->workflowParser);
			}
		}
	}
}
