#include "WfEmitter.h"

namespace vl
{
	namespace workflow
	{
		namespace emitter
		{
			using namespace glr;
			using namespace reflection::description;
			using namespace analyzer;
			using namespace runtime;
			using namespace typeimpl;

			typedef WfInstruction Ins;

#define INSTRUCTION(X) context.AddInstruction(node, X)
#define FILL_LABEL_TO_INS(LABEL, INS) context.assembly->instructions[LABEL].indexParameter = INS
#define FILL_LABEL_TO_CURRENT(LABEL) FILL_LABEL_TO_INS(LABEL, context.assembly->instructions.Count())

/***********************************************************************
GenerateTypeCastInstructions
***********************************************************************/

			void GenerateTypeCastInstructions(WfCodegenContext& context, Ptr<reflection::description::ITypeInfo> expectedType, bool strongCast, WfExpression* node)
			{
				if (expectedType->GetTypeDescriptor() != GetTypeDescriptor<Value>())
				{
					vint fillElseIndex = -1;
					vint fillEndIndex = -1;

					if (expectedType->GetDecorator() == ITypeInfo::Nullable)
					{
						INSTRUCTION(Ins::Duplicate(0));
						INSTRUCTION(Ins::LoadValue({}));
						INSTRUCTION(Ins::CompareReference());
						fillElseIndex = INSTRUCTION(Ins::JumpIf(-1));
					}

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

					if (fillElseIndex != -1)
					{
						fillEndIndex = INSTRUCTION(Ins::Jump(-1));
						FILL_LABEL_TO_CURRENT(fillElseIndex);
						INSTRUCTION(Ins::LoadValue({}));
						FILL_LABEL_TO_CURRENT(fillEndIndex);
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
					INSTRUCTION(Ins::LoadValue({ true }));
				}
			}

/***********************************************************************
GetInstructionTypeArgument
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

#define EXECUTE_CALLBACK(EXPR) if (callback) callback->EXPR

			Ptr<runtime::WfAssembly> GenerateAssembly(analyzer::WfLexicalScopeManager* manager, IWfCompilerCallback* callback)
			{
				EXECUTE_CALLBACK(OnGenerateMetadata());
				auto assembly = Ptr(new WfAssembly);
				assembly->insBeforeCodegen = new WfInstructionDebugInfo;
				assembly->insAfterCodegen = new WfInstructionDebugInfo;
				
				WfCodegenContext context(assembly, manager);
				for (auto [module, index] : indexed(manager->GetModules()))
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
					for (auto td : manager->declarationTypes.Values())
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

				for (auto module : manager->GetModules())
				{
					for (auto decl : module->declarations)
					{
						GenerateGlobalDeclarationMetadata(context, decl);
					}
				}

				{
					auto meta = Ptr(new WfAssemblyFunction);
					meta->name = L"<initialize>";
					vint functionIndex = assembly->functions.Add(meta);
					assembly->functionByName.Add(meta->name, functionIndex);

					auto functionContext = Ptr(new WfCodegenFunctionContext);
					functionContext->function = meta;
					context.functionContext = functionContext;
					
					meta->firstInstruction = assembly->instructions.Count();
					for (auto module : manager->GetModules())
					{
						for (auto decl : module->declarations)
						{
							GenerateInitializeInstructions(context, decl);
						}
					}

					// define node for INSTRUCTION
					glr::ParsingAstBase* node = nullptr;
					INSTRUCTION(Ins::LoadValue({}));
					INSTRUCTION(Ins::Return());

					meta->lastInstruction = assembly->instructions.Count() - 1;

					context.functionContext = 0;
					GenerateClosureInstructions(context, functionContext);
				}

				for (auto module : manager->GetModules())
				{
					EXECUTE_CALLBACK(OnGenerateCode(module));
					for (auto decl : module->declarations)
					{
						GenerateDeclarationInstructions(context, decl);
					}
				}

				EXECUTE_CALLBACK(OnGenerateDebugInfo());
				assembly->Initialize();
				return assembly;
			}

#undef EXECUTE_CALLBACK
#undef FILL_LABEL_TO_CURRENT
#undef FILL_LABEL_TO_INS
#undef INSTRUCTION

/***********************************************************************
Compile
***********************************************************************/

			Ptr<runtime::WfAssembly> Compile(analyzer::WfLexicalScopeManager* manager, collections::List<WString>& moduleCodes, collections::List<glr::ParsingError>& errors)
			{
				manager->Clear(true, true);
				for (auto code : moduleCodes)
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

			Ptr<runtime::WfAssembly> Compile(workflow::Parser& workflowParser, collections::List<WString>& moduleCodes, collections::List<glr::ParsingError>& errors)
			{
				WfLexicalScopeManager manager(workflowParser);
				return Compile(&manager, moduleCodes, errors);
			}
		}
	}
}