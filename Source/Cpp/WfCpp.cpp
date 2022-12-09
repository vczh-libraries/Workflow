#include "WfCpp.h"

namespace vl
{
	namespace workflow
	{
		namespace cppcodegen
		{
			using namespace collections;
			using namespace regex;
			using namespace reflection;
			using namespace reflection::description;
			using namespace stream;
			using namespace emitter;

/***********************************************************************
WfCppConfig
***********************************************************************/

			WfCppConfig::WfCppConfig(analyzer::WfLexicalScopeManager* _manager, const WString& _assemblyName, const WString& _assemblyNamespace)
				:manager(_manager)
				, regexSplitName(L"::")
				, regexSpecialName(L"/<(<category>/w+)(-(<category>/w+))*/>(<name>/w*)")
				, regexTemplate(L", /$Arguments|/$Arguments, |/$/l+")
				, specialName_category(regexSpecialName.CaptureNames().IndexOf(L"category"))
				, specialName_name(regexSpecialName.CaptureNames().IndexOf(L"name"))
				, assemblyName(_assemblyName)
				, assemblyNamespace(_assemblyNamespace)
			{
				attributeEvaluator = Ptr(new WfAttributeEvaluator(manager));
				Collect();
				for (vint i = 0; i < enumDecls.Count(); i++)
				{
					const auto& values = enumDecls.GetByIndex(i);
					SortDeclsByName(const_cast<List<Ptr<WfEnumDeclaration>>&>(values));
				}
				for (vint i = 0; i < structDecls.Count(); i++)
				{
					const auto& values = structDecls.GetByIndex(i);
					SortDeclsByName(const_cast<List<Ptr<WfStructDeclaration>>&>(values));
				}
				for (vint i = 0; i < classDecls.Count(); i++)
				{
					const auto& values = classDecls.GetByIndex(i);
					SortDeclsByName(const_cast<List<Ptr<WfClassDeclaration>>&>(values));
				}
				AssignClassDeclsToFiles();
			}

			WfCppConfig::~WfCppConfig()
			{
			}

			void WfCppConfig::WriteFunctionBody(stream::StreamWriter& writer, Ptr<WfExpression> expr, const WString& prefix, ITypeInfo* expectedType)
			{
				writer.WriteString(prefix);
				writer.WriteString(L"return ");
				GenerateExpression(this, writer, expr, expectedType);
				writer.WriteLine(L";");
			}

			void WfCppConfig::WriteFunctionBody(stream::StreamWriter& writer, Ptr<WfStatement> stat, const WString& prefix, ITypeInfo* expectedType)
			{
				GenerateStatement(this, Ptr(new FunctionRecord), writer, stat, prefix, WString::Unmanaged(L"\t"), expectedType);
			}

			WString WfCppConfig::CppNameToHeaderEnumStructName(const WString& fullName, const WString& type)
			{
				return L"__vwsn_" + type + L"s::" + ConvertFullName(fullName, L"_");
			}

			WString WfCppConfig::ConvertNameInternal(const WString& name, const WString& specialNameCategory, bool alwaysUseCategory)
			{
				if (name.Length() > 0 && name[0] == L'$')
				{
					return L"__vwsno_" + name.Sub(1, name.Length() - 1);
				}

				auto match = regexSpecialName.Match(name);
				if (match)
				{
					return specialNameCategory
						+ From(match->Groups()[specialName_category])
							.Select([](const RegexString& rs)
							{
								return rs.Value();
							})
							.Aggregate([](const WString& a, const WString& b)
							{
								return a + L"_" + b;
							})
						+ L"_" + match->Groups()[specialName_name][0].Value();
				}
				else if (alwaysUseCategory)
				{
					return specialNameCategory + name;
				}
				{
					return name;
				}
			}

			WString WfCppConfig::ConvertName(const WString& name)
			{
				return ConvertNameInternal(name, WString::Unmanaged(L"__vwsn_"), false);
			}

			WString WfCppConfig::ConvertName(const WString& name, const WString& specialNameCategory)
			{
				return ConvertNameInternal(name, specialNameCategory, true);
			}

			WString WfCppConfig::ConvertFullName(const WString& fullName, WString delimiter)
			{
				List<Ptr<RegexMatch>> matches;
				regexSplitName.Split(fullName, false, matches);
				return (fullName[0] == L':' ? delimiter : WString::Empty) + From(matches)
					.Select([this](Ptr<RegexMatch> match)
					{
						return ConvertName(match->Result().Value());
					})
					.Aggregate([&](const WString& a, const WString& b)
					{
						return a + delimiter + b;
					});
			}

			WString WfCppConfig::ConvertFunctionType(IMethodInfo* methodInfo, WString name)
			{
				WString type = ConvertType(methodInfo->GetReturn()) + name + L"(";
				vint count = methodInfo->GetParameterCount();
				for (vint i = 0; i < count; i++)
				{
					if (i > 0)
					{
						type += L", ";
					}
					type += ConvertArgumentType(methodInfo->GetParameter(i)->GetType());
				}
				type += L")";
				return type;
			}

			WString WfCppConfig::ConvertFunctionType(ITypeInfo* typeInfo)
			{
				if (typeInfo->GetDecorator() == ITypeInfo::SharedPtr)
				{
					return ConvertFunctionType(typeInfo->GetElementType());
				}
				CHECK_ERROR(typeInfo->GetDecorator() == ITypeInfo::Generic, L"WfCppConfig::ConvertFunctionType(ITypeInfo*)#Internal error: Wrong function type.");
				CHECK_ERROR(typeInfo->GetTypeDescriptor() == description::GetTypeDescriptor<IValueFunctionProxy>(), L"WfCppConfig::ConvertFunctionType(ITypeInfo*)#Internal error: Wrong function type.");

				WString type = ConvertType(typeInfo->GetGenericArgument(0)) + L"(";
				vint count = typeInfo->GetGenericArgumentCount();
				for (vint i = 1; i < count; i++)
				{
					if (i > 1) type += L", ";
					type += ConvertArgumentType(typeInfo->GetGenericArgument(i));
				}
				type += L")";
				return type;
			}

			bool WfCppConfig::IsSpecialGenericType(ITypeInfo* typeInfo)
			{
				switch (typeInfo->GetDecorator())
				{
				case ITypeInfo::SharedPtr:
					if (typeInfo->GetTypeDescriptor() == description::GetTypeDescriptor<IValueFunctionProxy>())
					{
						return true;
					}
					else if (typeInfo->GetTypeDescriptor() == description::GetTypeDescriptor<IValueEnumerable>())
					{
						return true;
					}
					else
					{
						return false;
					}
				default:
					return false;
				}
			}

			WString WfCppConfig::ConvertType(ITypeDescriptor* typeInfo, WString delimiter)
			{
				return ConvertFullName(CppGetFullName(typeInfo), delimiter);
			}

			WString WfCppConfig::ConvertType(ITypeInfo* typeInfo, bool useHeaderEnumStructName)
			{
				switch (typeInfo->GetDecorator())
				{
				case ITypeInfo::RawPtr:
					return ConvertType(typeInfo->GetElementType(), useHeaderEnumStructName) + L"*";
				case ITypeInfo::SharedPtr:
					if (typeInfo->GetElementType()->GetDecorator() == ITypeInfo::Generic)
					{
						if (typeInfo->GetTypeDescriptor() == description::GetTypeDescriptor<IValueFunctionProxy>())
						{
							return ConvertType(typeInfo->GetElementType(), useHeaderEnumStructName);
						}
						else if (typeInfo->GetTypeDescriptor() == description::GetTypeDescriptor<IValueEnumerable>())
						{
							return ConvertType(typeInfo->GetElementType(), useHeaderEnumStructName);
						}
					}
					return L"::vl::Ptr<" + ConvertType(typeInfo->GetElementType(), useHeaderEnumStructName) + L">";
				case ITypeInfo::Nullable:
					return L"::vl::Nullable<" + ConvertType(typeInfo->GetElementType(), useHeaderEnumStructName) + L">";
				case ITypeInfo::Generic:
					if (typeInfo->GetTypeDescriptor() == description::GetTypeDescriptor<IValueFunctionProxy>())
					{
						return L"::vl::Func<" + ConvertFunctionType(typeInfo) + L">";
					}
					else if(typeInfo->GetTypeDescriptor() == description::GetTypeDescriptor<IValueEnumerable>())
					{
						return L"::vl::collections::LazyList<" + ConvertType(typeInfo->GetGenericArgument(0), useHeaderEnumStructName) + L">";
					}
					else
					{
						return ConvertType(typeInfo->GetElementType(), useHeaderEnumStructName);
					}
				default:;
				}
				if (useHeaderEnumStructName)
				{
					switch (typeInfo->GetTypeDescriptor()->GetTypeDescriptorFlags())
					{
					case TypeDescriptorFlags::EnumType:
						if (tdDecls.Keys().Contains(typeInfo->GetTypeDescriptor()))
						{
							return L"::" + CppNameToHeaderEnumStructName(CppGetFullName(typeInfo->GetTypeDescriptor()), L"enum");
						}
						break;
					case TypeDescriptorFlags::Struct:
						if (tdDecls.Keys().Contains(typeInfo->GetTypeDescriptor()))
						{
							return L"::" + CppNameToHeaderEnumStructName(CppGetFullName(typeInfo->GetTypeDescriptor()), L"struct");
						}
						break;
					default:;
					}
					return ConvertType(typeInfo->GetTypeDescriptor());
				}
				else
				{
					return ConvertType(typeInfo->GetTypeDescriptor());
				}
			}

			WString WfCppConfig::ConvertArgumentType(ITypeInfo* typeInfo)
			{
				auto td = typeInfo->GetTypeDescriptor();
				bool constRef =
					td == description::GetTypeDescriptor<WString>() ||
					td == description::GetTypeDescriptor<Value>() ||
					(
						typeInfo->GetDecorator() == ITypeInfo::SharedPtr && typeInfo->GetElementType()->GetDecorator() == ITypeInfo::Generic &&
						(
							td == description::GetTypeDescriptor<IValueFunctionProxy>() ||
							td == description::GetTypeDescriptor<IValueEnumerable>()
							)
					);

				if (constRef)
				{
					return L"const " + ConvertType(typeInfo) + L"&";
				}
				else
				{
					return ConvertType(typeInfo);
				}
			}

			WString WfCppConfig::DefaultValue(ITypeInfo* typeInfo)
			{
				switch (typeInfo->GetDecorator())
				{
				case ITypeInfo::RawPtr:
					return L"nullptr";
				case ITypeInfo::SharedPtr:
					return L"";
				case ITypeInfo::Nullable:
					return L"";
				case ITypeInfo::Generic:
					return L"";
				default:;
				}
				auto td = typeInfo->GetTypeDescriptor();
				if ((td->GetTypeDescriptorFlags()&TypeDescriptorFlags::EnumType) != TypeDescriptorFlags::Undefined)
				{
					return L"static_cast<" + ConvertType(typeInfo) + L">(0)";
				}
				if (td == description::GetTypeDescriptor<vint8_t>()) return L"0";
				if (td == description::GetTypeDescriptor<vint16_t>()) return L"0";
				if (td == description::GetTypeDescriptor<vint32_t>()) return L"0";
				if (td == description::GetTypeDescriptor<vint64_t>()) return L"0";
				if (td == description::GetTypeDescriptor<vuint8_t>()) return L"0";
				if (td == description::GetTypeDescriptor<vuint16_t>()) return L"0";
				if (td == description::GetTypeDescriptor<vuint32_t>()) return L"0";
				if (td == description::GetTypeDescriptor<vuint64_t>()) return L"0";
				if (td == description::GetTypeDescriptor<bool>()) return L"false";
				if (td == description::GetTypeDescriptor<float>()) return L"0.0f";
				if (td == description::GetTypeDescriptor<double>()) return L"0.0";
				return L"";
			}

			bool WfCppConfig::IsClassHasUserImplMethods(Ptr<WfClassDeclaration> decl, bool searchInternalClasses)
			{
				List<Ptr<WfDeclaration>> unprocessed;
				CopyFrom(unprocessed, decl->declarations);

				for (vint i = 0; i < unprocessed.Count(); i++)
				{
					auto memberDecl = unprocessed[i];
					if (auto cfe = memberDecl.Cast<WfVirtualCfeDeclaration>())
					{
						CopyFrom(unprocessed, cfe->expandedDeclarations, true);
					}
					else if (auto cse = memberDecl.Cast<WfVirtualCseDeclaration>())
					{
						CopyFrom(unprocessed, cse->expandedDeclarations, true);
					}
					else if (auto classDecl = memberDecl.Cast<WfClassDeclaration>())
					{
						if (searchInternalClasses)
						{
							CopyFrom(unprocessed, classDecl->declarations, true);
						}
					}
					else if (attributeEvaluator->GetAttribute(memberDecl->attributes, L"cpp", L"UserImpl"))
					{
						return true;
					}
				}
				return false;
			}

			vint WfCppConfig::CountClassNamespace(Ptr<WfClassDeclaration> decl)
			{
				vint result = 0;
				auto scope = manager->nodeScopes[decl.Obj()].Obj();
				while (scope)
				{
					if (scope->ownerNode.Cast<WfNamespaceDeclaration>())
					{
						result++;
					}
					scope = scope->parentScope.Obj();
				}
				return result;
			}

			void WfCppConfig::GetClassNamespace(Ptr<WfClassDeclaration> decl, collections::List<WString>& nss)
			{
				auto td = manager->declarationTypes[decl.Obj()].Obj();
				auto name = ConvertType(td);
				vint count = CountClassNamespace(decl);

				List<Ptr<RegexMatch>> matches;
				regexSplitName.Split(name, false, matches);

				CopyFrom(
					nss,
					From(matches)
						.Take(count)
						.Select([this](Ptr<RegexMatch> match)
						{
							return ConvertName(match->Result().Value());
						})
					);
			}

			WString WfCppConfig::GetClassBaseName(Ptr<WfClassDeclaration> decl)
			{
				auto td = manager->declarationTypes[decl.Obj()].Obj();
				auto name = ConvertType(td);
				vint count = CountClassNamespace(decl);

				List<Ptr<RegexMatch>> matches;
				regexSplitName.Split(name, false, matches);

				return From(matches)
					.Skip(count)
					.Select([this](Ptr<RegexMatch> match)
					{
						return ConvertName(match->Result().Value());
					})
					.Aggregate([](const WString& a, const WString& b)
					{
						return a + L"::" + b;
					});
			}

			WString WfCppConfig::WriteNamespace(stream::StreamWriter& writer, collections::List<WString>& nss, collections::List<WString>& nss2)
			{
				vint commonPrefix = 0;
				for (vint i = 0; i < nss.Count() && i < nss2.Count(); i++)
				{
					if (nss[i] == nss2[i])
					{
						commonPrefix++;
					}
					else
					{
						break;
					}
				}

				while (nss.Count() > commonPrefix)
				{
					for (vint i = 1; i < nss.Count(); i++)
					{
						writer.WriteChar(L'\t');
					}
					writer.WriteLine(L"}");
					nss.RemoveAt(nss.Count() - 1);
				}

				WString prefix;
				for (vint i = 0; i < nss.Count(); i++)
				{
					prefix += WString::FromChar(L'\t');
				}

				for (vint i = commonPrefix; i < nss2.Count(); i++)
				{
					writer.WriteString(prefix);
					writer.WriteString(L"namespace ");
					writer.WriteLine(nss2[i]);

					writer.WriteString(prefix);
					writer.WriteLine(L"{");

					nss.Add(nss2[i]);
					prefix += WString::FromChar(L'\t');
				}

				return prefix;
			}

			WString WfCppConfig::WriteNamespace(stream::StreamWriter& writer, const WString& fullName, collections::List<WString>& nss, WString& name)
			{
				List<Ptr<RegexMatch>> matches;
				regexSplitName.Split(fullName, false, matches);

				List<WString> nss2;
				CopyFrom(
					nss2,
					From(matches)
						.Select([this](Ptr<RegexMatch> match)
						{
							return ConvertName(match->Result().Value());
						})
					);

				name = nss2[nss2.Count() - 1];
				nss2.RemoveAt(nss2.Count() - 1);
				return WriteNamespace(writer, nss, nss2);
			}

			void WfCppConfig::WriteNamespaceEnd(stream::StreamWriter& writer, collections::List<WString>& nss)
			{
				while (nss.Count() > 0)
				{
					for (vint i = 1; i < nss.Count(); i++)
					{
						writer.WriteChar(L'\t');
					}
					writer.WriteLine(L"}");
					nss.RemoveAt(0);
				}
			}
		}
	}
}
