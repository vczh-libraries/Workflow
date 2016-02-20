#include "WfRuntime.h"
#include <math.h>

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
				context.PushValue(BoxValue(value));\
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
				context.PushValue(BoxValue(value));\
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
				T value = exp(secondValue * log(firstValue));
				context.PushValue(BoxValue(value));
				return WfRuntimeExecutionAction::ExecuteInstruction;
			}
			
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
						context.PushValue(BoxValue((vint)0));
					}
					else
					{
						context.PushValue(BoxValue((vint)-1));
					}
				}
				else
				{
					if (secondNull)
					{context.PushValue(BoxValue((vint)1));
					}
					else
					{
						T firstValue = UnboxValue<T>(first);
						T secondValue = UnboxValue<T>(second);
						if (firstValue < secondValue)
						{
							context.PushValue(BoxValue((vint)-1));
						}
						else if (firstValue > secondValue)
						{
							context.PushValue(BoxValue((vint)1));
						}
						else
						{
							context.PushValue(BoxValue((vint)0));
						}
					}
				}
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
					if (result.GetValueType() == Value::Text)
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
					if (result.GetValueType() == Value::Text)
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
				case Value::Text:
					if (result.GetValueType() != Value::Text)
					{
						return false;
					}
					else if (ins.typeDescriptorParameter == GetTypeDescriptor<void>())
					{
						if (result.GetText() != L"")
						{
							return false;
						}
					}
					else
					{
						auto serializer = ins.typeDescriptorParameter->GetValueSerializer();
						if (!serializer)
						{
							return false;
						}
						if (!serializer->Parse(result.GetText(), converted))
						{
							return false;
						}
					}
					break;
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
				auto enumerable = MakePtr<WfRuntimeRange<T>>(firstValue, secondValue);
				context.PushValue(Value::From(enumerable));
				return WfRuntimeExecutionAction::ExecuteInstruction;
			}
			
/***********************************************************************
WfRuntimeThreadContext (ReverseEnumerable)
***********************************************************************/
			
			Value OPERATOR_OpReverseEnumerable(Value operand)
			{
				auto enumerable = UnboxValue<Ptr<IValueEnumerable>>(operand);
				auto list = enumerable.Cast<IValueReadonlyList>();
				if (!list)
				{
					list = IValueList::Create(GetLazyList<Value>(enumerable));
				}
				return Value::From(MakePtr<WfRuntimeReverseEnumerable>(list));
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
				auto lambda = MakePtr<WfRuntimeLambda>(context, nullptr, functionIndex);
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
					PushValue(ins.valueParameter);
					return WfRuntimeExecutionAction::ExecuteInstruction;
				case WfInsCode::LoadFunction:
					{
						PushValue(BoxValue(ins.indexParameter));
						return WfRuntimeExecutionAction::ExecuteInstruction;
					}
				case WfInsCode::LoadException:
					{
						PushValue(BoxValue(exceptionInfo));
						return WfRuntimeExecutionAction::ExecuteInstruction;
					}
				case WfInsCode::LoadLocalVar:
					{
						Value operand;
						CONTEXT_ACTION(LoadLocalVariable(ins.indexParameter, operand), L"illegal local variable index.");
						PushValue(operand);
						return WfRuntimeExecutionAction::ExecuteInstruction;
					}
				case WfInsCode::LoadCapturedVar:
					{
						Value operand;
						CONTEXT_ACTION(LoadCapturedVariable(ins.indexParameter, operand), L"illegal captured variable index.");
						PushValue(operand);
						return WfRuntimeExecutionAction::ExecuteInstruction;
					}
				case WfInsCode::LoadGlobalVar:
					{
						CALL_DEBUGGER(callback->BreakRead(globalContext->assembly.Obj(), ins.indexParameter));
						Value operand;
						CONTEXT_ACTION(LoadGlobalVariable(ins.indexParameter, operand), L"illegal global variable index.");
						PushValue(operand);
						return WfRuntimeExecutionAction::ExecuteInstruction;
					}
				case WfInsCode::LoadMethodInfo:
					{
						PushValue(Value::From(ins.methodParameter));
						return WfRuntimeExecutionAction::ExecuteInstruction;
					}
				case WfInsCode::LoadMethodClosure:
					{
						Value operand;
						CONTEXT_ACTION(PopValue(operand), L"failed to pop a value from the stack.");
						auto closure = ins.methodParameter->CreateFunctionProxy(operand);
						PushValue(closure);
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
						PushValue(operand);
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
						PushValue(operand);
						if (stackFrames.Count() == 0)
						{
							status = WfRuntimeExecutionStatus::Finished;
						}
						return WfRuntimeExecutionAction::ExitStackFrame;
					}
				case WfInsCode::CreateArray:
					{
						auto list = IValueList::Create();
						Value operand;
						for (vint i = 0; i < ins.countParameter; i++)
						{
							CONTEXT_ACTION(PopValue(operand), L"failed to pop a value from the stack.");
							list->Add(operand);
						}
						PushValue(Value::From(list));
						return WfRuntimeExecutionAction::ExecuteInstruction;
					}
				case WfInsCode::CreateMap:
					{
						auto map = IValueDictionary::Create();
						Value key, value;
						for (vint i = 0; i < ins.countParameter; i+=2)
						{
							CONTEXT_ACTION(PopValue(value), L"failed to pop a value from the stack.");
							CONTEXT_ACTION(PopValue(key), L"failed to pop a value from the stack.");
							map->Set(key, value);
						}
						PushValue(Value::From(map));
						return WfRuntimeExecutionAction::ExecuteInstruction;
					}
				case WfInsCode::CreateClosuerContext:
					{
						Ptr<WfRuntimeVariableContext> capturedVariables;
						if (ins.countParameter > 0)
						{
							capturedVariables = new WfRuntimeVariableContext;
							capturedVariables->variables.Resize(ins.countParameter);
							Value operand;
							for (vint i = 0; i < ins.countParameter; i++)
							{
								CONTEXT_ACTION(PopValue(operand), L"failed to pop a value from the stack.");
								capturedVariables->variables[ins.countParameter - 1 - i] = operand;
							}
						}

						PushValue(Value::From(capturedVariables));
						return WfRuntimeExecutionAction::ExecuteInstruction;
					}
				case WfInsCode::CreateClosure:
					{
						Value context, function;
						CONTEXT_ACTION(PopValue(function), L"failed to pop a value from the stack.");
						CONTEXT_ACTION(PopValue(context), L"failed to pop a value from the stack.");
						auto capturedVariables = context.GetSharedPtr().Cast<WfRuntimeVariableContext>();
						auto functionIndex = UnboxValue<vint>(function);

						auto lambda = MakePtr<WfRuntimeLambda>(globalContext, capturedVariables, functionIndex);
						PushValue(Value::From(lambda));
						return WfRuntimeExecutionAction::ExecuteInstruction;
					}
				case WfInsCode::CreateInterface:
					{
						auto proxy = MakePtr<WfRuntimeInterfaceInstance>();
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
						proxy->globalContext = globalContext;

						Array<Value> arguments(1);
						arguments[0] = Value::From(proxy);
						auto obj = ins.methodParameter->Invoke(Value(), arguments);
						capturedVariables->variables[capturedVariables->variables.Count() - 1] = Value::From(obj.GetRawPtr());

						PushValue(obj);
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
				case WfInsCode::ReverseEnumerable:
					{
						Value operand;
						CONTEXT_ACTION(PopValue(operand), L"failed to pop a value from the stack.");
						Value reversedEnumerable = OPERATOR_OpReverseEnumerable(operand);
						PushValue(reversedEnumerable);
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
							PushValue(converted);
							return WfRuntimeExecutionAction::ExecuteInstruction;
						}
						else
						{
							WString from = result.IsNull() ? L"<null>" : L"<" + result.GetText() + L"> of " + result.GetTypeDescriptor()->GetTypeName();
							WString to = ins.typeDescriptorParameter->GetTypeName();
							RaiseException(L"Failed to convert from \"" + from + L"\" to \"" + to + L"\".", false);
							return WfRuntimeExecutionAction::Nop;
						}
					}
				case WfInsCode::TryConvertToType:
					{
						Value result, converted;
						CONTEXT_ACTION(PopValue(result), L"failed to pop a value from the stack.");
						if (OPERATOR_OpConvertToType(result, converted, ins))
						{
							PushValue(converted);
						}
						else
						{
							PushValue(Value());
						}
						return WfRuntimeExecutionAction::ExecuteInstruction;
					}
				case WfInsCode::TestType:
					{
						Value operand;
						CONTEXT_ACTION(PopValue(operand), L"failed to pop a value from the stack.");
						if (operand.GetTypeDescriptor() && operand.GetValueType() == ins.flagParameter && operand.GetTypeDescriptor()->CanConvertTo(ins.typeDescriptorParameter))
						{
							PushValue(BoxValue(true));
						}
						else
						{
							PushValue(BoxValue(false));
						}
						return WfRuntimeExecutionAction::ExecuteInstruction;
					}
				case WfInsCode::GetType:
					{
						Value operand;
						CONTEXT_ACTION(PopValue(operand), L"failed to pop a value from the stack.");
						PushValue(Value::From(operand.GetTypeDescriptor()));
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
				case WfInsCode::GetProperty:
					{
						Value operand;
						CONTEXT_ACTION(PopValue(operand), L"failed to pop a value from the stack.");
						CALL_DEBUGGER(callback->BreakGet(operand.GetRawPtr(), ins.propertyParameter));
						Value result = ins.propertyParameter->GetValue(operand);
						PushValue(result);
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

						Ptr<IValueList> list = new ValueListWrapper<List<Value>*>(&arguments);
						Value result = proxy->Invoke(list);
						PushValue(result);
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

						Array<Value> arguments(ins.countParameter);
						for (vint i = 0; i < ins.countParameter; i++)
						{
							Value argument;
							CONTEXT_ACTION(PopValue(argument), L"failed to pop a value from the stack.");
							arguments[ins.countParameter - i - 1] = argument;
						}

						Value result = ins.methodParameter->Invoke(thisValue, arguments);
						PushValue(result);
						return WfRuntimeExecutionAction::ExecuteInstruction;
					}
				case WfInsCode::InvokeEvent:
					{
						Value thisValue;
						CONTEXT_ACTION(PopValue(thisValue), L"failed to pop a value from the stack.");
						CALL_DEBUGGER(callback->BreakInvoke(thisValue.GetRawPtr(), ins.eventParameter));

						Array<Value> arguments(ins.countParameter);
						for (vint i = 0; i < ins.countParameter; i++)
						{
							Value argument;
							CONTEXT_ACTION(PopValue(argument), L"failed to pop a value from the stack.");
							arguments[ins.countParameter - i - 1] = argument;
						}

						ins.eventParameter->Invoke(thisValue, arguments);
						PushValue(Value());
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
						PushValue(Value::From(handler));
						return WfRuntimeExecutionAction::ExecuteInstruction;
					}
				case WfInsCode::DetachEvent:
					{
						Value operand;
						CONTEXT_ACTION(PopValue(operand), L"failed to pop a value from the stack.");
						auto handler = UnboxValue<Ptr<IEventHandler>>(operand);
						CALL_DEBUGGER(callback->BreakDetach(handler->GetOwnerObject().GetRawPtr(), handler->GetOwnerEvent()));
						auto result = handler->Detach();
						PushValue(BoxValue(result));
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
						if (operand.GetValueType() == Value::Text)
						{
							RaiseException(operand.GetText(), false);
						}
						else if (auto info = operand.GetSharedPtr().Cast<WfRuntimeExceptionInfo>())
						{
							RaiseException(info);
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
								PushValue(BoxValue(true));
								return WfRuntimeExecutionAction::ExecuteInstruction;
							}
						}
						PushValue(BoxValue(false));
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
				case WfInsCode::CompareStruct:
					{
						Value first, second;
						CONTEXT_ACTION(PopValue(second), L"failed to pop a value from the stack.");
						CONTEXT_ACTION(PopValue(first), L"failed to pop a value from the stack.");
						if (!first.IsNull() && !first.GetTypeDescriptor()->GetValueSerializer())
						{
							INTERNAL_ERROR(L"type" + first.GetTypeDescriptor()->GetTypeName() + L" is not a struct.");
						}
						if (!second.IsNull() && !second.GetTypeDescriptor()->GetValueSerializer())
						{
							INTERNAL_ERROR(L"type" + second.GetTypeDescriptor()->GetTypeName() + L" is not a struct.");
						}

						if (first.GetValueType() != second.GetValueType())
						{
							PushValue(BoxValue(false));
						}
						else if (first.IsNull())
						{
							PushValue(BoxValue(true));
						}
						else
						{
							PushValue(BoxValue(first.GetText() == second.GetText()));
						}
						return WfRuntimeExecutionAction::ExecuteInstruction;
					}
				case WfInsCode::CompareReference:
					{
						Value first, second;
						CONTEXT_ACTION(PopValue(second), L"failed to pop a value from the stack.");
						CONTEXT_ACTION(PopValue(first), L"failed to pop a value from the stack.");
						bool result = first.GetValueType() != Value::Text && second.GetValueType() != Value::Text && first.GetRawPtr() == second.GetRawPtr();
						PushValue(BoxValue(result));
						return WfRuntimeExecutionAction::ExecuteInstruction;
					}
				case WfInsCode::CompareValue:
					{
						Value first, second;
						CONTEXT_ACTION(PopValue(second), L"failed to pop a value from the stack.");
						CONTEXT_ACTION(PopValue(first), L"failed to pop a value from the stack.");
						switch (first.GetValueType())
						{
						case Value::RawPtr:
						case Value::SharedPtr:
							switch (first.GetValueType())
							{
							case Value::RawPtr:
							case Value::SharedPtr:
								PushValue(BoxValue(first.GetRawPtr() == second.GetRawPtr()));
								break;
							default:
								PushValue(BoxValue(false));
							}
							break;
						case Value::Text:
							switch (first.GetValueType())
							{
							case Value::Text:
								PushValue(BoxValue(first.GetText() == second.GetText()));
							default:
								PushValue(BoxValue(false));
							}
							break;
						default:
							PushValue(BoxValue(second.IsNull()));
						}
						return WfRuntimeExecutionAction::ExecuteInstruction;
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
						PushValue(BoxValue(first.GetText() + second.GetText()));
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
						PushValue(BoxValue(value < 0));
						return WfRuntimeExecutionAction::ExecuteInstruction;
					}
					break;
				case WfInsCode::OpGT:
					{
						Value operand;
						CONTEXT_ACTION(PopValue(operand), L"failed to pop a value from the stack.");
						vint value = UnboxValue<vint>(operand);
						PushValue(BoxValue(value > 0));
						return WfRuntimeExecutionAction::ExecuteInstruction;
					}
					break;
				case WfInsCode::OpLE:
					{
						Value operand;
						CONTEXT_ACTION(PopValue(operand), L"failed to pop a value from the stack.");
						vint value = UnboxValue<vint>(operand);
						PushValue(BoxValue(value <= 0));
						return WfRuntimeExecutionAction::ExecuteInstruction;
					}
					break;
				case WfInsCode::OpGE:
					{
						Value operand;
						CONTEXT_ACTION(PopValue(operand), L"failed to pop a value from the stack.");
						vint value = UnboxValue<vint>(operand);
						PushValue(BoxValue(value >= 0));
						return WfRuntimeExecutionAction::ExecuteInstruction;
					}
					break;
				case WfInsCode::OpEQ:
					{
						Value operand;
						CONTEXT_ACTION(PopValue(operand), L"failed to pop a value from the stack.");
						vint value = UnboxValue<vint>(operand);
						PushValue(BoxValue(value == 0));
						return WfRuntimeExecutionAction::ExecuteInstruction;
					}
					break;
				case WfInsCode::OpNE:
					{
						Value operand;
						CONTEXT_ACTION(PopValue(operand), L"failed to pop a value from the stack.");
						vint value = UnboxValue<vint>(operand);
						PushValue(BoxValue(value != 0));
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
				catch (const Error& ex)
				{
					RaiseException(ex.Description(), false);
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