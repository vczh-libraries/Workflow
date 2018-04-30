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

			void WfCppConfig::Collect()
			{
				FOREACH(Ptr<WfModule>, module, manager->GetModules())
				{
					CollectModule(this, module);
				}

				FOREACH(Ptr<WfExpression>, lambda, lambdaExprs.Keys())
				{
					auto closureInfo = CollectClosureInfo(lambda);
					closureInfo->lambdaClassName = lambdaExprs[lambda.Obj()];
					closureInfos.Add(lambda, closureInfo);
				}

				FOREACH(Ptr<WfNewInterfaceExpression>, classExpr, classExprs.Keys())
				{
					auto closureInfo = CollectClosureInfo(classExpr);
					closureInfo->lambdaClassName = classExprs[classExpr.Obj()];
					closureInfos.Add(classExpr, closureInfo);
				}
			}

			template<typename T, typename U>
			void WfCppConfig::SortInternal(collections::List<Ptr<T>>& decls, U dependOn)
			{
				List<ITypeDescriptor*> tds;
				Dictionary<ITypeDescriptor*, Ptr<T>> tdMap;
				FOREACH_INDEXER(Ptr<T>, decl, index, decls)
				{
					auto td = manager->declarationTypes[decl.Obj()].Obj();
					tds.Add(td);
					tdMap.Add(td, decl);
				}
				// key depends on values
				Group<ITypeDescriptor*, ITypeDescriptor*> deps;
				FOREACH(ITypeDescriptor*, td, tds)
				{
					deps.Add(td, td);
				}

				for (vint i = 0; i < tds.Count(); i++)
				{
					for (vint j = 0; j < tds.Count(); j++)
					{
						if (dependOn(tds[i], tds[j]))
						{
							if (!deps.Contains(tds[i], tds[j]))
							{
								deps.Add(tds[i], tds[j]);
							}
						}
					}
				}

				tds.Clear();
				while (deps.Count() != 0)
				{
					List<ITypeDescriptor*> selected;

					for (vint i = 0; i < deps.Count(); i++)
					{
						if (deps.GetByIndex(i).Count() == 1)
						{
							selected.Add(deps.Keys()[i]);
						}
					}

					for (vint i = deps.Count() - 1; i >= 0; i--)
					{
						if (deps.GetByIndex(i).Count() == 1)
						{
							deps.Remove(deps.Keys()[i]);
						}
					}

					for (vint i = deps.Count() - 1; i >= 0; i--)
					{
						for (vint j = 0; j < selected.Count(); j++)
						{
							deps.Remove(deps.Keys()[i], selected[j]);
						}
					}

					CopyFrom(selected, From(selected).OrderBy([](ITypeDescriptor* a, ITypeDescriptor* b) {return WString::Compare(a->GetTypeName(), b->GetTypeName()); }));
					CopyFrom(tds, selected, true);
					CHECK_ERROR(selected.Count() > 0, L"WfCppConfig::SortInternal<T, U>(collections::List<Ptr<T>>&, U)#Internal error: Unexpected circle dependency found, which should be cought by the Workflow semantic analyzer.");
				}

				CopyFrom(decls, From(tds).Select([&](ITypeDescriptor* td) {return tdMap[td]; }));
			}

			void WfCppConfig::Sort(collections::List<Ptr<WfStructDeclaration>>& structDecls)
			{
				SortInternal(structDecls, [](ITypeDescriptor* type, ITypeDescriptor* field)
				{
					vint count = type->GetPropertyCount();
					for (vint i = 0; i < count; i++)
					{
						auto propType = type->GetProperty(i)->GetReturn();
						if (propType->GetDecorator() == ITypeInfo::TypeDescriptor)
						{
							auto td = propType->GetTypeDescriptor();
							if (td == field)
							{
								return true;
							}
							else if (INVLOC.StartsWith(td->GetTypeName(), field->GetTypeName() + L"::", Locale::None))
							{
								return true;
							}
						}
					}
					return false;
				});
			}

			void WfCppConfig::Sort(collections::List<Ptr<WfClassDeclaration>>& classDecls)
			{
				SortInternal(classDecls, [](ITypeDescriptor* derived, ITypeDescriptor* base)
				{
					vint count = derived->GetBaseTypeDescriptorCount();
					for (vint i = 0; i < count; i++)
					{
						auto td = derived->GetBaseTypeDescriptor(i);
						if (td == base)
						{
							return true;
						}
						else if (INVLOC.StartsWith(td->GetTypeName(), base->GetTypeName() + L"::", Locale::None))
						{
							return true;
						}
					}
					return false;
				});
			}

			WfCppConfig::WfCppConfig(analyzer::WfLexicalScopeManager* _manager, const WString& _assemblyName, const WString& _assemblyNamespace)
				:manager(_manager)
				, regexSplitName(L"::")
				, regexSpecialName(L"/<(<category>/w+)(-(<category>/w+))*/>(<name>/w*)")
				, regexTemplate(L", /$Arguments|/$Arguments, |/$/l+")
				, assemblyName(_assemblyName)
				, assemblyNamespace(_assemblyNamespace)
			{
				attributeEvaluator = MakePtr<WfAttributeEvaluator>(manager);
				Collect();
				for (vint i = 0; i < structDecls.Count(); i++)
				{
					const auto& values = structDecls.GetByIndex(i);
					Sort(const_cast<List<Ptr<WfStructDeclaration>>&>(values));
				}
				for (vint i = 0; i < topLevelClassDeclsForFiles.Count(); i++)
				{
					const auto& values = topLevelClassDeclsForFiles.GetByIndex(i);
					Sort(const_cast<List<Ptr<WfClassDeclaration>>&>(values));
				}
				for (vint i = 0; i < classDecls.Count(); i++)
				{
					const auto& values = classDecls.GetByIndex(i);
					Sort(const_cast<List<Ptr<WfClassDeclaration>>&>(values));
				}
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
				GenerateStatement(this, MakePtr<FunctionRecord>(), writer, stat, prefix, WString(L"\t", false), expectedType);
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
						+ From(match->Groups()[L"category"])
							.Select([](const RegexString& rs)
							{
								return rs.Value();
							})
							.Aggregate([](const WString& a, const WString& b)
							{
								return a + L"_" + b;
							})
						+ L"_" + match->Groups()[L"name"][0].Value();
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
				return ConvertNameInternal(name, WString(L"__vwsn_", false), false);
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
				CHECK_ERROR(typeInfo->GetDecorator() == ITypeInfo::Generic, L"WfCppConfig::ConvertFunctionType(ITypeInfo*)#Wrong function type.");
				CHECK_ERROR(typeInfo->GetTypeDescriptor() == description::GetTypeDescriptor<IValueFunctionProxy>(), L"WfCppConfig::ConvertFunctionType(ITypeInfo*)#Wrong function type.");

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

			WString WfCppConfig::ConvertType(ITypeInfo* typeInfo)
			{
				switch (typeInfo->GetDecorator())
				{
				case ITypeInfo::RawPtr:
					return ConvertType(typeInfo->GetElementType()) + L"*";
				case ITypeInfo::SharedPtr:
					if (typeInfo->GetElementType()->GetDecorator() == ITypeInfo::Generic)
					{
						if (typeInfo->GetTypeDescriptor() == description::GetTypeDescriptor<IValueFunctionProxy>())
						{
							return ConvertType(typeInfo->GetElementType());
						}
						else if (typeInfo->GetTypeDescriptor() == description::GetTypeDescriptor<IValueEnumerable>())
						{
							return ConvertType(typeInfo->GetElementType());
						}
					}
					return L"::vl::Ptr<" + ConvertType(typeInfo->GetElementType()) + L">";
				case ITypeInfo::Nullable:
					return L"::vl::Nullable<" + ConvertType(typeInfo->GetElementType()) + L">";
				case ITypeInfo::Generic:
					if (typeInfo->GetTypeDescriptor() == description::GetTypeDescriptor<IValueFunctionProxy>())
					{
						return L"::vl::Func<" + ConvertFunctionType(typeInfo) + L">";
					}
					else if(typeInfo->GetTypeDescriptor() == description::GetTypeDescriptor<IValueEnumerable>())
					{
						return L"::vl::collections::LazyList<" + ConvertType(typeInfo->GetGenericArgument(0)) + L">";
					}
					else
					{
						return ConvertType(typeInfo->GetElementType());
					}
				default:;
				}
				return ConvertType(typeInfo->GetTypeDescriptor());
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
					prefix += L'\t';
				}

				for (vint i = commonPrefix; i < nss2.Count(); i++)
				{
					writer.WriteString(prefix);
					writer.WriteString(L"namespace ");
					writer.WriteLine(nss2[i]);

					writer.WriteString(prefix);
					writer.WriteLine(L"{");

					nss.Add(nss2[i]);
					prefix += L'\t';
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
