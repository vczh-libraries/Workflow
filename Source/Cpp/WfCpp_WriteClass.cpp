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

			void WfCppConfig::WriteHeader_ClassPreDecl(stream::StreamWriter& writer, Ptr<WfClassDeclaration> decl, const WString& name, const WString& prefix)
			{
				writer.WriteLine(prefix + L"class " + name + L";");
			}

			void WfCppConfig::WriteHeader_ClassPreDecl(stream::StreamWriter& writer, Ptr<WfClassDeclaration> decl, collections::List<WString>& nss)
			{
				auto td = manager->declarationTypes[decl.Obj()].Obj();
				WString name;
				auto prefix = WriteNamespace(writer, CppGetFullName(td), nss, name);
				WriteHeader_ClassPreDecl(writer, decl, name, prefix);
			}

			class WriteHeader_Class_FindClassDeclVisitor
				: public empty_visitor::DeclarationVisitor
			{
			public:
				List<Ptr<WfClassDeclaration>>&		unprocessed;

				WriteHeader_Class_FindClassDeclVisitor(List<Ptr<WfClassDeclaration>>& _unprocessed)
					:unprocessed(_unprocessed)
				{
				}

				void Dispatch(WfVirtualDeclaration* node)override
				{
					FOREACH(Ptr<WfDeclaration>, decl, node->expandedDeclarations)
					{
						decl->Accept(this);
					}
				}

				void Visit(WfClassDeclaration* node)override
				{
					unprocessed.Add(node);
				}
			};

			void WfCppConfig::WriteHeader_Class(stream::StreamWriter& writer, Ptr<WfClassDeclaration> decl, const WString& name, const WString& prefix)
			{
				auto td = manager->declarationTypes[decl.Obj()].Obj();

				writer.WriteString(prefix + L"class " + name + L" : ");
				switch (decl->kind)
				{
				case WfClassKind::Class:
				{
					vint count = td->GetBaseTypeDescriptorCount();
					bool hasClassBase = Range<vint>(0, count)
						.Any([=](vint index)
					{
						auto baseTd = td->GetBaseTypeDescriptor(index);
						return baseTd->GetTypeDescriptorFlags() == TypeDescriptorFlags::Class
							&& baseTd != description::GetTypeDescriptor<DescriptableObject>();
					});

					if (!hasClassBase)
					{
						writer.WriteString(L"public ::vl::Object, ");
					}
					for (vint i = 0; i < count; i++)
					{
						auto baseTd = td->GetBaseTypeDescriptor(i);
						switch (baseTd->GetTypeDescriptorFlags())
						{
						case TypeDescriptorFlags::Class:
							if (baseTd != description::GetTypeDescriptor<DescriptableObject>())
							{
								writer.WriteString(L"public " + ConvertType(baseTd) + L", ");
							}
							break;
						case TypeDescriptorFlags::Interface:
							writer.WriteString(L"public virtual " + ConvertType(baseTd) + L", ");
							break;
						default:;
						}
					}
				}
				break;
				case WfClassKind::Interface:
				{
					vint count = td->GetBaseTypeDescriptorCount();
					for (vint i = 0; i < count; i++)
					{
						writer.WriteString(L"public virtual " + ConvertType(td->GetBaseTypeDescriptor(i)) + L", ");
					}
				}
				break;
				}
				writer.WriteLine(L"public ::vl::reflection::Description<" + name + L">");
				writer.WriteLine(prefix + L"{");

				List<Ptr<WfClassDeclaration>> unprocessed;
				unprocessed.Add(decl);

				FOREACH(Ptr<WfAttribute>, attribute, attributeEvaluator->GetAttributes(decl->attributes, L"cpp", L"Friend"))
				{
					auto td = UnboxValue<ITypeDescriptor*>(attributeEvaluator->GetAttributeValue(attribute));

					auto scopeName = manager->typeNames[td];
					if (scopeName->declarations.Count() == 0)
					{
						writer.WriteLine(prefix + L"\tfriend class " + ConvertType(td) + L";");
					}
					else
					{
						auto friendDecl = scopeName->declarations[0].Cast<WfClassDeclaration>();
						unprocessed.Add(friendDecl);
					}
				}

				auto declTypeName = ConvertType(manager->declarationTypes[decl.Obj()].Obj());
				for (vint i = 0; i < unprocessed.Count(); i++)
				{
					auto current = unprocessed[i];
					if (current != decl)
					{
						auto currentTypeName = ConvertType(manager->declarationTypes[current.Obj()].Obj());

						bool isInternalClass = false;
						if (currentTypeName.Length() > declTypeName.Length() + 2)
						{
							if (currentTypeName.Left(declTypeName.Length() + 2) == declTypeName + L"::")
							{
								isInternalClass = true;
							}
						}
						if (!isInternalClass)
						{
							writer.WriteLine(prefix + L"\tfriend class " + currentTypeName + L";");
						}
					}

					vint index = classClosures.Keys().IndexOf(current.Obj());
					if (index != -1)
					{
						SortedList<WString> closureNames;
						CopyFrom(
							closureNames,
							From(classClosures.GetByIndex(index))
							.Select([&](Ptr<WfExpression> closure)
						{
							return (closure.Cast<WfNewInterfaceExpression>() ? L"class ::" : L"struct ::") +
								assemblyNamespace +
								L"::" +
								closureInfos[closure.Obj()]->lambdaClassName;
						})
						);
						FOREACH(WString, closureName, closureNames)
						{
							writer.WriteLine(prefix + L"\tfriend " + closureName + L";");
						}
					}

					WriteHeader_Class_FindClassDeclVisitor visitor(unprocessed);
					FOREACH(Ptr<WfDeclaration>, memberDecl, current->declarations)
					{
						memberDecl->Accept(&visitor);
					}
				}
				writer.WriteLine(L"#ifndef VCZH_DEBUG_NO_REFLECTION");
				writer.WriteLine(prefix + L"\tfriend struct ::vl::reflection::description::CustomTypeDescriptorSelector<" + name + L">;");
				writer.WriteLine(L"#endif");

				const vint PRIVATE = 0;
				const vint PROTECTED = 1;
				const vint PUBLIC = 2;
				vint accessor = -1;

				{
					vint index = enumDecls.Keys().IndexOf(decl.Obj());
					if (index != -1)
					{
						if (accessor != PUBLIC)
						{
							accessor = PUBLIC;
							writer.WriteLine(prefix + L"public:");
						}
						FOREACH(Ptr<WfEnumDeclaration>, decl, enumDecls.GetByIndex(index))
						{
							WriteHeader_Enum(writer, decl, ConvertName(decl->name.value), prefix + L"\t");
							writer.WriteLine(L"");
						}
					}
				}

				{
					vint index = structDecls.Keys().IndexOf(decl.Obj());
					if (index != -1)
					{
						if (accessor != PUBLIC)
						{
							accessor = PUBLIC;
							writer.WriteLine(prefix + L"public:");
						}
						FOREACH(Ptr<WfStructDeclaration>, decl, structDecls.GetByIndex(index))
						{
							WriteHeader_Struct(writer, decl, ConvertName(decl->name.value), prefix + L"\t");
							writer.WriteLine(L"");
						}
					}
				}

				{
					vint index = classDecls.Keys().IndexOf(decl.Obj());
					if (index != -1)
					{
						if (accessor != PUBLIC)
						{
							accessor = PUBLIC;
							writer.WriteLine(prefix + L"public:");
						}
						FOREACH(Ptr<WfClassDeclaration>, decl, classDecls.GetByIndex(index))
						{
							WriteHeader_ClassPreDecl(writer, decl, ConvertName(decl->name.value), prefix + L"\t");
						}
						writer.WriteLine(L"");
						FOREACH(Ptr<WfClassDeclaration>, decl, classDecls.GetByIndex(index))
						{
							WriteHeader_Class(writer, decl, ConvertName(decl->name.value), prefix + L"\t");
						}
					}
				}

				FOREACH(Ptr<WfDeclaration>, memberDecl, decl->declarations)
				{
					vint memberAccessor = PUBLIC;
					if (attributeEvaluator->GetAttribute(memberDecl->attributes, L"cpp", L"Private"))
					{
						memberAccessor = PRIVATE;
					}
					else if (attributeEvaluator->GetAttribute(memberDecl->attributes, L"cpp", L"Protected"))
					{
						memberAccessor = PROTECTED;
					}

					if (accessor != memberAccessor)
					{
						accessor = memberAccessor;
						switch (accessor)
						{
						case PRIVATE:
							writer.WriteLine(prefix + L"private:");
							break;
						case PROTECTED:
							writer.WriteLine(prefix + L"protected:");
							break;
						case PUBLIC:
							writer.WriteLine(prefix + L"public:");
							break;
						}
					}
					GenerateClassMemberDecl(this, writer, ConvertName(decl->name.value), memberDecl, prefix + L"\t", false);
				}

				writer.WriteLine(prefix + L"};");
			}

			WString WfCppConfig::WriteHeader_Class(stream::StreamWriter& writer, Ptr<WfClassDeclaration> decl, collections::List<WString>& nss)
			{
				auto td = manager->declarationTypes[decl.Obj()].Obj();
				WString name;
				auto prefix = WriteNamespace(writer, CppGetFullName(td), nss, name);
				WriteHeader_Class(writer, decl, name, prefix);
				return prefix;
			}

			void WfCppConfig::WriteHeader_TopLevelClass(stream::StreamWriter& writer, Ptr<WfClassDeclaration> decl, collections::List<WString>& nss)
			{
				auto prefix = WriteHeader_Class(writer, decl, nss);
				List<Ptr<WfClassDeclaration>> classes;
				classes.Add(decl);
				vint processed = 0;
				while (processed < classes.Count())
				{
					auto current = classes[processed++];
					{
						vint index = enumDecls.Keys().IndexOf(current.Obj());
						if (index != -1)
						{
							FOREACH(Ptr<WfEnumDeclaration>, enumDecl, enumDecls.GetByIndex(index))
							{
								auto td = manager->declarationTypes[enumDecl.Obj()].Obj();
								WriteHeader_EnumOp(writer, enumDecl, ConvertType(td), prefix);
							}
						}
					}
					{
						vint index = structDecls.Keys().IndexOf(current.Obj());
						if (index != -1)
						{
							FOREACH(Ptr<WfStructDeclaration>, structDecl, structDecls.GetByIndex(index))
							{
								auto td = manager->declarationTypes[structDecl.Obj()].Obj();
								WriteHeader_StructOp(writer, structDecl, ConvertType(td), prefix);
							}
						}
					}
					{
						vint index = classDecls.Keys().IndexOf(current.Obj());
						if (index != -1)
						{
							CopyFrom(classes, classDecls.GetByIndex(index), true);
						}
					}
				}
			}

			bool WfCppConfig::WriteCpp_ClassMember(stream::StreamWriter& writer, Ptr<WfClassDeclaration> decl, Ptr<WfDeclaration> memberDecl, collections::List<WString>& nss)
			{
				List<WString> nss2;
				GetClassNamespace(decl, nss2);
				auto prefix = WriteNamespace(writer, nss, nss2);

				auto td = manager->declarationTypes[decl.Obj()].Obj();
				auto classFullName = CppGetFullName(td);
				return GenerateClassMemberImpl(this, writer, GetClassBaseName(decl), ConvertName(decl->name.value), classFullName , memberDecl, prefix);
			}

			void WfCppConfig::WriteCpp_Class(stream::StreamWriter& writer, Ptr<WfClassDeclaration> decl, collections::List<WString>& nss)
			{
				List<Ptr<WfClassDeclaration>> classes;
				classes.Add(decl);
				vint processed = 0;
				while (processed < classes.Count())
				{
					auto current = classes[processed++];

					writer.WriteLine(L"/***********************************************************************");
					writer.WriteLine(L"Class (" + CppGetFullName(manager->declarationTypes[current.Obj()].Obj()) + L")");
					writer.WriteLine(L"***********************************************************************/");
					writer.WriteLine(L"");

					FOREACH(Ptr<WfDeclaration>, memberDecl, current->declarations)
					{
						if (WriteCpp_ClassMember(writer, current, memberDecl, nss))
						{
							writer.WriteLine(L"");
						}
					}

					vint index = classDecls.Keys().IndexOf(current.Obj());
					if (index != -1)
					{
						CopyFrom(classes, classDecls.GetByIndex(index), true);
					}
				}
			}
		}
	}
}
