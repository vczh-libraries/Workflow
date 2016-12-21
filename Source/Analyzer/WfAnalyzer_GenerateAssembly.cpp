#include "WfAnalyzer.h"

namespace vl
{
	namespace workflow
	{
		namespace analyzer
		{
			using namespace collections;
			using namespace regex;
			using namespace parsing;
			using namespace reflection;
			using namespace reflection::description;
			using namespace runtime;
			using namespace typeimpl;

			typedef WfInstruction Ins;

#define INSTRUCTION(X) context.AddInstruction(node, X)

/***********************************************************************
GenerateTypeCastInstructions
***********************************************************************/

			void GenerateTypeCastInstructions(WfCodegenContext& context, Ptr<reflection::description::ITypeInfo> expectedType, bool strongCast, WfExpression* node)
			{
				if (expectedType->GetTypeDescriptor() != GetTypeDescriptor<Value>())
				{
					if (strongCast)
					{
						switch (expectedType->GetDecorator())
						{
						case ITypeInfo::RawPtr:
							INSTRUCTION(Ins::ConvertToType(Value::RawPtr, expectedType->GetTypeDescriptor()));
							break;
						case ITypeInfo::SharedPtr:
							INSTRUCTION(Ins::ConvertToType(Value::SharedPtr, expectedType->GetTypeDescriptor()));
							break;
						case ITypeInfo::Nullable:
						case ITypeInfo::TypeDescriptor:
						case ITypeInfo::Generic:
							INSTRUCTION(Ins::ConvertToType(Value::BoxedValue, expectedType->GetTypeDescriptor()));
							break;
						}
					}
					else
					{
						switch (expectedType->GetDecorator())
						{
						case ITypeInfo::RawPtr:
							INSTRUCTION(Ins::TryConvertToType(Value::RawPtr, expectedType->GetTypeDescriptor()));
							break;
						case ITypeInfo::SharedPtr:
							INSTRUCTION(Ins::TryConvertToType(Value::SharedPtr, expectedType->GetTypeDescriptor()));
							break;
						case ITypeInfo::Nullable:
						case ITypeInfo::TypeDescriptor:
						case ITypeInfo::Generic:
							INSTRUCTION(Ins::TryConvertToType(Value::BoxedValue, expectedType->GetTypeDescriptor()));
							break;
						}
					}
				}
			}

/***********************************************************************
GetInstructionTypeArgument
***********************************************************************/

			void GenerateTypeTestingInstructions(WfCodegenContext& context, Ptr<reflection::description::ITypeInfo> expectedType, WfExpression* node)
			{
				if (expectedType->GetTypeDescriptor() != GetTypeDescriptor<Value>())
				{
					switch (expectedType->GetDecorator())
					{
					case ITypeInfo::RawPtr:
						INSTRUCTION(Ins::TestType(Value::RawPtr, expectedType->GetTypeDescriptor()));
						break;
					case ITypeInfo::SharedPtr:
						INSTRUCTION(Ins::TestType(Value::SharedPtr, expectedType->GetTypeDescriptor()));
						break;
					case ITypeInfo::Nullable:
					case ITypeInfo::TypeDescriptor:
					case ITypeInfo::Generic:
						INSTRUCTION(Ins::TestType(Value::BoxedValue, expectedType->GetTypeDescriptor()));
						break;
					}
				}
				else
				{
					INSTRUCTION(Ins::Pop());
					INSTRUCTION(Ins::LoadValue(BoxValue(true)));
				}
			}

/***********************************************************************
GenerateTypeCastInstructions
***********************************************************************/

			runtime::WfInsType GetInstructionTypeArgument(Ptr<reflection::description::ITypeInfo> expectedType)
			{
				auto td = expectedType->GetTypeDescriptor();
				if (td == GetTypeDescriptor<bool>()) return WfInsType::Bool;
				if (td == GetTypeDescriptor<vint8_t>()) return WfInsType::I1;
				if (td == GetTypeDescriptor<vint16_t>()) return WfInsType::I2;
				if (td == GetTypeDescriptor<vint32_t>()) return WfInsType::I4;
				if (td == GetTypeDescriptor<vint64_t>()) return WfInsType::I8;
				if (td == GetTypeDescriptor<vuint8_t>()) return WfInsType::U1;
				if (td == GetTypeDescriptor<vuint16_t>()) return WfInsType::U2;
				if (td == GetTypeDescriptor<vuint32_t>()) return WfInsType::U4;
				if (td == GetTypeDescriptor<vuint64_t>()) return WfInsType::U8;
				if (td == GetTypeDescriptor<float>()) return WfInsType::F4;
				if (td == GetTypeDescriptor<double>()) return WfInsType::F8;
				if (td == GetTypeDescriptor<WString>()) return WfInsType::String;
				return WfInsType::Unknown;
			}

/***********************************************************************
GenerateAssembly
***********************************************************************/

			Ptr<runtime::WfAssembly> GenerateAssembly(WfLexicalScopeManager* manager)
			{
				auto assembly = MakePtr<WfAssembly>();
				assembly->insBeforeCodegen = new WfInstructionDebugInfo;
				assembly->insAfterCodegen = new WfInstructionDebugInfo;
				
				WfCodegenContext context(assembly, manager);
				FOREACH_INDEXER(Ptr<WfModule>, module, index, manager->GetModules())
				{
					auto codeBeforeCodegen = manager->GetModuleCodes()[index];

					auto recorderBefore = new ParsingGeneratedLocationRecorder(context.nodePositionsBeforeCodegen);
					auto recorderAfter = new ParsingGeneratedLocationRecorder(context.nodePositionsAfterCodegen);
					auto recorderOriginal = new ParsingOriginalLocationRecorder(recorderBefore);
					auto recorderMultiple = new ParsingMultiplePrintNodeRecorder;
					recorderMultiple->AddRecorder(recorderOriginal);
					recorderMultiple->AddRecorder(recorderAfter);

					stream::MemoryStream memoryStream;
					{
						stream::StreamWriter streamWriter(memoryStream);
						ParsingWriter parsingWriter(streamWriter, recorderMultiple, index);
						WfPrint(module, L"", parsingWriter);
					}

					memoryStream.SeekFromBegin(0);
					auto codeAfterCodegen = stream::StreamReader(memoryStream).ReadToEnd();

					assembly->insBeforeCodegen->moduleCodes.Add(codeBeforeCodegen);
					assembly->insAfterCodegen->moduleCodes.Add(codeAfterCodegen);
				}

				if (manager->declarationTypes.Count() > 0)
				{
					assembly->typeImpl = new WfTypeImpl;
					FOREACH(Ptr<ITypeDescriptor>, td, manager->declarationTypes.Values())
					{
						if (auto tdClass = td.Cast<WfClass>())
						{
							assembly->typeImpl->classes.Add(tdClass);
						}
						else if (auto tdInterface = td.Cast<WfInterface>())
						{
							assembly->typeImpl->interfaces.Add(tdInterface);
						}
						else if (auto tdStruct = td.Cast<WfStruct>())
						{
							assembly->typeImpl->structs.Add(tdStruct);
						}
						else if (auto tdEnum = td.Cast<WfEnum>())
						{
							assembly->typeImpl->enums.Add(tdEnum);
						}
					}
				}

				FOREACH(Ptr<WfModule>, module, manager->GetModules())
				{
					FOREACH(Ptr<WfDeclaration>, decl, module->declarations)
					{
						GenerateGlobalDeclarationMetadata(context, decl);
					}
				}

				{
					auto meta = MakePtr<WfAssemblyFunction>();
					meta->name = L"<initialize>";
					vint functionIndex = assembly->functions.Add(meta);
					assembly->functionByName.Add(meta->name, functionIndex);

					auto functionContext = MakePtr<WfCodegenFunctionContext>();
					functionContext->function = meta;
					context.functionContext = functionContext;
					
					meta->firstInstruction = assembly->instructions.Count();
					FOREACH(Ptr<WfModule>, module, manager->GetModules())
					{
						FOREACH(Ptr<WfDeclaration>, decl, module->declarations)
						{
							GenerateInitializeInstructions(context, decl);
						}
					}

					// define node for INSTRUCTION
					parsing::ParsingTreeCustomBase* node = nullptr;
					INSTRUCTION(Ins::LoadValue(Value()));
					INSTRUCTION(Ins::Return());

					meta->lastInstruction = assembly->instructions.Count() - 1;

					context.functionContext = 0;
					GenerateClosureInstructions(context, functionContext);
				}

				FOREACH(Ptr<WfModule>, module, manager->GetModules())
				{
					FOREACH(Ptr<WfDeclaration>, decl, module->declarations)
					{
						GenerateDeclarationInstructions(context, decl);
					}
				}

				assembly->Initialize();
				return assembly;
			}

#undef INSTRUCTION

/***********************************************************************
Compile
***********************************************************************/

			Ptr<runtime::WfAssembly> Compile(Ptr<parsing::tabling::ParsingTable> table, WfLexicalScopeManager* manager, collections::List<WString>& moduleCodes, collections::List<Ptr<parsing::ParsingError>>& errors)
			{
				manager->Clear(true, true);
				FOREACH(WString, code, moduleCodes)
				{
					manager->AddModule(code);
				}

				if (manager->errors.Count() > 0)
				{
					CopyFrom(errors, manager->errors);
					return 0;
				}

				manager->Rebuild(true);
				if (manager->errors.Count() > 0)
				{
					CopyFrom(errors, manager->errors);
					return 0;
				}

				return GenerateAssembly(manager);
			}

			Ptr<runtime::WfAssembly> Compile(Ptr<parsing::tabling::ParsingTable> table, collections::List<WString>& moduleCodes, collections::List<Ptr<parsing::ParsingError>>& errors)
			{
				WfLexicalScopeManager manager(table);
				return Compile(table, &manager, moduleCodes, errors);
			}
		}
	}
}