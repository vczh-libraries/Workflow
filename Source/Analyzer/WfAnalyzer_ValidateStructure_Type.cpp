#include "WfAnalyzer.h"

namespace vl
{
	namespace workflow
	{
		namespace analyzer
		{
			using namespace collections;
			using namespace parsing;
			using namespace reflection::description;

/***********************************************************************
ValidateStructure(Type)
***********************************************************************/

			class ValidateReferenceTypeVisitor : public Object, public WfType::IVisitor
			{
			public:
				WfLexicalScopeManager*					manager;
				bool									result;

				ValidateReferenceTypeVisitor(WfLexicalScopeManager* _manager)
					:manager(_manager)
					, result(true)
				{
				}
				
				void Visit(WfPredefinedType* node)override
				{
				}

				void Visit(WfTopQualifiedType* node)override
				{
				}

				void Visit(WfReferenceType* node)override
				{
				}

				void Visit(WfRawPointerType* node)override
				{
					result = false;
				}

				void Visit(WfSharedPointerType* node)override
				{
					result = false;
				}

				void Visit(WfNullableType* node)override
				{
					result = false;
				}

				void Visit(WfEnumerableType* node)override
				{
					result = false;
				}

				void Visit(WfMapType* node)override
				{
					result = false;
				}

				void Visit(WfObservableListType* node)override
				{
					result = false;
				}

				void Visit(WfFunctionType* node)override
				{
					result = false;
				}

				void Visit(WfChildType* node)override
				{
					node->parent->Accept(this);
				}

				static bool Execute(Ptr<WfType> type, WfLexicalScopeManager* manager)
				{
					ValidateReferenceTypeVisitor visitor(manager);
					type->Accept(&visitor);
					return visitor.result;
				}
			};

			class ValidateStructureTypeVisitor : public Object, public WfType::IVisitor
			{
			public:
				WfLexicalScopeManager*					manager;
				ValidateTypeStragety					strategy;
				WfClassDeclaration*						classDecl;

				ValidateStructureTypeVisitor(WfLexicalScopeManager* _manager, ValidateTypeStragety _strategy, WfClassDeclaration* _classDecl)
					:manager(_manager)
					, strategy(_strategy)
					, classDecl(_classDecl)
				{
				}
				
				void Visit(WfPredefinedType* node)override
				{
					switch (node->name)
					{
					case WfPredefinedTypeName::Void:
						switch (strategy)
						{
						case ValidateTypeStragety::Value:
							manager->errors.Add(WfErrors::WrongVoidType(node));
							break;
						case ValidateTypeStragety::BaseType:
							manager->errors.Add(WfErrors::WrongBaseType(classDecl, node));
						default:;
						}
						break;
					case WfPredefinedTypeName::Interface:
						switch (strategy)
						{
						case ValidateTypeStragety::ReturnType:
						case ValidateTypeStragety::Value:
							manager->errors.Add(WfErrors::WrongInterfaceType(node));
							break;
						default:;
						}
					default:;
					}
				}

				void Visit(WfTopQualifiedType* node)override
				{
				}

				void Visit(WfReferenceType* node)override
				{
				}

				void Visit(WfRawPointerType* node)override
				{
					if (strategy == ValidateTypeStragety::BaseType)
					{
						manager->errors.Add(WfErrors::WrongBaseType(classDecl, node));
					}

					if (!ValidateReferenceTypeVisitor::Execute(node->element, manager))
					{
						manager->errors.Add(WfErrors::RawPointerToNonReferenceType(node));
					}
					else if (auto predefinedType = node->element.Cast<WfPredefinedType>())
					{
						if (predefinedType->name != WfPredefinedTypeName::Interface)
						{
							manager->errors.Add(WfErrors::RawPointerToNonReferenceType(node));
						}
					}
				}

				void Visit(WfSharedPointerType* node)override
				{
					if (strategy == ValidateTypeStragety::BaseType)
					{
						manager->errors.Add(WfErrors::WrongBaseType(classDecl, node));
					}

					if (!ValidateReferenceTypeVisitor::Execute(node->element, manager))
					{
						manager->errors.Add(WfErrors::SharedPointerToNonReferenceType(node));
					}
					else if (auto predefinedType = node->element.Cast<WfPredefinedType>())
					{
						if (predefinedType->name != WfPredefinedTypeName::Interface)
						{
							manager->errors.Add(WfErrors::SharedPointerToNonReferenceType(node));
						}
					}
				}

				void Visit(WfNullableType* node)override
				{
					if (strategy == ValidateTypeStragety::BaseType)
					{
						manager->errors.Add(WfErrors::WrongBaseType(classDecl, node));
					}

					if (!ValidateReferenceTypeVisitor::Execute(node->element, manager))
					{
						manager->errors.Add(WfErrors::NullableToNonReferenceType(node));
					}
					else if (auto predefinedType = node->element.Cast<WfPredefinedType>())
					{
						switch (predefinedType->name)
						{
						case WfPredefinedTypeName::Void:
						case WfPredefinedTypeName::Object:
						case WfPredefinedTypeName::Interface:
							manager->errors.Add(WfErrors::NullableToNonReferenceType(node));
							break;
						default:;
						}
					}
				}

				void Visit(WfEnumerableType* node)override
				{
					if (strategy == ValidateTypeStragety::BaseType)
					{
						manager->errors.Add(WfErrors::WrongBaseType(classDecl, node));
					}

					ValidateTypeStructure(manager, node->element);
				}

				void Visit(WfMapType* node)override
				{
					if (strategy == ValidateTypeStragety::BaseType)
					{
						manager->errors.Add(WfErrors::WrongBaseType(classDecl, node));
					}

					if (node->key)
					{
						ValidateTypeStructure(manager, node->key);
					}
					ValidateTypeStructure(manager, node->value);
				}

				void Visit(WfObservableListType* node)override
				{
					if (strategy == ValidateTypeStragety::BaseType)
					{
						manager->errors.Add(WfErrors::WrongBaseType(classDecl, node));
					}

					ValidateTypeStructure(manager, node->element);
				}

				void Visit(WfFunctionType* node)override
				{
					if (strategy == ValidateTypeStragety::BaseType)
					{
						manager->errors.Add(WfErrors::WrongBaseType(classDecl, node));
					}

					ValidateTypeStructure(manager, node->result, ValidateTypeStragety::ReturnType);
					for (auto argument : node->arguments)
					{
						ValidateTypeStructure(manager, argument);
					}
				}

				void Visit(WfChildType* node)override
				{
					if (!ValidateReferenceTypeVisitor::Execute(node->parent, manager))
					{
						manager->errors.Add(WfErrors::ChildOfNonReferenceType(node));
					}
				}

				static void Execute(Ptr<WfType> type, WfLexicalScopeManager* manager, ValidateTypeStragety strategy, WfClassDeclaration* classDecl)
				{
					ValidateStructureTypeVisitor visitor(manager, strategy, classDecl);
					type->Accept(&visitor);
				}
			};

/***********************************************************************
ValidateStructure
***********************************************************************/

			void ValidateTypeStructure(WfLexicalScopeManager* manager, Ptr<WfType> type, ValidateTypeStragety strategy, WfClassDeclaration* classDecl)
			{
				ValidateStructureTypeVisitor::Execute(type, manager, strategy, classDecl);
			}
		}
	}
}
