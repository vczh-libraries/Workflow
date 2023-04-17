#include "WfRuntime.h"
#include <math.h>

#ifdef VCZH_DESCRIPTABLEOBJECT_WITH_METADATA

namespace vl
{
	namespace workflow
	{
		namespace runtime
		{
			using namespace collections;
			using namespace reflection;
			using namespace reflection::description;
			using namespace typeimpl;

/***********************************************************************
WfRuntimeThreadContext (Operators)
***********************************************************************/

#define INTERNAL_ERROR(MESSAGE)\
				do{\
					context.RaiseException(WString(L"Internal error: " MESSAGE), true);\
					return WfRuntimeExecutionAction::Nop; \
				} while (0)\

#define CONTEXT_ACTION(ACTION, MESSAGE)\
				do{\
					if ((context.ACTION) != WfRuntimeThreadContextError::Success)\
					{\
						INTERNAL_ERROR(MESSAGE);\
					}\
				} while (0)\

			//-------------------------------------------------------------------------------

#define UNARY_OPERATOR(NAME, OPERATOR)\
			template<typename T>\
			WfRuntimeExecutionAction OPERATOR_##NAME(WfRuntimeThreadContext& context)\
			{\
				Value operand;\
				CONTEXT_ACTION(PopValue(operand), L"failed to pop a value from the stack.");\
				T value = OPERATOR UnboxValue<T>(operand);\
				CONTEXT_ACTION(PushValue(BoxValue(value)), L"failed to push a value to the stack.");\
				return WfRuntimeExecutionAction::ExecuteInstruction;\
			}\

#define BINARY_OPERATOR(NAME, OPERATOR)\
			template<typename T>\
			WfRuntimeExecutionAction OPERATOR_##NAME(WfRuntimeThreadContext& context)\
			{\
				Value first, second;\
				CONTEXT_ACTION(PopValue(second), L"failed to pop a value from the stack.");\
				CONTEXT_ACTION(PopValue(first), L"failed to pop a value from the stack.");\
				T value = UnboxValue<T>(first) OPERATOR UnboxValue<T>(second);\
				CONTEXT_ACTION(PushValue(BoxValue(value)), L"failed to push a value to the stack.");\
				return WfRuntimeExecutionAction::ExecuteInstruction;\
			}\

			//-------------------------------------------------------------------------------

			UNARY_OPERATOR(OpNot, ~)
			UNARY_OPERATOR(OpNot_Bool, !)
			UNARY_OPERATOR(OpPositive, +)
			UNARY_OPERATOR(OpNegative, -)

			BINARY_OPERATOR(OpAdd, +)
			BINARY_OPERATOR(OpSub, -)
			BINARY_OPERATOR(OpMul, *)
			BINARY_OPERATOR(OpDiv, /)
			BINARY_OPERATOR(OpMod, %)
			BINARY_OPERATOR(OpShl, <<)
			BINARY_OPERATOR(OpShr, >>)
			BINARY_OPERATOR(OpAnd, &)
			BINARY_OPERATOR(OpAnd_Bool, &&)
			BINARY_OPERATOR(OpOr, |)
			BINARY_OPERATOR(OpOr_Bool, ||)
			BINARY_OPERATOR(OpXor, ^)

			template<typename T>
			WfRuntimeExecutionAction OPERATOR_OpExp(WfRuntimeThreadContext& context)
			{
				Value first, second;
				CONTEXT_ACTION(PopValue(second), L"failed to pop a value from the stack.");
				CONTEXT_ACTION(PopValue(first), L"failed to pop a value from the stack.");
				T firstValue = UnboxValue<T>(first);
				T secondValue = UnboxValue<T>(second);
				T value = (T)exp(secondValue * log(firstValue));
				CONTEXT_ACTION(PushValue(BoxValue(value)), L"failed to push a value to the stack.");
				return WfRuntimeExecutionAction::ExecuteInstruction;
			}

			//-------------------------------------------------------------------------------
			
			template<typename T>
			WfRuntimeExecutionAction OPERATOR_OpCompare(WfRuntimeThreadContext& context)
			{
				Value first, second;
				CONTEXT_ACTION(PopValue(second), L"failed to pop a value from the stack.");
				CONTEXT_ACTION(PopValue(first), L"failed to pop a value from the stack.");

				bool firstNull = first.GetValueType() == Value::Null;
				bool secondNull = second.GetValueType() == Value::Null;
				if (firstNull)
				{
					if (secondNull)
					{
						CONTEXT_ACTION(PushValue(BoxValue((vint)0)), L"failed to push a value to the stack.");
					}
					else
					{
						CONTEXT_ACTION(PushValue(BoxValue((vint)-1)), L"failed to push a value to the stack.");
					}
				}
				else
				{
					if (secondNull)
					{
						CONTEXT_ACTION(PushValue(BoxValue((vint)1)), L"failed to push a value to the stack.");
					}
					else
					{
						T firstValue = UnboxValue<T>(first);
						T secondValue = UnboxValue<T>(second);
						if (firstValue < secondValue)
						{
							CONTEXT_ACTION(PushValue(BoxValue((vint)-1)), L"failed to push a value to the stack.");
						}
						else if (firstValue > secondValue)
						{
							CONTEXT_ACTION(PushValue(BoxValue((vint)1)), L"failed to push a value to the stack.");
						}
						else
						{
							CONTEXT_ACTION(PushValue(BoxValue((vint)0)), L"failed to push a value to the stack.");
						}
					}
				}
				return WfRuntimeExecutionAction::ExecuteInstruction;
			}

			//-------------------------------------------------------------------------------

			WfRuntimeExecutionAction OPERATOR_OpCompareReference(WfRuntimeThreadContext& context)
			{
				Value first, second;
				CONTEXT_ACTION(PopValue(second), L"failed to pop a value from the stack.");
				CONTEXT_ACTION(PopValue(first), L"failed to pop a value from the stack.");
				bool result = first.GetValueType() != Value::BoxedValue && second.GetValueType() != Value::BoxedValue && first.GetRawPtr() == second.GetRawPtr();
				CONTEXT_ACTION(PushValue(BoxValue(result)), L"failed to push a value to the stack.");
				return WfRuntimeExecutionAction::ExecuteInstruction;
			}

			//-------------------------------------------------------------------------------

			WfRuntimeExecutionAction OPERATOR_OpCompareValue(WfRuntimeThreadContext& context)
			{
				Value first, second;
				CONTEXT_ACTION(PopValue(second), L"failed to pop a value from the stack.");
				CONTEXT_ACTION(PopValue(first), L"failed to pop a value from the stack.");
				bool result = first == second;
				CONTEXT_ACTION(PushValue(BoxValue(result)), L"failed to push a value to the stack.");
				return WfRuntimeExecutionAction::ExecuteInstruction;
			}
			
/***********************************************************************
WfRuntimeThreadContext (TypeConversion)
***********************************************************************/

			bool OPERATOR_OpConvertToType(const Value& result, Value& converted, const WfInstruction& ins)
			{
				switch (ins.flagParameter)
				{
				case Value::Null:
					return false;
				case Value::RawPtr:
					if (result.GetValueType() == Value::BoxedValue)
					{
						return false;
					}
					else if (result.GetRawPtr())
					{
						if (result.GetTypeDescriptor()->CanConvertTo(ins.typeDescriptorParameter))
						{
							converted = Value::From(result.GetRawPtr());
						}
						else
						{
							return false;
						}
					}
					break;
				case Value::SharedPtr:
					if (result.GetValueType() == Value::BoxedValue)
					{
						return false;
					}
					else if (result.GetRawPtr())
					{
						if (result.GetTypeDescriptor()->CanConvertTo(ins.typeDescriptorParameter))
						{
							converted = Value::From(Ptr<DescriptableObject>(result.GetRawPtr()));
						}
						else
						{
							return false;
						}
					}
					break;
				case Value::BoxedValue:
					if (result.GetValueType() != Value::BoxedValue)
					{
						return false;
					}
					if (result.GetTypeDescriptor() == ins.typeDescriptorParameter)
					{
						converted = result;
						return true;
					}

					if (auto stFrom = result.GetTypeDescriptor()->GetSerializableType())
					{
						if (auto stTo = ins.typeDescriptorParameter->GetSerializableType())
						{
							WString text;
							return stFrom->Serialize(result, text) && stTo->Deserialize(text, converted);
						}
						else
						{
							switch (ins.typeDescriptorParameter->GetTypeDescriptorFlags())
							{
							case TypeDescriptorFlags::FlagEnum:
							case TypeDescriptorFlags::NormalEnum:
								if (result.GetTypeDescriptor() != GetTypeDescriptor<vuint64_t>())
								{
									return false;
								}
								else
								{
									auto intValue = result.GetBoxedValue().Cast<IValueType::TypedBox<vuint64_t>>()->value;
									converted = ins.typeDescriptorParameter->GetEnumType()->ToEnum(intValue);
								}
								break;
							default:
								return false;
							}
						}
					}
					else
					{
						switch (result.GetTypeDescriptor()->GetTypeDescriptorFlags())
						{
						case TypeDescriptorFlags::FlagEnum:
						case TypeDescriptorFlags::NormalEnum:
							if (ins.typeDescriptorParameter != GetTypeDescriptor<vuint64_t>())
							{
								return false;
							}
							else
							{
								auto intValue = result.GetTypeDescriptor()->GetEnumType()->FromEnum(result);
								converted = BoxValue<vuint64_t>(intValue);
							}
							break;
						default:
							return false;
						}
					}
				}

				return true;
			}
			
/***********************************************************************
WfRuntimeThreadContext (Range)
***********************************************************************/
			
			template<typename T>
			WfRuntimeExecutionAction OPERATOR_OpCreateRange(WfRuntimeThreadContext& context)
			{
				Value first, second;
				CONTEXT_ACTION(PopValue(second), L"failed to pop a value from the stack.");
				CONTEXT_ACTION(PopValue(first), L"failed to pop a value from the stack.");
				T firstValue = UnboxValue<T>(first);
				T secondValue = UnboxValue<T>(second);
				auto enumerable = Ptr(new WfRuntimeRange<T>(firstValue, secondValue));
				CONTEXT_ACTION(PushValue(Value::From(enumerable)), L"failed to push a value to the stack.");
				return WfRuntimeExecutionAction::ExecuteInstruction;
			}

#undef INTERNAL_ERROR
#undef CONTEXT_ACTION
#undef UNARY_OPERATOR
#undef BINARY_OPERATOR

/***********************************************************************
Helper Functions
***********************************************************************/

			Ptr<reflection::description::IValueFunctionProxy> LoadFunction(Ptr<WfRuntimeGlobalContext> context, const WString& name)
			{
				const auto& names = context->assembly->functionByName[name];
				CHECK_ERROR(names.Count() == 1, L"vl::workflow::runtime::LoadFunction(Ptr<WfRUntimeGlobalContext>, const WString&)#Multiple functions are found.");
				vint functionIndex = names[0];
				auto lambda = Ptr(new WfRuntimeLambda(context, nullptr, functionIndex));
				return lambda;
			}

/***********************************************************************
WfRuntimeThreadContext
***********************************************************************/
			
#define INTERNAL_ERROR(MESSAGE)\
				do{\
					RaiseException(WString(L"Internal error: " MESSAGE), true);\
					return WfRuntimeExecutionAction::Nop; \
				} while (0)\

#define CONTEXT_ACTION(ACTION, MESSAGE)\
				do{\
					if ((ACTION) != WfRuntimeThreadContextError::Success)\
					{\
						INTERNAL_ERROR(MESSAGE);\
					}\
				} while (0)\

#define CALL_DEBUGGER(ACTION)\
				do {\
					if (callback)\
					{\
						if (ACTION)\
						{\
							if (!callback->WaitForContinue())\
							{\
								INTERNAL_ERROR(L"Debugger stopped the program.");\
							}\
						}\
					}\
				} while (0)\

#define TYPE_OF_Bool							bool
#define TYPE_OF_I1								vint8_t
#define TYPE_OF_I2								vint16_t
#define TYPE_OF_I4								vint32_t
#define TYPE_OF_I8								vint64_t
#define TYPE_OF_U1								vuint8_t
#define TYPE_OF_U2								vuint16_t
#define TYPE_OF_U4								vuint32_t
#define TYPE_OF_U8								vuint64_t
#define TYPE_OF_F4								float
#define TYPE_OF_F8								double
#define TYPE_OF_String							WString
#define EXECUTE(OPERATION, TYPE)				case WfInsType::TYPE: return OPERATOR_##OPERATION<TYPE_OF_##TYPE>(*this);
#define BEGIN_TYPE								switch(ins.typeParameter) {
#define END_TYPE								default: INTERNAL_ERROR(L"unexpected type argument."); }

			WfRuntimeExecutionAction WfRuntimeThreadContext::ExecuteInternal(WfInstruction& ins, WfRuntimeStackFrame& stackFrame, IWfDebuggerCallback* callback)
			{
				switch (ins.code)
				{
				case WfInsCode::LoadValue:
					CONTEXT_ACTION(PushRuntimeValue(ins.valueParameter), L"failed to push a value to the stack.");
					return WfRuntimeExecutionAction::ExecuteInstruction;
				case WfInsCode::LoadFunction:
					{
						CONTEXT_ACTION(PushValue(BoxValue(ins.indexParameter)), L"failed to push a value to the stack.");
						return WfRuntimeExecutionAction::ExecuteInstruction;
					}
				case WfInsCode::LoadException:
					{
						CONTEXT_ACTION(PushValue(BoxValue(exceptionInfo)), L"failed to push a value to the stack.");
						return WfRuntimeExecutionAction::ExecuteInstruction;
					}
				case WfInsCode::LoadLocalVar:
					{
						Value operand;
						CONTEXT_ACTION(LoadLocalVariable(ins.indexParameter, operand), L"illegal local variable index.");
						CONTEXT_ACTION(PushValue(operand), L"failed to push a value to the stack.");
						return WfRuntimeExecutionAction::ExecuteInstruction;
					}
				case WfInsCode::LoadCapturedVar:
					{
						Value operand;
						CONTEXT_ACTION(LoadCapturedVariable(ins.indexParameter, operand), L"illegal captured variable index.");
						CONTEXT_ACTION(PushValue(operand), L"failed to push a value to the stack.");
						return WfRuntimeExecutionAction::ExecuteInstruction;
					}
				case WfInsCode::LoadGlobalVar:
					{
						CALL_DEBUGGER(callback->BreakRead(globalContext->assembly.Obj(), ins.indexParameter));
						Value operand;
						CONTEXT_ACTION(LoadGlobalVariable(ins.indexParameter, operand), L"illegal global variable index.");
						CONTEXT_ACTION(PushValue(operand), L"failed to push a value to the stack.");
						return WfRuntimeExecutionAction::ExecuteInstruction;
					}
				case WfInsCode::LoadMethodInfo:
					{
						CONTEXT_ACTION(PushValue(Value::From(ins.methodParameter)), L"failed to push a value to the stack.");
						return WfRuntimeExecutionAction::ExecuteInstruction;
					}
				case WfInsCode::LoadMethodClosure:
					{
						Value operand;
						CONTEXT_ACTION(PopValue(operand), L"failed to pop a value from the stack.");
						auto closure = ins.methodParameter->CreateFunctionProxy(operand);
						CONTEXT_ACTION(PushValue(closure), L"failed to push a value to the stack.");
						return WfRuntimeExecutionAction::ExecuteInstruction;
					}
				case WfInsCode::LoadClosureContext:
					{
						auto capturedVariables = GetCurrentStackFrame().capturedVariables;
						CONTEXT_ACTION(PushValue(Value::From(capturedVariables)), L"failed to push a value to the stack.");
						return WfRuntimeExecutionAction::ExecuteInstruction;
					}
				case WfInsCode::StoreLocalVar:
					{
						Value operand;
						CONTEXT_ACTION(PopValue(operand), L"failed to pop a value from the stack.");
						CONTEXT_ACTION(StoreLocalVariable(ins.indexParameter, operand), L"illegal local variable index.");
						return WfRuntimeExecutionAction::ExecuteInstruction;
					}
				case WfInsCode::StoreCapturedVar:
					{
						Value operand;
						CONTEXT_ACTION(PopValue(operand), L"failed to pop a value from the stack.");
						CONTEXT_ACTION(StoreCapturedVariable(ins.indexParameter, operand), L"illegal global variable index.");
						return WfRuntimeExecutionAction::ExecuteInstruction;
					}
				case WfInsCode::StoreGlobalVar:
					{
						CALL_DEBUGGER(callback->BreakWrite(globalContext->assembly.Obj(), ins.indexParameter));
						Value operand;
						CONTEXT_ACTION(PopValue(operand), L"failed to pop a value from the stack.");
						CONTEXT_ACTION(StoreGlobalVariable(ins.indexParameter, operand), L"illegal global variable index.");
						return WfRuntimeExecutionAction::ExecuteInstruction;
					}
				case WfInsCode::Duplicate:
					{
						vint index = stack.Count() - 1 - ins.countParameter;
						Value operand;
						CONTEXT_ACTION(LoadStackValue(index, operand), L"failed to duplicate a value from the stack.");
						CONTEXT_ACTION(PushValue(operand), L"failed to push a value to the stack.");
						return WfRuntimeExecutionAction::ExecuteInstruction;
					}
				case WfInsCode::Pop:
					{
						Value operand;
						CONTEXT_ACTION(PopValue(operand), L"failed to pop a value from the stack.");
						return WfRuntimeExecutionAction::ExecuteInstruction;
					}
				case WfInsCode::Return:
					{
						Value operand;
						CONTEXT_ACTION(PopValue(operand), L"failed to pop the function result.");
						CONTEXT_ACTION(PopStackFrame(), L"failed to pop the stack frame.");
						CONTEXT_ACTION(PushValue(operand), L"failed to push a value to the stack.");
						if (stackFrames.Count() == 0)
						{
							status = WfRuntimeExecutionStatus::Finished;
						}
						return WfRuntimeExecutionAction::ExitStackFrame;
					}
				case WfInsCode::NewArray:
					{
						auto list = IValueArray::Create();
						if (ins.countParameter > 0)
						{
							list->Resize(ins.countParameter);
							Value operand;
							for (vint i = 0; i < ins.countParameter; i++)
							{
								CONTEXT_ACTION(PopValue(operand), L"failed to pop a value from the stack.");
								list->Set(i, operand);
							}
						}
						CONTEXT_ACTION(PushValue(Value::From(list)), L"failed to push a value to the stack.");
						return WfRuntimeExecutionAction::ExecuteInstruction;
					}
				case WfInsCode::NewList:
					{
						auto list = IValueList::Create();
						Value operand;
						for (vint i = 0; i < ins.countParameter; i++)
						{
							CONTEXT_ACTION(PopValue(operand), L"failed to pop a value from the stack.");
							list->Add(operand);
						}
						CONTEXT_ACTION(PushValue(Value::From(list)), L"failed to push a value to the stack.");
						return WfRuntimeExecutionAction::ExecuteInstruction;
					}
				case WfInsCode::NewObservableList:
					{
						auto list = IValueObservableList::Create();
						Value operand;
						for (vint i = 0; i < ins.countParameter; i++)
						{
							CONTEXT_ACTION(PopValue(operand), L"failed to pop a value from the stack.");
							list->Add(operand);
						}
						CONTEXT_ACTION(PushValue(Value::From(list)), L"failed to push a value to the stack.");
						return WfRuntimeExecutionAction::ExecuteInstruction;
					}
				case WfInsCode::NewDictionary:
					{
						auto map = IValueDictionary::Create();
						Value key, value;
						for (vint i = 0; i < ins.countParameter; i+=2)
						{
							CONTEXT_ACTION(PopValue(value), L"failed to pop a value from the stack.");
							CONTEXT_ACTION(PopValue(key), L"failed to pop a value from the stack.");
							map->Set(key, value);
						}
						CONTEXT_ACTION(PushValue(Value::From(map)), L"failed to push a value to the stack.");
						return WfRuntimeExecutionAction::ExecuteInstruction;
					}
				case WfInsCode::CreateClosureContext:
					{
						Ptr<WfRuntimeVariableContext> capturedVariables;
						if (ins.countParameter > 0)
						{
							capturedVariables = Ptr(new WfRuntimeVariableContext);
							capturedVariables->variables.Resize(ins.countParameter);
							Value operand;
							for (vint i = 0; i < ins.countParameter; i++)
							{
								CONTEXT_ACTION(PopValue(operand), L"failed to pop a value from the stack.");
								capturedVariables->variables[ins.countParameter - 1 - i] = operand;
							}
						}

						CONTEXT_ACTION(PushValue(Value::From(capturedVariables)), L"failed to push a value to the stack.");
						return WfRuntimeExecutionAction::ExecuteInstruction;
					}
				case WfInsCode::CreateClosure:
					{
						Value context, function;
						CONTEXT_ACTION(PopValue(function), L"failed to pop a value from the stack.");
						CONTEXT_ACTION(PopValue(context), L"failed to pop a value from the stack.");
						auto capturedVariables = context.GetSharedPtr().Cast<WfRuntimeVariableContext>();
						auto functionIndex = UnboxValue<vint>(function);

						auto lambda = Ptr(new WfRuntimeLambda(globalContext, capturedVariables, functionIndex));
						CONTEXT_ACTION(PushValue(Value::From(lambda)), L"failed to push a value to the stack.");
						return WfRuntimeExecutionAction::ExecuteInstruction;
					}
				case WfInsCode::CreateInterface:
					{
						auto proxy = Ptr(new WfRuntimeInterfaceInstance);
						Value key, value, operand;
						for (vint i = 0; i < ins.countParameter; i+=2)
						{
							CONTEXT_ACTION(PopValue(value), L"failed to pop a value from the stack.");
							CONTEXT_ACTION(PopValue(key), L"failed to pop a value from the stack.");
							auto name = UnboxValue<IMethodInfo*>(key);
							auto func = UnboxValue<vint>(value);
							proxy->functions.Add(name, func);
						}

						CONTEXT_ACTION(PopValue(operand), L"failed to pop a value from the stack.");
						auto capturedVariables = operand.GetSharedPtr().Cast<WfRuntimeVariableContext>();
						proxy->capturedVariables = capturedVariables;
						proxy->globalContext = globalContext.Obj();

						Array<Value> arguments(1);
						arguments[0] = Value::From(proxy);
						auto obj = ins.methodParameter->Invoke(Value(), arguments);
						capturedVariables->variables[capturedVariables->variables.Count() - 1] = Value::From(obj.GetRawPtr());

						CONTEXT_ACTION(PushValue(obj), L"failed to push a value to the stack.");
						return WfRuntimeExecutionAction::ExecuteInstruction;
					}
				case WfInsCode::CreateRange:
					BEGIN_TYPE
						EXECUTE(OpCreateRange, I1)
						EXECUTE(OpCreateRange, I2)
						EXECUTE(OpCreateRange, I4)
						EXECUTE(OpCreateRange, I8)
						EXECUTE(OpCreateRange, U1)
						EXECUTE(OpCreateRange, U2)
						EXECUTE(OpCreateRange, U4)
						EXECUTE(OpCreateRange, U8)
					END_TYPE
				case WfInsCode::CreateStruct:
					{
						if (ins.typeDescriptorParameter->GetTypeDescriptorFlags() != TypeDescriptorFlags::Struct)
						{
							INTERNAL_ERROR(L"Type \"" + ins.typeDescriptorParameter->GetTypeName() + L"\" is not a struct.");
						}
						Value result = ins.typeDescriptorParameter->GetValueType()->CreateDefault();
						CONTEXT_ACTION(PushValue(result), L"failed to push a value to the stack.");
						return WfRuntimeExecutionAction::ExecuteInstruction;
					}
				case WfInsCode::DeleteRawPtr:
					{
						Value operand;
						CONTEXT_ACTION(PopValue(operand), L"failed to pop a value from the stack.");
						operand.DeleteRawPtr();
						return WfRuntimeExecutionAction::ExecuteInstruction;
					}
				case WfInsCode::ConvertToType:
					{
						Value result, converted;
						CONTEXT_ACTION(PopValue(result), L"failed to pop a value from the stack.");
						if (OPERATOR_OpConvertToType(result, converted, ins))
						{
							CONTEXT_ACTION(PushValue(converted), L"failed to push a value to the stack.");
						}
						else
						{
							WString from;
							if (result.IsNull())
							{
								from = L"<null>";
							}
							else
							{
								if (auto st = result.GetTypeDescriptor()->GetSerializableType())
								{
									WString text;
									st->Serialize(result, text);
									from = L"<" + text + L"> of " + result.GetTypeDescriptor()->GetTypeName();
								}
								else
								{
									from = result.GetTypeDescriptor()->GetTypeName();
								}
							}
							WString to = ins.typeDescriptorParameter->GetTypeName();
							RaiseException(L"Failed to convert from \"" + from + L"\" to \"" + to + L"\".", false);
						}
						return WfRuntimeExecutionAction::ExecuteInstruction;
					}
				case WfInsCode::TryConvertToType:
					{
						Value result, converted;
						CONTEXT_ACTION(PopValue(result), L"failed to pop a value from the stack.");
						if (OPERATOR_OpConvertToType(result, converted, ins))
						{
							CONTEXT_ACTION(PushValue(converted), L"failed to push a value to the stack.");
						}
						else
						{
							CONTEXT_ACTION(PushValue(Value()), L"failed to push a value to the stack.");
						}
						return WfRuntimeExecutionAction::ExecuteInstruction;
					}
				case WfInsCode::TestType:
					{
						Value operand;
						CONTEXT_ACTION(PopValue(operand), L"failed to pop a value from the stack.");
						if (operand.GetTypeDescriptor() && operand.GetValueType() == ins.flagParameter && operand.GetTypeDescriptor()->CanConvertTo(ins.typeDescriptorParameter))
						{
							CONTEXT_ACTION(PushValue(BoxValue(true)), L"failed to push a value to the stack.");
						}
						else
						{
							CONTEXT_ACTION(PushValue(BoxValue(false)), L"failed to push a value to the stack.");
						}
						return WfRuntimeExecutionAction::ExecuteInstruction;
					}
				case WfInsCode::GetType:
					{
						Value operand;
						CONTEXT_ACTION(PopValue(operand), L"failed to pop a value from the stack.");
						CONTEXT_ACTION(PushValue(Value::From(operand.GetTypeDescriptor())), L"failed to push a value to the stack.");
						return WfRuntimeExecutionAction::ExecuteInstruction;
					}
				case WfInsCode::Jump:
					{
						stackFrame.nextInstructionIndex = ins.indexParameter;
						return WfRuntimeExecutionAction::ExecuteInstruction;
					}
				case WfInsCode::JumpIf:
					{
						Value operand;
						CONTEXT_ACTION(PopValue(operand), L"failed to pop a value from the stack.");
						if (UnboxValue<bool>(operand))
						{
							stackFrame.nextInstructionIndex = ins.indexParameter;
						}
						return WfRuntimeExecutionAction::ExecuteInstruction;
					}
				case WfInsCode::Invoke:
					{
						CONTEXT_ACTION(PushStackFrame(ins.indexParameter, ins.countParameter), L"failed to invoke a function.");
						return WfRuntimeExecutionAction::EnterStackFrame;
					}
				case WfInsCode::InvokeWithContext:
					{
						CONTEXT_ACTION(PushStackFrame(ins.indexParameter, ins.countParameter, GetCurrentStackFrame().capturedVariables), L"failed to invoke a function.");
						return WfRuntimeExecutionAction::EnterStackFrame;
					}
				case WfInsCode::GetProperty:
					{
						Value operand;
						CONTEXT_ACTION(PopValue(operand), L"failed to pop a value from the stack.");
						CALL_DEBUGGER(callback->BreakGet(operand.GetRawPtr(), ins.propertyParameter));
						Value result = ins.propertyParameter->GetValue(operand);
						CONTEXT_ACTION(PushValue(result), L"failed to push a value to the stack.");
						return WfRuntimeExecutionAction::ExecuteInstruction;
					}
				case WfInsCode::SetProperty:
					{
						Value operand, value;
						CONTEXT_ACTION(PopValue(operand), L"failed to pop a value from the stack.");
						CONTEXT_ACTION(PopValue(value), L"failed to pop a value from the stack.");
						CALL_DEBUGGER(callback->BreakSet(operand.GetRawPtr(), ins.propertyParameter));
						ins.propertyParameter->SetValue(operand, value);
						return WfRuntimeExecutionAction::ExecuteInstruction;
					}
				case WfInsCode::UpdateProperty:
					{
						Value operand, value;
						CONTEXT_ACTION(PopValue(value), L"failed to pop a value from the stack.");
						CONTEXT_ACTION(PopValue(operand), L"failed to pop a value from the stack.");
						CALL_DEBUGGER(callback->BreakSet(operand.GetRawPtr(), ins.propertyParameter));
						ins.propertyParameter->SetValue(operand, value);
						CONTEXT_ACTION(PushValue(operand), L"failed to push a value to the stack.");
						return WfRuntimeExecutionAction::ExecuteInstruction;
					}
				case WfInsCode::InvokeProxy:
					{
						Value thisValue;
						CONTEXT_ACTION(PopValue(thisValue), L"failed to pop a value from the stack.");
						auto proxy = UnboxValue<Ptr<IValueFunctionProxy>>(thisValue);
						if (!proxy)
						{
							INTERNAL_ERROR(L"failed to invoke a null function proxy.");
							return WfRuntimeExecutionAction::Nop;
						}

						if (auto lambda = proxy.Cast<WfRuntimeLambda>())
						{
							if (lambda->globalContext == globalContext)
							{
								CONTEXT_ACTION(PushStackFrame(lambda->functionIndex, ins.countParameter, lambda->capturedVariables), L"failed to invoke a function.");
								return WfRuntimeExecutionAction::EnterStackFrame;
							}
						}

						List<Value> arguments;
						for (vint i = 0; i < ins.countParameter; i++)
						{
							Value argument;
							CONTEXT_ACTION(PopValue(argument), L"failed to pop a value from the stack.");
							arguments.Insert(0, argument);
						}

						Ptr<IValueList> list = Ptr(new ValueListWrapper<List<Value>*>(&arguments));
						Value result = proxy->Invoke(list);
						CONTEXT_ACTION(PushValue(result), L"failed to push a value to the stack.");
						return WfRuntimeExecutionAction::ExecuteInstruction;
					}
				case WfInsCode::InvokeMethod:
					{
						Value thisValue;
						CONTEXT_ACTION(PopValue(thisValue), L"failed to pop a value from the stack.");
						CALL_DEBUGGER(callback->BreakInvoke(thisValue.GetRawPtr(), ins.methodParameter));

						if (auto staticMethod = dynamic_cast<WfStaticMethod*>(ins.methodParameter))
						{
							if (staticMethod->GetGlobalContext() == globalContext.Obj())
							{
								CONTEXT_ACTION(PushStackFrame(staticMethod->functionIndex, ins.countParameter, nullptr), L"failed to invoke a function.");
								return WfRuntimeExecutionAction::EnterStackFrame;
							}
						}

						if (auto classMethod = dynamic_cast<WfClassMethod*>(ins.methodParameter))
						{
							if (classMethod->GetGlobalContext() == globalContext.Obj())
							{
								auto capturedVariable = Ptr(new WfRuntimeVariableContext);
								capturedVariable->variables.Resize(1);
								capturedVariable->variables[0] = Value::From(thisValue.GetRawPtr());

								CONTEXT_ACTION(PushStackFrame(classMethod->functionIndex, ins.countParameter, capturedVariable), L"failed to invoke a function.");
								return WfRuntimeExecutionAction::EnterStackFrame;
							}
						}

						Array<Value> arguments(ins.countParameter);
						for (vint i = 0; i < ins.countParameter; i++)
						{
							Value argument;
							CONTEXT_ACTION(PopValue(argument), L"failed to pop a value from the stack.");
							arguments[ins.countParameter - i - 1] = argument;
						}

						Value result = ins.methodParameter->Invoke(thisValue, arguments);
						CONTEXT_ACTION(PushValue(result), L"failed to push a value to the stack.");
						return WfRuntimeExecutionAction::ExecuteInstruction;
					}
				case WfInsCode::InvokeEvent:
					{
						Value thisValue;
						CONTEXT_ACTION(PopValue(thisValue), L"failed to pop a value from the stack.");
						CALL_DEBUGGER(callback->BreakInvoke(thisValue.GetRawPtr(), ins.eventParameter));

						auto arguments = IValueList::Create();
						for (vint i = 0; i < ins.countParameter; i++)
						{
							Value argument;
							CONTEXT_ACTION(PopValue(argument), L"failed to pop a value from the stack.");
							arguments->Insert(0, argument);
						}

						ins.eventParameter->Invoke(thisValue, arguments);
						CONTEXT_ACTION(PushValue(Value()), L"failed to push a value to the stack.");
						return WfRuntimeExecutionAction::ExecuteInstruction;
					}
				case WfInsCode::InvokeBaseCtor:
					{
						Value thisValue;
						CONTEXT_ACTION(PopValue(thisValue), L"failed to pop a value from the stack.");
						CALL_DEBUGGER(callback->BreakInvoke(thisValue.GetRawPtr(), ins.eventParameter));
						
						if (auto ctor = dynamic_cast<WfClassConstructor*>(ins.methodParameter))
						{
							if (ctor->GetGlobalContext() == globalContext.Obj())
							{
								auto capturedVariable = Ptr(new WfRuntimeVariableContext);
								capturedVariable->variables.Resize(1);
								capturedVariable->variables[0] = Value::From(thisValue.GetRawPtr());

								CONTEXT_ACTION(PushStackFrame(ctor->functionIndex, ins.countParameter, capturedVariable), L"failed to invoke a function.");
								return WfRuntimeExecutionAction::EnterStackFrame;
							}
						}

						Array<Value> arguments(ins.countParameter);
						for (vint i = 0; i < ins.countParameter; i++)
						{
							Value argument;
							CONTEXT_ACTION(PopValue(argument), L"failed to pop a value from the stack.");
							arguments[ins.countParameter - i - 1] = argument;
						}
						
						if (auto ctor = dynamic_cast<WfClassConstructor*>(ins.methodParameter))
						{
							ctor->InvokeBaseCtor(thisValue, arguments);
						}
						else
						{
							auto instance = dynamic_cast<WfClassInstance*>(thisValue.GetRawPtr());
							if (!instance)
							{
								INTERNAL_ERROR(L"Wrong class instance for invoking base constructor.");
							}

							Value baseValue = ins.methodParameter->Invoke(Value(), arguments);
							instance->InstallBaseObject(ins.methodParameter->GetOwnerTypeDescriptor(), baseValue);
						}
						CONTEXT_ACTION(PushValue(Value()), L"failed to push a value to the stack.");
						return WfRuntimeExecutionAction::ExecuteInstruction;
					}
				case WfInsCode::AttachEvent:
					{
						Value thisValue, function;
						CONTEXT_ACTION(PopValue(function), L"failed to pop a value from the stack.");
						CONTEXT_ACTION(PopValue(thisValue), L"failed to pop a value from the stack.");
						CALL_DEBUGGER(callback->BreakAttach(thisValue.GetRawPtr(), ins.eventParameter));
						auto proxy = UnboxValue<Ptr<IValueFunctionProxy>>(function);
						auto handler = ins.eventParameter->Attach(thisValue, proxy);
						CONTEXT_ACTION(PushValue(Value::From(handler)), L"failed to push a value to the stack.");
						return WfRuntimeExecutionAction::ExecuteInstruction;
					}
				case WfInsCode::DetachEvent:
					{
						Value thisValue, operand;
						CONTEXT_ACTION(PopValue(operand), L"failed to pop a value from the stack.");
						CONTEXT_ACTION(PopValue(thisValue), L"failed to pop a value from the stack.");
						CALL_DEBUGGER(callback->BreakDetach(thisValue.GetRawPtr(), ins.eventParameter));
						auto handler = UnboxValue<Ptr<IEventHandler>>(operand);
						auto result = ins.eventParameter->Detach(thisValue, handler);
						CONTEXT_ACTION(PushValue(BoxValue(result)), L"failed to push a value to the stack.");
						return WfRuntimeExecutionAction::ExecuteInstruction;
					}
				case WfInsCode::InstallTry:
					CONTEXT_ACTION(PushTrapFrame(ins.indexParameter), L"failed to push a trap frame");
					return WfRuntimeExecutionAction::ExecuteInstruction;
				case WfInsCode::UninstallTry:
					{
						if (trapFrames.Count() == 0)
						{
							INTERNAL_ERROR(L"failed to pop the trap frame.");
						}
						auto frame = GetCurrentTrapFrame();
						CONTEXT_ACTION(PopTrapFrame(ins.countParameter), L"failed to pop the trap frame.");
						return WfRuntimeExecutionAction::ExecuteInstruction;
					}
				case WfInsCode::RaiseException:
					{
						Value operand;
						CONTEXT_ACTION(PopValue(operand), L"failed to pop a value from the stack.");
						if (operand.GetValueType() == Value::BoxedValue)
						{
							WString text;
							operand.GetTypeDescriptor()->GetSerializableType()->Serialize(operand, text);
							RaiseException(text, false);
						}
						else if (auto info = operand.GetSharedPtr().Cast<WfRuntimeExceptionInfo>())
						{
							RaiseException(info);
						}
						else if (auto ex = operand.GetSharedPtr().Cast<IValueException>())
						{
							RaiseException(ex->GetMessage(), false);
						}
						else
						{
							INTERNAL_ERROR(L"failed to raise an exception which is neither a string nor a WfRuntimeExceptionInfo.");
						}
						return WfRuntimeExecutionAction::ExecuteInstruction;
					}
				case WfInsCode::TestElementInSet:
					{
						Value element, set;
						CONTEXT_ACTION(PopValue(set), L"failed to pop a value from the stack.");
						CONTEXT_ACTION(PopValue(element), L"failed to pop a value from the stack.");

						auto enumerable = UnboxValue<Ptr<IValueEnumerable>>(set);
						auto enumerator = enumerable->CreateEnumerator();
						while (enumerator->Next())
						{
							if (enumerator->GetCurrent() == element)
							{
								CONTEXT_ACTION(PushValue(BoxValue(true)), L"failed to push a value to the stack.");
								return WfRuntimeExecutionAction::ExecuteInstruction;
							}
						}
						CONTEXT_ACTION(PushValue(BoxValue(false)), L"failed to push a value to the stack.");
						return WfRuntimeExecutionAction::ExecuteInstruction;
					}
				case WfInsCode::CompareLiteral:
					BEGIN_TYPE
						EXECUTE(OpCompare, Bool)
						EXECUTE(OpCompare, I1)
						EXECUTE(OpCompare, I2)
						EXECUTE(OpCompare, I4)
						EXECUTE(OpCompare, I8)
						EXECUTE(OpCompare, U1)
						EXECUTE(OpCompare, U2)
						EXECUTE(OpCompare, U4)
						EXECUTE(OpCompare, U8)
						EXECUTE(OpCompare, F4)
						EXECUTE(OpCompare, F8)
						EXECUTE(OpCompare, String)
					END_TYPE
				case WfInsCode::CompareReference:
					{
						return OPERATOR_OpCompareReference(*this);
					}
				case WfInsCode::CompareValue:
					{
						return OPERATOR_OpCompareValue(*this);
					}
				case WfInsCode::OpNot:
					BEGIN_TYPE
						EXECUTE(OpNot_Bool, Bool)
						EXECUTE(OpNot, I1)
						EXECUTE(OpNot, I2)
						EXECUTE(OpNot, I4)
						EXECUTE(OpNot, I8)
						EXECUTE(OpNot, U1)
						EXECUTE(OpNot, U2)
						EXECUTE(OpNot, U4)
						EXECUTE(OpNot, U8)
					END_TYPE
				case WfInsCode::OpPositive:
					BEGIN_TYPE
						EXECUTE(OpPositive, I1)
						EXECUTE(OpPositive, I2)
						EXECUTE(OpPositive, I4)
						EXECUTE(OpPositive, I8)
						EXECUTE(OpPositive, U1)
						EXECUTE(OpPositive, U2)
						EXECUTE(OpPositive, U4)
						EXECUTE(OpPositive, U8)
						EXECUTE(OpPositive, F4)
						EXECUTE(OpPositive, F8)
					END_TYPE
				case WfInsCode::OpNegative:
					BEGIN_TYPE
						EXECUTE(OpNegative, I1)
						EXECUTE(OpNegative, I2)
						EXECUTE(OpNegative, I4)
						EXECUTE(OpNegative, I8)
						EXECUTE(OpNegative, F4)
						EXECUTE(OpNegative, F8)
					END_TYPE
				case WfInsCode::OpConcat:
					{
						Value first, second;
						CONTEXT_ACTION(PopValue(second), L"failed to pop a value from the stack.");
						CONTEXT_ACTION(PopValue(first), L"failed to pop a value from the stack.");

						WString firstText, secondText;
						first.GetTypeDescriptor()->GetSerializableType()->Serialize(first, firstText);
						first.GetTypeDescriptor()->GetSerializableType()->Serialize(second, secondText);

						CONTEXT_ACTION(PushValue(BoxValue(firstText + secondText)), L"failed to push a value to the stack.");
						return WfRuntimeExecutionAction::ExecuteInstruction;
					}
				case WfInsCode::OpExp:
					BEGIN_TYPE
						EXECUTE(OpExp, F4)
						EXECUTE(OpExp, F8)
					END_TYPE
				case WfInsCode::OpAdd:
					BEGIN_TYPE
						EXECUTE(OpAdd, I1)
						EXECUTE(OpAdd, I2)
						EXECUTE(OpAdd, I4)
						EXECUTE(OpAdd, I8)
						EXECUTE(OpAdd, U1)
						EXECUTE(OpAdd, U2)
						EXECUTE(OpAdd, U4)
						EXECUTE(OpAdd, U8)
						EXECUTE(OpAdd, F4)
						EXECUTE(OpAdd, F8)
					END_TYPE
				case WfInsCode::OpSub:
					BEGIN_TYPE
						EXECUTE(OpSub, I1)
						EXECUTE(OpSub, I2)
						EXECUTE(OpSub, I4)
						EXECUTE(OpSub, I8)
						EXECUTE(OpSub, U1)
						EXECUTE(OpSub, U2)
						EXECUTE(OpSub, U4)
						EXECUTE(OpSub, U8)
						EXECUTE(OpSub, F4)
						EXECUTE(OpSub, F8)
					END_TYPE
				case WfInsCode::OpMul:
					BEGIN_TYPE
						EXECUTE(OpMul, I1)
						EXECUTE(OpMul, I2)
						EXECUTE(OpMul, I4)
						EXECUTE(OpMul, I8)
						EXECUTE(OpMul, U1)
						EXECUTE(OpMul, U2)
						EXECUTE(OpMul, U4)
						EXECUTE(OpMul, U8)
						EXECUTE(OpMul, F4)
						EXECUTE(OpMul, F8)
					END_TYPE
				case WfInsCode::OpDiv:
					BEGIN_TYPE
						EXECUTE(OpDiv, I1)
						EXECUTE(OpDiv, I2)
						EXECUTE(OpDiv, I4)
						EXECUTE(OpDiv, I8)
						EXECUTE(OpDiv, U1)
						EXECUTE(OpDiv, U2)
						EXECUTE(OpDiv, U4)
						EXECUTE(OpDiv, U8)
						EXECUTE(OpDiv, F4)
						EXECUTE(OpDiv, F8)
					END_TYPE
				case WfInsCode::OpMod:
					BEGIN_TYPE
						EXECUTE(OpMod, I1)
						EXECUTE(OpMod, I2)
						EXECUTE(OpMod, I4)
						EXECUTE(OpMod, I8)
						EXECUTE(OpMod, U1)
						EXECUTE(OpMod, U2)
						EXECUTE(OpMod, U4)
						EXECUTE(OpMod, U8)
					END_TYPE
				case WfInsCode::OpShl:
					BEGIN_TYPE
						EXECUTE(OpShl, I1)
						EXECUTE(OpShl, I2)
						EXECUTE(OpShl, I4)
						EXECUTE(OpShl, I8)
						EXECUTE(OpShl, U1)
						EXECUTE(OpShl, U2)
						EXECUTE(OpShl, U4)
						EXECUTE(OpShl, U8)
					END_TYPE
				case WfInsCode::OpShr:
					BEGIN_TYPE
						EXECUTE(OpShr, I1)
						EXECUTE(OpShr, I2)
						EXECUTE(OpShr, I4)
						EXECUTE(OpShr, I8)
						EXECUTE(OpShr, U1)
						EXECUTE(OpShr, U2)
						EXECUTE(OpShr, U4)
						EXECUTE(OpShr, U8)
					END_TYPE
				case WfInsCode::OpXor:
					BEGIN_TYPE
						EXECUTE(OpXor, Bool)
						EXECUTE(OpXor, I1)
						EXECUTE(OpXor, I2)
						EXECUTE(OpXor, I4)
						EXECUTE(OpXor, I8)
						EXECUTE(OpXor, U1)
						EXECUTE(OpXor, U2)
						EXECUTE(OpXor, U4)
						EXECUTE(OpXor, U8)
					END_TYPE
				case WfInsCode::OpAnd:
					BEGIN_TYPE
						EXECUTE(OpAnd_Bool, Bool)
						EXECUTE(OpAnd, I1)
						EXECUTE(OpAnd, I2)
						EXECUTE(OpAnd, I4)
						EXECUTE(OpAnd, I8)
						EXECUTE(OpAnd, U1)
						EXECUTE(OpAnd, U2)
						EXECUTE(OpAnd, U4)
						EXECUTE(OpAnd, U8)
					END_TYPE
				case WfInsCode::OpOr:
					BEGIN_TYPE
						EXECUTE(OpOr_Bool, Bool)
						EXECUTE(OpOr, I1)
						EXECUTE(OpOr, I2)
						EXECUTE(OpOr, I4)
						EXECUTE(OpOr, I8)
						EXECUTE(OpOr, U1)
						EXECUTE(OpOr, U2)
						EXECUTE(OpOr, U4)
						EXECUTE(OpOr, U8)
					END_TYPE
				case WfInsCode::OpLT:
					{
						Value operand;
						CONTEXT_ACTION(PopValue(operand), L"failed to pop a value from the stack.");
						vint value = UnboxValue<vint>(operand);
						CONTEXT_ACTION(PushValue(BoxValue(value < 0)), L"failed to push a value to the stack.");
						return WfRuntimeExecutionAction::ExecuteInstruction;
					}
					break;
				case WfInsCode::OpGT:
					{
						Value operand;
						CONTEXT_ACTION(PopValue(operand), L"failed to pop a value from the stack.");
						vint value = UnboxValue<vint>(operand);
						CONTEXT_ACTION(PushValue(BoxValue(value > 0)), L"failed to push a value to the stack.");
						return WfRuntimeExecutionAction::ExecuteInstruction;
					}
					break;
				case WfInsCode::OpLE:
					{
						Value operand;
						CONTEXT_ACTION(PopValue(operand), L"failed to pop a value from the stack.");
						vint value = UnboxValue<vint>(operand);
						CONTEXT_ACTION(PushValue(BoxValue(value <= 0)), L"failed to push a value to the stack.");
						return WfRuntimeExecutionAction::ExecuteInstruction;
					}
					break;
				case WfInsCode::OpGE:
					{
						Value operand;
						CONTEXT_ACTION(PopValue(operand), L"failed to pop a value from the stack.");
						vint value = UnboxValue<vint>(operand);
						CONTEXT_ACTION(PushValue(BoxValue(value >= 0)), L"failed to push a value to the stack.");
						return WfRuntimeExecutionAction::ExecuteInstruction;
					}
					break;
				case WfInsCode::OpEQ:
					{
						Value operand;
						CONTEXT_ACTION(PopValue(operand), L"failed to pop a value from the stack.");
						vint value = UnboxValue<vint>(operand);
						CONTEXT_ACTION(PushValue(BoxValue(value == 0)), L"failed to push a value to the stack.");
						return WfRuntimeExecutionAction::ExecuteInstruction;
					}
					break;
				case WfInsCode::OpNE:
					{
						Value operand;
						CONTEXT_ACTION(PopValue(operand), L"failed to pop a value from the stack.");
						vint value = UnboxValue<vint>(operand);
						CONTEXT_ACTION(PushValue(BoxValue(value != 0)), L"failed to push a value to the stack.");
						return WfRuntimeExecutionAction::ExecuteInstruction;
					}
					break;
				default:
					return WfRuntimeExecutionAction::Nop;
				}
			}

			WfRuntimeExecutionAction WfRuntimeThreadContext::Execute(IWfDebuggerCallback* callback)
			{
				try
				{
					switch (status)
					{
					case WfRuntimeExecutionStatus::Ready:
					case WfRuntimeExecutionStatus::Executing:
						{
							if (stackFrames.Count() == 0)
							{
								INTERNAL_ERROR(L"empty stack frame.");
							}
							auto& stackFrame = GetCurrentStackFrame();
							if (stackFrame.nextInstructionIndex < 0 || stackFrame.nextInstructionIndex >= globalContext->assembly->instructions.Count())
							{
								INTERNAL_ERROR(L"illegal instruction index.");
							}

							auto insIndex = stackFrame.nextInstructionIndex;
							CALL_DEBUGGER(callback->BreakIns(globalContext->assembly.Obj(), insIndex));

							stackFrame.nextInstructionIndex++;
							auto& ins = globalContext->assembly->instructions[insIndex];
							return ExecuteInternal(ins, stackFrame, callback);
						}
						break;
					case WfRuntimeExecutionStatus::RaisedException:
						if (trapFrames.Count() > 0)
						{
							auto trapFrame = GetCurrentTrapFrame();
							if (trapFrame.stackFrameIndex == stackFrames.Count() - 1)
							{
								CONTEXT_ACTION(PopTrapFrame(0), L"failed to pop the trap frame");
								GetCurrentStackFrame().nextInstructionIndex = trapFrame.instructionIndex;
								status = WfRuntimeExecutionStatus::Executing;
								return WfRuntimeExecutionAction::UnwrapStack;
							}
							else if (stackFrames.Count() > 0)
							{
								CONTEXT_ACTION(PopStackFrame(), L"failed to pop the stack frame.");
								return WfRuntimeExecutionAction::UnwrapStack;
							}
						}
						break;
					default:;
					}
					return WfRuntimeExecutionAction::Nop;
				}
				catch (const WfRuntimeException& ex)
				{
					if (ex.GetInfo())
					{
						RaiseException(ex.GetInfo());
					}
					else
					{
						RaiseException(ex.Message(), ex.IsFatal());
					}
					return WfRuntimeExecutionAction::ExecuteInstruction;
				}
				catch (const Exception& ex)
				{
					RaiseException(ex.Message(), false);
					return WfRuntimeExecutionAction::ExecuteInstruction;
				}
			}

#undef INTERNAL_ERROR
#undef CONTEXT_ACTION
#undef CALL_DEBUGGER
#undef TYPE_OF_Bool
#undef TYPE_OF_I1
#undef TYPE_OF_I2
#undef TYPE_OF_I4
#undef TYPE_OF_I8
#undef TYPE_OF_U1
#undef TYPE_OF_U2
#undef TYPE_OF_U4
#undef TYPE_OF_U8
#undef TYPE_OF_F4
#undef TYPE_OF_F8
#undef TYPE_OF_String
#undef EXECUTE
#undef BEGIN_TYPE
#undef END_TYPE
		}
	}
}

#endif