#include "WfAnalyzer.h"

namespace vl
{
	namespace workflow
	{
		namespace analyzer
		{
			using namespace collections;
			using namespace reflection;
			using namespace reflection::description;

/***********************************************************************
GetTypeFlag
***********************************************************************/

			TypeFlag GetTypeFlag(reflection::description::ITypeDescriptor* typeDescriptor)
			{
				if (typeDescriptor == GetTypeDescriptor<bool>())		return TypeFlag::Bool;
				if (typeDescriptor == GetTypeDescriptor<vint8_t>())		return TypeFlag::I1;
				if (typeDescriptor == GetTypeDescriptor<vint16_t>())	return TypeFlag::I2;
				if (typeDescriptor == GetTypeDescriptor<vint32_t>())	return TypeFlag::I4;
				if (typeDescriptor == GetTypeDescriptor<vint64_t>())	return TypeFlag::I8;
				if (typeDescriptor == GetTypeDescriptor<vuint8_t>())	return TypeFlag::U1;
				if (typeDescriptor == GetTypeDescriptor<vuint16_t>())	return TypeFlag::U2;
				if (typeDescriptor == GetTypeDescriptor<vuint32_t>())	return TypeFlag::U4;
				if (typeDescriptor == GetTypeDescriptor<vuint64_t>())	return TypeFlag::U8;
				if (typeDescriptor == GetTypeDescriptor<float>())		return TypeFlag::F4;
				if (typeDescriptor == GetTypeDescriptor<double>())		return TypeFlag::F8;
				if (typeDescriptor == GetTypeDescriptor<WString>())		return TypeFlag::String;
				return TypeFlag::Others;
			}

			TypeFlag GetTypeFlag(reflection::description::ITypeInfo* typeInfo)
			{
				ITypeDescriptor* td = typeInfo->GetTypeDescriptor();
				return GetTypeFlag(td);
			}

/***********************************************************************
CreateTypeInfoFromTypeFlag
***********************************************************************/

			Ptr<reflection::description::ITypeInfo> CreateTypeInfoFromTypeFlag(TypeFlag flag)
			{
				switch (flag)
				{
				case TypeFlag::Bool:	return TypeInfoRetriver<bool>::CreateTypeInfo();
				case TypeFlag::I1:		return TypeInfoRetriver<vint8_t>::CreateTypeInfo();
				case TypeFlag::I2:		return TypeInfoRetriver<vint16_t>::CreateTypeInfo();
				case TypeFlag::I4:		return TypeInfoRetriver<vint32_t>::CreateTypeInfo();
				case TypeFlag::I8:		return TypeInfoRetriver<vint64_t>::CreateTypeInfo();
				case TypeFlag::U1:		return TypeInfoRetriver<vuint8_t>::CreateTypeInfo();
				case TypeFlag::U2:		return TypeInfoRetriver<vuint16_t>::CreateTypeInfo();
				case TypeFlag::U4:		return TypeInfoRetriver<vuint32_t>::CreateTypeInfo();
				case TypeFlag::U8:		return TypeInfoRetriver<vuint64_t>::CreateTypeInfo();
				case TypeFlag::F4:		return TypeInfoRetriver<float>::CreateTypeInfo();
				case TypeFlag::F8:		return TypeInfoRetriver<double>::CreateTypeInfo();
				case TypeFlag::String:	return TypeInfoRetriver<WString>::CreateTypeInfo();
				default:;
				}
				return 0;
			}

/***********************************************************************
GetTypeFromTypeInfo
***********************************************************************/

			Ptr<WfType> GetTypeFromTypeInfo(reflection::description::ITypeInfo* typeInfo)
			{
				switch (typeInfo->GetDecorator())
				{
				case ITypeInfo::RawPtr:
					{
						Ptr<WfType> element = GetTypeFromTypeInfo(typeInfo->GetElementType());
						if (element)
						{
							Ptr<WfRawPointerType> type = new WfRawPointerType;
							type->element = element;
							return type;
						}
						return 0;
					}
				case ITypeInfo::SharedPtr:
					{
						if (typeInfo->GetElementType()->GetDecorator() == ITypeInfo::Generic)
						{
							return GetTypeFromTypeInfo(typeInfo->GetElementType());
						}
						else
						{
							Ptr<WfType> element = GetTypeFromTypeInfo(typeInfo->GetElementType());
							if (element)
							{
								Ptr<WfSharedPointerType> type = new WfSharedPointerType;
								type->element = element;
								return type;
							}
							return 0;
						}
					}
				case ITypeInfo::Nullable:
					{
						Ptr<WfType> element = GetTypeFromTypeInfo(typeInfo->GetElementType());
						if (element)
						{
							Ptr<WfNullableType> type = new WfNullableType;
							type->element = element;
							return type;
						}
						return 0;
					}
				case ITypeInfo::TypeDescriptor:
					{
						List<WString> fragments;
						{
							WString name = typeInfo->GetTypeDescriptor()->GetTypeName();
							const wchar_t* reading = name.Buffer();
							while (reading)
							{
								const wchar_t* delimiter = wcsstr(reading, L"::");
								if (delimiter)
								{
									fragments.Add(WString(reading, vint(delimiter - reading)));
									reading = delimiter + 2;
								}
								else
								{
									fragments.Add(reading);
									break;
								}
							}
						}

						Ptr<WfType> parentType;
						FOREACH(WString, fragment, fragments)
						{
							if (!parentType)
							{
								Ptr<WfTopQualifiedType> type = new WfTopQualifiedType;
								type->name.value = fragment;
								parentType = type;
							}
							else
							{
								Ptr<WfChildType> type = new WfChildType;
								type->parent = parentType;
								type->name.value = fragment;
								parentType = type;
							}
						}
						return parentType;
					}
				case ITypeInfo::Generic:
					{
						if (typeInfo->GetElementType()->GetDecorator() == ITypeInfo::TypeDescriptor)
						{
							if (typeInfo->GetTypeDescriptor() == GetTypeDescriptor<IValueEnumerable>())
							{
								if (typeInfo->GetGenericArgumentCount() == 1)
								{
									if (Ptr<WfType> elementType = GetTypeFromTypeInfo(typeInfo->GetGenericArgument(0)))
									{
										Ptr<WfEnumerableType> type = new WfEnumerableType;
										type->element = elementType;
										return type;
									}
								}
							}
							else if (typeInfo->GetTypeDescriptor() == GetTypeDescriptor<IValueReadonlyList>())
							{
								if (typeInfo->GetGenericArgumentCount() == 1)
								{
									if (Ptr<WfType> valueType = GetTypeFromTypeInfo(typeInfo->GetGenericArgument(0)))
									{
										Ptr<WfMapType> type = new WfMapType;
										type->writability = WfMapWritability::Readonly;
										type->value = valueType;
										return type;
									}
								}
							}
							else if (typeInfo->GetTypeDescriptor() == GetTypeDescriptor<IValueList>())
							{
								if (typeInfo->GetGenericArgumentCount() == 1)
								{
									if (Ptr<WfType> valueType = GetTypeFromTypeInfo(typeInfo->GetGenericArgument(0)))
									{
										Ptr<WfMapType> type = new WfMapType;
										type->writability = WfMapWritability::Writable;
										type->value = valueType;
										return type;
									}
								}
							}
							else if (typeInfo->GetTypeDescriptor() == GetTypeDescriptor<IValueReadonlyDictionary>())
							{
								if (typeInfo->GetGenericArgumentCount() == 2)
								{
									if (Ptr<WfType> keyType = GetTypeFromTypeInfo(typeInfo->GetGenericArgument(0)))
									if (Ptr<WfType> valueType = GetTypeFromTypeInfo(typeInfo->GetGenericArgument(1)))
									{
										Ptr<WfMapType> type = new WfMapType;
										type->writability = WfMapWritability::Readonly;
										type->key = keyType;
										type->value = valueType;
										return type;
									}
								}
							}
							else if (typeInfo->GetTypeDescriptor() == GetTypeDescriptor<IValueDictionary>())
							{
								if (typeInfo->GetGenericArgumentCount() == 2)
								{
									if (Ptr<WfType> keyType = GetTypeFromTypeInfo(typeInfo->GetGenericArgument(0)))
									if (Ptr<WfType> valueType = GetTypeFromTypeInfo(typeInfo->GetGenericArgument(1)))
									{
										Ptr<WfMapType> type = new WfMapType;
										type->writability = WfMapWritability::Writable;
										type->key = keyType;
										type->value = valueType;
										return type;
									}
								}
							}
							else if (typeInfo->GetTypeDescriptor() == GetTypeDescriptor<IValueFunctionProxy>())
							{
								if (typeInfo->GetGenericArgumentCount() >= 1)
								{
									if (Ptr<WfType> returnType = GetTypeFromTypeInfo(typeInfo->GetGenericArgument(0)))
									{
										Ptr<WfFunctionType> type = new WfFunctionType;
										type->result = GetTypeFromTypeInfo(typeInfo->GetGenericArgument(0));
										for (vint i = 1; i < typeInfo->GetGenericArgumentCount(); i++)
										{
											if (Ptr<WfType> argumentType = GetTypeFromTypeInfo(typeInfo->GetGenericArgument(i)))
											{
												type->arguments.Add(argumentType);
											}
											else
											{
												return 0;
											}
										}
										return type;
									}
								}
							}
						}
						return 0;
					}
				default:
					return 0;
				}
			}

/***********************************************************************
GetScopeNameFromReferenceType
***********************************************************************/

			class GetScopeNameFromReferenceTypeVisitor : public Object, public WfType::IVisitor
			{
			public:
				WfLexicalScope*				scope;
				Ptr<WfLexicalScopeName>		result;

				GetScopeNameFromReferenceTypeVisitor(WfLexicalScope* _scope)
					:scope(_scope)
				{
				}

				Ptr<WfLexicalScopeName> Call(WfType* node)
				{
					node->Accept(this);
					Ptr<WfLexicalScopeName> scopeName = result;
					result = 0;
					return scopeName;
				}

				void Visit(WfPredefinedType* node)override
				{
					WString name;
					switch (node->name)
					{
					case WfPredefinedTypeName::Void:
						name = L"Void";
						break;
					case WfPredefinedTypeName::Object:
						name = L"Object";
						break;
					case WfPredefinedTypeName::Interface:
						name = L"Interface";
						break;
					case WfPredefinedTypeName::Int:
#if defined VCZH_64
						name = L"Int64";
#else
						name = L"Int32";
#endif
						break;
					case WfPredefinedTypeName::UInt:
#if defined VCZH_64
						name = L"UInt64";
#else
						name = L"UInt32";
#endif
						break;
					case WfPredefinedTypeName::Float:
						name = L"Single";
						break;
					case WfPredefinedTypeName::Double:
						name = L"Double";
						break;
					case WfPredefinedTypeName::String:
						name = L"String";
						break;
					case WfPredefinedTypeName::Char:
						name = L"Char";
						break;
					case WfPredefinedTypeName::Bool:
						name = L"Boolean";
						break;
					default:
						CHECK_FAIL(L"GetScopeNameFromReferenceTypeVisitor::Visit(WfPredefinedType*)#Internal error, ValidateTypeStructure function should check correctly.");
					}

					Ptr<WfTopQualifiedType> ns = new WfTopQualifiedType;
					ns->name.value = L"system";

					Ptr<WfChildType> type = new WfChildType;
					type->parent = ns;
					type->name.value = name;

					type->Accept(this);
				}

				void Visit(WfTopQualifiedType* node)override
				{
					auto manager = scope->FindManager();
					if (manager->globalName)
					{
						vint index = manager->globalName->children.Keys().IndexOf(node->name.value);
						if (index != -1)
						{
							result = manager->globalName->children.Values()[index];
							return;
						}
					}
					manager->errors.Add(WfErrors::TopQualifiedSymbolNotExists(node, node->name.value));
				}

				void Visit(WfReferenceType* node)override
				{
					auto manager = scope->FindManager();
					List<ResolveExpressionResult> results;
					manager->ResolveName(scope, node->name.value, results);

					if (results.Count() > 1)
					{
						manager->errors.Add(WfErrors::TooManyTargets(node, results, node->name.value));
					}
					else if (results.Count() == 1)
					{
						if (auto scopeName = results[0].scopeName)
						{
							result = scopeName;
						}
						else
						{
							manager->errors.Add(WfErrors::TypeNotExists(node, results[0].symbol));
						}
					}
					else
					{
						manager->errors.Add(WfErrors::ReferenceNotExists(node, node->name.value));
					}
				}

				void Visit(WfRawPointerType* node)override
				{
					CHECK_FAIL(L"GetScopeNameFromReferenceTypeVisitor::Visit(WfRawPointerType*)#Internal error, ValidateTypeStructure function should check correctly.");
				}

				void Visit(WfSharedPointerType* node)override
				{
					CHECK_FAIL(L"GetScopeNameFromReferenceTypeVisitor::Visit(WfSharedPointerType*)#Internal error, ValidateTypeStructure function should check correctly.");
				}

				void Visit(WfNullableType* node)override
				{
					CHECK_FAIL(L"GetScopeNameFromReferenceTypeVisitor::Visit(WfNullableType*)#Internal error, ValidateTypeStructure function should check correctly.");
				}

				void Visit(WfEnumerableType* node)override
				{
					CHECK_FAIL(L"GetScopeNameFromReferenceTypeVisitor::Visit(WfEnumerableType*)#Internal error, ValidateTypeStructure function should check correctly.");
				}

				void Visit(WfMapType* node)override
				{
					CHECK_FAIL(L"GetScopeNameFromReferenceTypeVisitor::Visit(WfMapType*)#Internal error, ValidateTypeStructure function should check correctly.");
				}

				void Visit(WfFunctionType* node)override
				{
					CHECK_FAIL(L"GetScopeNameFromReferenceTypeVisitor::Visit(WfFunctionType*)#Internal error, ValidateTypeStructure function should check correctly.");
				}

				void Visit(WfChildType* node)override
				{
					if (Ptr<WfLexicalScopeName> scopeName = Call(node->parent.Obj()))
					{
						vint index = scopeName->children.Keys().IndexOf(node->name.value);
						if (index != -1)
						{
							result = scopeName->children.Values()[index];
							return;
						}
						scope->FindManager()->errors.Add(WfErrors::ChildSymbolNotExists(node, scopeName, node->name.value));
					}
				}

				static Ptr<WfLexicalScopeName> Execute(WfLexicalScope* scope, WfType* type)
				{
					return GetScopeNameFromReferenceTypeVisitor(scope).Call(type);
				}
			};

			Ptr<WfLexicalScopeName> GetScopeNameFromReferenceType(WfLexicalScope* scope, Ptr<WfType> type)
			{
				return GetScopeNameFromReferenceTypeVisitor::Execute(scope, type.Obj());
			}

/***********************************************************************
CreateTypeInfoFromType
***********************************************************************/

			class CreateTypeInfoFromTypeVisitor : public Object, public WfType::IVisitor
			{
			public:
				WfLexicalScope*				scope;
				Ptr<ITypeInfo>				result;

				CreateTypeInfoFromTypeVisitor(WfLexicalScope* _scope)
					:scope(_scope)
				{
				}

				Ptr<ITypeInfo> Call(WfType* node, bool checkTypeForValue)
				{
					node->Accept(this);
					Ptr<ITypeInfo> typeInfo = result;
					result = 0;
					if (typeInfo)
					{
						auto manager = scope->FindManager();
						switch (typeInfo->GetDecorator())
						{
						case ITypeInfo::RawPtr:
						case ITypeInfo::SharedPtr:
							{
								auto element = typeInfo->GetElementType();
								if (element->GetDecorator() == ITypeInfo::Generic)
								{
									element = element->GetElementType();
								}

								if (element->GetDecorator() == ITypeInfo::TypeDescriptor)
								{
									if (element->GetTypeDescriptor()->GetValueSerializer() == 0)
									{
										goto RAW_SHARED_POINTER_CORRECT;
									}
								}

								if (typeInfo->GetDecorator() == ITypeInfo::RawPtr)
								{
									manager->errors.Add(WfErrors::RawPointerToNonReferenceType(node, element));
								}
								else
								{
									manager->errors.Add(WfErrors::SharedPointerToNonReferenceType(node, element));
								}
							RAW_SHARED_POINTER_CORRECT:
								;
							}
							break;
						case ITypeInfo::Nullable:
							{
								auto element = typeInfo->GetElementType();
								if (element->GetDecorator() == ITypeInfo::Generic)
								{
									element = element->GetElementType();
								}

								if (element->GetDecorator() == ITypeInfo::TypeDescriptor)
								{
									if (element->GetTypeDescriptor()->GetValueSerializer() != 0)
									{
										goto NULLABLE_CORRECT;
									}
								}

								manager->errors.Add(WfErrors::NullableToNonReferenceType(node, element));
							NULLABLE_CORRECT:
								;
							}
							break;
						case ITypeInfo::TypeDescriptor:
							if (checkTypeForValue)
							{
								if (typeInfo->GetTypeDescriptor()->GetValueSerializer() == 0 && typeInfo->GetTypeDescriptor() != description::GetTypeDescriptor<Value>())
								{
									manager->errors.Add(WfErrors::TypeNotForValue(node, typeInfo.Obj()));
								}
							}
							break;
						case ITypeInfo::Generic:
							if (checkTypeForValue)
							{
								manager->errors.Add(WfErrors::TypeNotForValue(node, typeInfo.Obj()));
							}
							break;
						}
					}
					return typeInfo;
				}

				void VisitReferenceType(WfType* node)
				{
					if (auto scopeName = GetScopeNameFromReferenceTypeVisitor::Execute(scope, node))
					{
						if (scopeName->typeDescriptor)
						{
							Ptr<TypeInfoImpl> typeInfo = new TypeInfoImpl(ITypeInfo::TypeDescriptor);
							typeInfo->SetTypeDescriptor(scopeName->typeDescriptor);
							result = typeInfo;
						}
						else
						{
							scope->FindManager()->errors.Add(WfErrors::TypeNotExists(node, scopeName));
						}
					}
				}

				void Visit(WfPredefinedType* node)override
				{
					ITypeDescriptor* typeDescriptor = 0;
					switch (node->name)
					{
					case WfPredefinedTypeName::Void:
						typeDescriptor = description::GetTypeDescriptor<VoidValue>();
						break;
					case WfPredefinedTypeName::Object:
						typeDescriptor = description::GetTypeDescriptor<Value>();
						break;
					case WfPredefinedTypeName::Interface:
						typeDescriptor = description::GetTypeDescriptor<IDescriptable>();
						break;
					case WfPredefinedTypeName::Int:
						typeDescriptor = description::GetTypeDescriptor<vint>();
						break;
					case WfPredefinedTypeName::UInt:
						typeDescriptor = description::GetTypeDescriptor<vuint>();
						break;
					case WfPredefinedTypeName::Float:
						typeDescriptor = description::GetTypeDescriptor<float>();
						break;
					case WfPredefinedTypeName::Double:
						typeDescriptor = description::GetTypeDescriptor<double>();
						break;
					case WfPredefinedTypeName::String:
						typeDescriptor = description::GetTypeDescriptor<WString>();
						break;
					case WfPredefinedTypeName::Char:
						typeDescriptor = description::GetTypeDescriptor<wchar_t>();
						break;
					case WfPredefinedTypeName::Bool:
						typeDescriptor = description::GetTypeDescriptor<bool>();
						break;
					default:
						CHECK_FAIL(L"CreateTypeInfoFromTypeVisitor::Visit(WfPredefinedType*)#Internal error, ValidateTypeStructure function should check correctly.");
					}
					if (typeDescriptor)
					{
						Ptr<TypeInfoImpl> typeInfo = new TypeInfoImpl(ITypeInfo::TypeDescriptor);
						typeInfo->SetTypeDescriptor(typeDescriptor);
						result = typeInfo;
					}
				}

				void Visit(WfTopQualifiedType* node)override
				{
					VisitReferenceType(node);
				}

				void Visit(WfReferenceType* node)override
				{
					VisitReferenceType(node);
				}

				void Visit(WfRawPointerType* node)override
				{
					if (Ptr<ITypeInfo> element = Call(node->element.Obj(), false))
					{
						Ptr<TypeInfoImpl> typeInfo = new TypeInfoImpl(ITypeInfo::RawPtr);
						typeInfo->SetElementType(element);
						result = typeInfo;
					}
				}

				void Visit(WfSharedPointerType* node)override
				{
					if (Ptr<ITypeInfo> element = Call(node->element.Obj(), false))
					{
						Ptr<TypeInfoImpl> typeInfo = new TypeInfoImpl(ITypeInfo::SharedPtr);
						typeInfo->SetElementType(element);
						result = typeInfo;
					}
				}

				void Visit(WfNullableType* node)override
				{
					if (Ptr<ITypeInfo> element = Call(node->element.Obj(), false))
					{
						Ptr<TypeInfoImpl> typeInfo = new TypeInfoImpl(ITypeInfo::Nullable);
						typeInfo->SetElementType(element);
						result = typeInfo;
					}
				}

				void Visit(WfEnumerableType* node)override
				{
					if (Ptr<ITypeInfo> element = Call(node->element.Obj(), true))
					{
						Ptr<TypeInfoImpl> enumerableTypeInfo = new TypeInfoImpl(ITypeInfo::TypeDescriptor);
						enumerableTypeInfo->SetTypeDescriptor(description::GetTypeDescriptor<IValueEnumerable>());

						Ptr<TypeInfoImpl> genericTypeInfo = new TypeInfoImpl(ITypeInfo::Generic);
						genericTypeInfo->SetElementType(enumerableTypeInfo);
						genericTypeInfo->AddGenericArgument(element);

						Ptr<TypeInfoImpl> shared = new TypeInfoImpl(ITypeInfo::SharedPtr);
						shared->SetElementType(genericTypeInfo);
						result = shared;
					}
				}

				void Visit(WfMapType* node)override
				{
					Ptr<ITypeInfo> key, value;
					if (!(value = Call(node->value.Obj(), true))) return;
					if (node->key)
					{
						if (!(key = Call(node->key.Obj(), true))) return;
					}
					
					Ptr<TypeInfoImpl> mapTypeInfo = new TypeInfoImpl(ITypeInfo::TypeDescriptor);
					if (node->writability == WfMapWritability::Writable)
					{
						if (node->key)
						{
							mapTypeInfo->SetTypeDescriptor(description::GetTypeDescriptor<IValueDictionary>());
						}
						else
						{
							mapTypeInfo->SetTypeDescriptor(description::GetTypeDescriptor<IValueList>());
						}
					}
					else
					{
						if (node->key)
						{
							mapTypeInfo->SetTypeDescriptor(description::GetTypeDescriptor<IValueReadonlyDictionary>());
						}
						else
						{
							mapTypeInfo->SetTypeDescriptor(description::GetTypeDescriptor<IValueReadonlyList>());
						}
					}

					Ptr<TypeInfoImpl> genericTypeInfo = new TypeInfoImpl(ITypeInfo::Generic);
					genericTypeInfo->SetElementType(mapTypeInfo);
					if (key) genericTypeInfo->AddGenericArgument(key);
					genericTypeInfo->AddGenericArgument(value);

					Ptr<TypeInfoImpl> shared = new TypeInfoImpl(ITypeInfo::SharedPtr);
					shared->SetElementType(genericTypeInfo);
					result = shared;
				}

				void Visit(WfFunctionType* node)override
				{
					if (Ptr<ITypeInfo> returnType = Call(node->result.Obj(), true))
					{
						Ptr<TypeInfoImpl> enumerableTypeInfo = new TypeInfoImpl(ITypeInfo::TypeDescriptor);
						enumerableTypeInfo->SetTypeDescriptor(description::GetTypeDescriptor<IValueFunctionProxy>());

						Ptr<TypeInfoImpl> genericTypeInfo = new TypeInfoImpl(ITypeInfo::Generic);
						genericTypeInfo->SetElementType(enumerableTypeInfo);
						genericTypeInfo->AddGenericArgument(returnType);
						FOREACH(Ptr<WfType>, argument, node->arguments)
						{
							if (Ptr<ITypeInfo> argumentType = Call(argument.Obj(), true))
							{
								genericTypeInfo->AddGenericArgument(argumentType);
							}
							else
							{
								return;
							}
						}

						Ptr<TypeInfoImpl> shared = new TypeInfoImpl(ITypeInfo::SharedPtr);
						shared->SetElementType(genericTypeInfo);
						result = shared;
					}
				}

				void Visit(WfChildType* node)override
				{
					VisitReferenceType(node);
				}

				static Ptr<ITypeInfo> Execute(WfLexicalScope* scope, WfType* type)
				{
					return CreateTypeInfoFromTypeVisitor(scope).Call(type, true);
				}
			};

			Ptr<reflection::description::ITypeInfo>	CreateTypeInfoFromType(WfLexicalScope* scope, Ptr<WfType> type)
			{
				return CreateTypeInfoFromTypeVisitor::Execute(scope, type.Obj());
			}

/***********************************************************************
CopyType
***********************************************************************/

			class CopyTypeVisitor : public Object, public WfType::IVisitor
			{
			public:
				Ptr<WfType>				result;

				void Visit(WfPredefinedType* node)override
				{
					auto type = MakePtr<WfPredefinedType>();
					type->name = node->name;
					result = type;
				}

				void Visit(WfTopQualifiedType* node)override
				{
					auto type = MakePtr<WfTopQualifiedType>();
					type->name.value = node->name.value;
					result = type;
				}

				void Visit(WfReferenceType* node)override
				{
					auto type = MakePtr<WfReferenceType>();
					type->name.value = node->name.value;
					result = type;
				}

				void Visit(WfRawPointerType* node)override
				{
					auto type = MakePtr<WfRawPointerType>();
					type->element = CopyType(node->element);
					result = type;
				}

				void Visit(WfSharedPointerType* node)override
				{
					auto type = MakePtr<WfSharedPointerType>();
					type->element = CopyType(node->element);
					result = type;
				}

				void Visit(WfNullableType* node)override
				{
					auto type = MakePtr<WfNullableType>();
					type->element = CopyType(node->element);
					result = type;
				}

				void Visit(WfEnumerableType* node)override
				{
					auto type = MakePtr<WfEnumerableType>();
					type->element = CopyType(node->element);
					result = type;
				}

				void Visit(WfMapType* node)override
				{
					auto type = MakePtr<WfMapType>();
					type->key = CopyType(node->key);
					type->value = CopyType(node->value);
					type->writability = node->writability;
					result = type;
				}

				void Visit(WfFunctionType* node)override
				{
					auto type = MakePtr<WfFunctionType>();
					type->result = CopyType(node->result);
					CopyFrom(type->arguments, From(node->arguments).Select(CopyType));
					result = type;
				}

				void Visit(WfChildType* node)override
				{
					auto type = MakePtr<WfChildType>();
					type->parent = CopyType(node->parent);
					type->name.value = node->name.value;
					result = type;
				}
			};

			Ptr<WfType> CopyType(Ptr<WfType> type)
			{
				if (type)
				{
					CopyTypeVisitor visitor;
					type->Accept(&visitor);
					return visitor.result;
				}
				else
				{
					return 0;
				}
			}

/***********************************************************************
CreateTypeInfoFromType
***********************************************************************/
			
			Ptr<reflection::description::ITypeInfo>	CopyTypeInfo(reflection::description::ITypeInfo* typeInfo)
			{
				switch (typeInfo->GetDecorator())
				{
				case ITypeInfo::RawPtr:
				case ITypeInfo::SharedPtr:
				case ITypeInfo::Nullable:
					{
						Ptr<TypeInfoImpl> impl = new TypeInfoImpl(typeInfo->GetDecorator());
						impl->SetElementType(CopyTypeInfo(typeInfo->GetElementType()));
						return impl;
					}
				case ITypeInfo::TypeDescriptor:
					{
						Ptr<TypeInfoImpl> impl = new TypeInfoImpl(ITypeInfo::TypeDescriptor);
						impl->SetTypeDescriptor(typeInfo->GetTypeDescriptor());
						return impl;
					}
				case ITypeInfo::Generic:
					{
						Ptr<TypeInfoImpl> impl = new TypeInfoImpl(ITypeInfo::Generic);
						impl->SetElementType(CopyTypeInfo(typeInfo->GetElementType()));
						vint count = typeInfo->GetGenericArgumentCount();
						for (vint i = 0; i < count; i++)
						{
							impl->AddGenericArgument(CopyTypeInfo(typeInfo->GetGenericArgument(i)));
						}
						return impl;
					}
				default:
					return 0;
				}
			}

/***********************************************************************
CanConvertToType
***********************************************************************/

			bool CanConvertToType(reflection::description::ITypeInfo* fromType, reflection::description::ITypeInfo* toType, bool explicitly)
			{
				ITypeDescriptor* objectType = GetTypeDescriptor<Value>();
				bool fromObject = fromType->GetDecorator() == ITypeInfo::TypeDescriptor && fromType->GetTypeDescriptor() == objectType;
				bool toObject = toType->GetDecorator() == ITypeInfo::TypeDescriptor && toType->GetTypeDescriptor() == objectType;

				if (fromObject && toObject)
				{
					return true;
				}
				else if (fromObject)
				{
					return explicitly;
				}
				else if (toObject)
				{
					return true;
				}

				switch (fromType->GetDecorator())
				{
				case ITypeInfo::RawPtr:
					switch (toType->GetDecorator())
					{
					case ITypeInfo::RawPtr:
						return CanConvertToType(fromType->GetElementType(), toType->GetElementType(), explicitly);
					case ITypeInfo::SharedPtr:
						return explicitly && CanConvertToType(fromType->GetElementType(), toType->GetElementType(), explicitly);
					case ITypeInfo::Nullable:
					case ITypeInfo::TypeDescriptor:
					case ITypeInfo::Generic:
						return false;
					}
					break;
				case ITypeInfo::SharedPtr:
					switch (toType->GetDecorator())
					{
					case ITypeInfo::RawPtr:
						return explicitly && CanConvertToType(fromType->GetElementType(), toType->GetElementType(), explicitly);
					case ITypeInfo::SharedPtr:
						return CanConvertToType(fromType->GetElementType(), toType->GetElementType(), explicitly);
					case ITypeInfo::Nullable:
					case ITypeInfo::TypeDescriptor:
					case ITypeInfo::Generic:
						return false;
					}
					break;
				case ITypeInfo::Nullable:
					switch (toType->GetDecorator())
					{
					case ITypeInfo::RawPtr:
					case ITypeInfo::SharedPtr:
						return false;
					case ITypeInfo::Nullable:
						return CanConvertToType(fromType->GetElementType(), toType->GetElementType(), explicitly);
					case ITypeInfo::TypeDescriptor:
						return explicitly && CanConvertToType(fromType->GetElementType(), toType, explicitly);
					case ITypeInfo::Generic:
						return false;
					}
					break;
				case ITypeInfo::TypeDescriptor:
					switch (toType->GetDecorator())
					{
					case ITypeInfo::RawPtr:
					case ITypeInfo::SharedPtr:
						return false;
					case ITypeInfo::Nullable:
						return CanConvertToType(fromType, toType->GetElementType(), explicitly);
					case ITypeInfo::TypeDescriptor:
						{
							ITypeDescriptor* fromTd = fromType->GetTypeDescriptor();
							ITypeDescriptor* toTd = toType->GetTypeDescriptor();
							if ((fromTd->GetValueSerializer() != 0) != (toTd->GetValueSerializer() != 0))
							{
								return false;
							}

							if (fromTd->GetValueSerializer())
							{
								if (fromTd == toTd) return true;
								TypeFlag fromFlag = GetTypeFlag(fromTd);
								TypeFlag toFlag = GetTypeFlag(toTd);
								static vint conversionTable[(vint)TypeFlag::Count][(vint)TypeFlag::Count] = {
									/*Bool		*/{1, /**/ 0, 0, 0, 0, /**/ 0, 0, 0, 0, /**/ 0, 0, /**/ 1, 0},
									//-------------------------------------------------------------------------
									/*I1		*/{0, /**/ 1, 1, 1, 1, /**/ 2, 2, 2, 2, /**/ 1, 1, /**/ 1, 0},
									/*I2		*/{0, /**/ 2, 1, 1, 1, /**/ 2, 2, 2, 2, /**/ 1, 1, /**/ 1, 0},
									/*I4		*/{0, /**/ 2, 2, 1, 1, /**/ 2, 2, 2, 2, /**/ 2, 1, /**/ 1, 0},
									/*I8		*/{0, /**/ 2, 2, 2, 1, /**/ 2, 2, 2, 2, /**/ 2, 1, /**/ 1, 0},
									//-------------------------------------------------------------------------
									/*U1		*/{0, /**/ 2, 2, 2, 2, /**/ 1, 1, 1, 1, /**/ 1, 1, /**/ 1, 0},
									/*U2		*/{0, /**/ 2, 2, 2, 2, /**/ 2, 1, 1, 1, /**/ 1, 1, /**/ 1, 0},
									/*U4		*/{0, /**/ 2, 2, 2, 2, /**/ 2, 2, 1, 1, /**/ 2, 1, /**/ 1, 0},
									/*U8		*/{0, /**/ 2, 2, 2, 2, /**/ 2, 2, 2, 1, /**/ 2, 1, /**/ 1, 0},
									//-------------------------------------------------------------------------
									/*F4		*/{0, /**/ 2, 2, 2, 2, /**/ 2, 2, 2, 2, /**/ 1, 1, /**/ 1, 0},
									/*F8		*/{0, /**/ 2, 2, 2, 2, /**/ 2, 2, 2, 2, /**/ 2, 1, /**/ 1, 0},
									//-------------------------------------------------------------------------
									/*String	*/{2, /**/ 2, 2, 2, 2, /**/ 2, 2, 2, 2, /**/ 2, 2, /**/ 1, 2},
									/*Others	*/{0, /**/ 0, 0, 0, 0, /**/ 0, 0, 0, 0, /**/ 0, 0, /**/ 1, 0},
								};
								vint conversion = conversionTable[(vint)fromFlag][(vint)toFlag];
								return conversion == 1 || (explicitly && conversion == 2);
							}
							else
							{
								if (fromTd->CanConvertTo(toTd))
								{
									return true;
								}
								if (explicitly && toTd->CanConvertTo(fromTd))
								{
									return true;
								}
							}
						}
						break;
					case ITypeInfo::Generic:
						return explicitly && CanConvertToType(fromType, toType->GetElementType(), explicitly);
					}
					break;
				case ITypeInfo::Generic:
					switch (toType->GetDecorator())
					{
					case ITypeInfo::RawPtr:
					case ITypeInfo::SharedPtr:
					case ITypeInfo::Nullable:
						return false;
					case ITypeInfo::TypeDescriptor:
						return CanConvertToType(fromType->GetElementType(), toType, explicitly);
					case ITypeInfo::Generic:
						if (explicitly) return true;
						if (fromType->GetGenericArgumentCount() != toType->GetGenericArgumentCount())
						{
							return false;
						}
						if (!CanConvertToType(fromType->GetElementType(), toType->GetElementType(), explicitly)) return false;
						for (vint i = 0; i < fromType->GetGenericArgumentCount(); i++)
						{
							if (!IsSameType(fromType->GetGenericArgument(i), toType->GetGenericArgument(i)))
							{
								return false;
							}
						}
						return true;
					}
					break;
				}
				return false;
			}

/***********************************************************************
IsSameType
***********************************************************************/

			bool IsSameType(reflection::description::ITypeInfo* fromType, reflection::description::ITypeInfo* toType)
			{
				if (fromType->GetDecorator() != toType->GetDecorator())
				{
					return false;
				}
				switch (fromType->GetDecorator())
				{
				case ITypeInfo::RawPtr:
				case ITypeInfo::SharedPtr:
				case ITypeInfo::Nullable:
					return IsSameType(fromType->GetElementType(), toType->GetElementType());
				case ITypeInfo::TypeDescriptor:
					return fromType->GetTypeDescriptor() == toType->GetTypeDescriptor();
				case ITypeInfo::Generic:
					if (fromType->GetGenericArgumentCount() != toType->GetGenericArgumentCount())
					{
						return false;
					}
					if (!IsSameType(fromType->GetElementType(), toType->GetElementType())) return false;
					for (vint i = 0; i < fromType->GetGenericArgumentCount(); i++)
					{
						if (!IsSameType(fromType->GetGenericArgument(i), toType->GetGenericArgument(i)))
						{
							return false;
						}
					}
					return true;
				}
				return false;
			}

/***********************************************************************
GetMergedType
***********************************************************************/

			Ptr<reflection::description::ITypeInfo>	GetMergedType(Ptr<reflection::description::ITypeInfo> firstType, Ptr<reflection::description::ITypeInfo> secondType)
			{
				if (CanConvertToType(secondType.Obj(), firstType.Obj(), false))
				{
					return firstType;
				}
				else if (CanConvertToType(firstType.Obj(), secondType.Obj(), false))
				{
					return secondType;
				}
				else
				{
					return 0;
				}
			}

/***********************************************************************
IsNullAcceptableType
***********************************************************************/

			bool IsNullAcceptableType(reflection::description::ITypeInfo* type)
			{
				switch (type->GetDecorator())
				{
				case ITypeInfo::RawPtr:
				case ITypeInfo::SharedPtr:
				case ITypeInfo::Nullable:
					return true;
				case ITypeInfo::TypeDescriptor:
					return type->GetTypeDescriptor() == description::GetTypeDescriptor<Value>();
				case ITypeInfo::Generic:
					return false;
				}
				return false;
			}

/***********************************************************************
CreateTypeInfoFromMethodInfo
***********************************************************************/

			Ptr<reflection::description::ITypeInfo> CreateTypeInfoFromMethodInfo(reflection::description::IMethodInfo* info)
			{
				Ptr<TypeInfoImpl> functionType = new TypeInfoImpl(ITypeInfo::SharedPtr);
				{
					Ptr<TypeInfoImpl> genericType = new TypeInfoImpl(ITypeInfo::Generic);
					functionType->SetElementType(genericType);
					{
						Ptr<TypeInfoImpl> elementType = new TypeInfoImpl(ITypeInfo::TypeDescriptor);
						elementType->SetTypeDescriptor(description::GetTypeDescriptor<IValueFunctionProxy>());
						genericType->SetElementType(elementType);
					}

					genericType->AddGenericArgument(CopyTypeInfo(info->GetReturn()));
					vint parameterCount = info->GetParameterCount();
					for (vint j = 0; j < parameterCount; j++)
					{
						genericType->AddGenericArgument(CopyTypeInfo(info->GetParameter(j)->GetType()));
					}
				}
				return functionType;
			}
		}
	}
}