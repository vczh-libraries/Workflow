#include "WfRuntime.h"

#ifdef VCZH_DESCRIPTABLEOBJECT_WITH_METADATA

namespace vl
{
	using namespace reflection;
	using namespace reflection::description;
	using namespace workflow::runtime;
	using namespace collections;

	namespace workflow
	{
		namespace runtime
		{

/***********************************************************************
WfRuntimeGlobalContext
***********************************************************************/

			WfRuntimeGlobalContext::WfRuntimeGlobalContext(Ptr<WfAssembly> _assembly)
				:assembly(_assembly)
			{
				globalVariables = Ptr(new WfRuntimeVariableContext);
				globalVariables->variables.Resize(assembly->variableNames.Count());
				if (assembly->typeImpl)
				{
					assembly->typeImpl->SetGlobalContext(this);
				}
			}

			WfRuntimeGlobalContext::~WfRuntimeGlobalContext()
			{
				if (assembly->typeImpl)
				{
					assembly->typeImpl->SetGlobalContext(nullptr);
				}
			}

/***********************************************************************
WfRuntimeCallStackInfo
***********************************************************************/

			Ptr<IValueReadonlyDictionary> WfRuntimeCallStackInfo::GetVariables(collections::List<WString>& names, Ptr<WfRuntimeVariableContext> context, Ptr<IValueReadonlyDictionary>& cache)
			{
				if (!cache)
				{
					if (!context)
					{
						Dictionary<WString, Value> map;
						for (auto [name, index] : indexed(names))
						{
							map.Add(name, context->variables[index]);
						}
						cache = IValueDictionary::Create(
							From(map)
								.Select([](Pair<WString, Value> pair)
								{
									return Pair<Value, Value>(BoxValue(pair.key), pair.value);
								})
							);
					}
					else
					{
						cache = IValueDictionary::Create();
					}
				}
				return cache;
			}

			WfRuntimeCallStackInfo::WfRuntimeCallStackInfo()
			{
			}

			WfRuntimeCallStackInfo::WfRuntimeCallStackInfo(WfRuntimeThreadContext* context, const WfRuntimeStackFrame& stackFrame)
			{
				assembly = context->globalContext->assembly;
				functionIndex = stackFrame.functionIndex;
				instruction = stackFrame.nextInstructionIndex - 1;

				auto function = assembly->functions[functionIndex];

				if (context->globalContext->globalVariables->variables.Count() > 0)
				{
					global = context->globalContext->globalVariables;
				}

				captured = stackFrame.capturedVariables;

				if (function->argumentNames.Count() > 0)
				{
					arguments = Ptr(new WfRuntimeVariableContext);
					arguments->variables.Resize(function->argumentNames.Count());
					for (vint i = 0; i < arguments->variables.Count(); i++)
					{
						arguments->variables[i] = context->stack[stackFrame.stackBase + i];
					}
				}

				if (function->localVariableNames.Count()>0)
				{
					localVariables = Ptr(new WfRuntimeVariableContext);
					localVariables->variables.Resize(function->localVariableNames.Count());
					for (vint i = 0; i < localVariables->variables.Count(); i++)
					{
						localVariables->variables[i] = context->stack[stackFrame.stackBase + function->argumentNames.Count() + i];
					}
				}
			}

			WfRuntimeCallStackInfo::~WfRuntimeCallStackInfo()
			{
			}

			Ptr<IValueReadonlyDictionary> WfRuntimeCallStackInfo::GetLocalVariables()
			{
				return GetVariables(assembly->functions[functionIndex]->localVariableNames, localVariables, cachedLocalVariables);
			}

			Ptr<IValueReadonlyDictionary> WfRuntimeCallStackInfo::GetLocalArguments()
			{
				return GetVariables(assembly->functions[functionIndex]->argumentNames, arguments, cachedLocalArguments);
			}

			Ptr<IValueReadonlyDictionary> WfRuntimeCallStackInfo::GetCapturedVariables()
			{
				return GetVariables(assembly->functions[functionIndex]->capturedVariableNames, captured, cachedCapturedVariables);
			}

			Ptr<IValueReadonlyDictionary> WfRuntimeCallStackInfo::GetGlobalVariables()
			{
				return GetVariables(assembly->variableNames, global, cachedGlobalVariables);
			}

			WString WfRuntimeCallStackInfo::GetFunctionName()
			{
				if (!assembly)
				{
					return L"<EXTERNAL CODE>";
				}
				return assembly->functions[functionIndex]->name;
			}

			WString WfRuntimeCallStackInfo::GetSourceCodeBeforeCodegen()
			{
				if (!assembly)
				{
					return L"";
				}
				const auto& range = assembly->insBeforeCodegen->instructionCodeMapping[instruction];
				if (range.codeIndex == -1)
				{
					return L"";
				}
				return assembly->insBeforeCodegen->moduleCodes[range.codeIndex];
			}

			WString WfRuntimeCallStackInfo::GetSourceCodeAfterCodegen()
			{
				if (!assembly)
				{
					return L"";
				}
				const auto& range = assembly->insAfterCodegen->instructionCodeMapping[instruction];
				if (range.codeIndex == -1)
				{
					return L"";
				}
				return assembly->insAfterCodegen->moduleCodes[range.codeIndex];
			}

			vint WfRuntimeCallStackInfo::GetRowBeforeCodegen()
			{
				if (!assembly)
				{
					return -1;
				}
				const auto& range = assembly->insBeforeCodegen->instructionCodeMapping[instruction];
				return range.start.row;
			}

			vint WfRuntimeCallStackInfo::GetRowAfterCodegen()
			{
				if (!assembly)
				{
					return -1;
				}
				const auto& range = assembly->insAfterCodegen->instructionCodeMapping[instruction];
				return range.start.row;
			}

/***********************************************************************
WfRuntimeExceptionInfo
***********************************************************************/

			WfRuntimeExceptionInfo::WfRuntimeExceptionInfo(const WString& _message, bool _fatal)
				:message(_message)
				, fatal(_fatal)
			{
			}

			WfRuntimeExceptionInfo::~WfRuntimeExceptionInfo()
			{
			}
				
#pragma push_macro("GetMessage")
#if defined GetMessage
#undef GetMessage
#endif
			WString WfRuntimeExceptionInfo::GetMessage()
			{
				return message;
			}
#pragma pop_macro("GetMessage")

			bool WfRuntimeExceptionInfo::GetFatal()
			{
				return fatal;
			}

			Ptr<IValueReadonlyList> WfRuntimeExceptionInfo::GetCallStack()
			{
				if (!cachedCallStack)
				{
					cachedCallStack = IValueList::Create(
						From(callStack)
							.Cast<IValueCallStack>()
							.Select([](Ptr<IValueCallStack> callStack)
							{
								return BoxValue(callStack);
							})
						);
				}
				return cachedCallStack;
			}

/***********************************************************************
WfRuntimeThreadContext
***********************************************************************/

			WfRuntimeThreadContext::WfRuntimeThreadContext(Ptr<WfRuntimeGlobalContext> _context)
				:globalContext(_context)
			{
			}

			WfRuntimeThreadContext::WfRuntimeThreadContext(Ptr<WfAssembly> _assembly)
				:globalContext(new WfRuntimeGlobalContext(_assembly))
			{
			}

			WfRuntimeStackFrame& WfRuntimeThreadContext::GetCurrentStackFrame()
			{
				return stackFrames[stackFrames.Count() - 1];
			}

			WfRuntimeThreadContextError WfRuntimeThreadContext::PushStackFrame(vint functionIndex, vint argumentCount, Ptr<WfRuntimeVariableContext> capturedVariables)
			{
				if (stackFrames.Count() == 0)
				{
					if (stack.Count() < argumentCount)
					{
						return WfRuntimeThreadContextError::StackCorrupted;
					}
				}
				else
				{
					auto& frame = GetCurrentStackFrame();
					if (stack.Count() - frame.freeStackBase < argumentCount)
					{
						return WfRuntimeThreadContextError::StackCorrupted;
					}
				}
				if (functionIndex < 0 || functionIndex >= globalContext->assembly->functions.Count())
				{
					return WfRuntimeThreadContextError::WrongFunctionIndex;
				}
				auto meta = globalContext->assembly->functions[functionIndex];
				if (meta->argumentNames.Count() != argumentCount)
				{
					return WfRuntimeThreadContextError::WrongArgumentCount;
				}
				if (meta->capturedVariableNames.Count() == 0)
				{
					if (capturedVariables)
					{
						return WfRuntimeThreadContextError::WrongCapturedVariableCount;
					}
				}
				else
				{
					if (!capturedVariables || capturedVariables->variables.Count() != meta->capturedVariableNames.Count())
					{
						return WfRuntimeThreadContextError::WrongCapturedVariableCount;
					}
				}

				WfRuntimeStackFrame frame;
				frame.capturedVariables = capturedVariables;
				frame.functionIndex = functionIndex;
				frame.nextInstructionIndex = globalContext->assembly->functions[functionIndex]->firstInstruction;
				frame.stackBase = stack.Count() - argumentCount;

				frame.fixedVariableCount = meta->argumentNames.Count() + meta->localVariableNames.Count();
				frame.freeStackBase = frame.stackBase + frame.fixedVariableCount;
				stackFrames.Add(frame);

				for (vint i = 0; i < meta->localVariableNames.Count(); i++)
				{
					stack.Add(Value());
				}
				if (status == WfRuntimeExecutionStatus::Finished || status == WfRuntimeExecutionStatus::FatalError)
				{
					status = WfRuntimeExecutionStatus::Ready;
				}
				return WfRuntimeThreadContextError::Success;
			}

			WfRuntimeThreadContextError WfRuntimeThreadContext::PopStackFrame()
			{
				if (stackFrames.Count() == 0) return WfRuntimeThreadContextError::EmptyStackFrame;
				WfRuntimeStackFrame frame = GetCurrentStackFrame();
				if (trapFrames.Count() > 0)
				{
					WfRuntimeTrapFrame& trapFrame = GetCurrentTrapFrame();
					if (trapFrame.stackFrameIndex == stackFrames.Count() - 1)
					{
						return WfRuntimeThreadContextError::TrapFrameCorrupted;
					}
				}
				stackFrames.RemoveAt(stackFrames.Count() - 1);

				if (stack.Count() > frame.stackBase)
				{
					stack.RemoveRange(frame.stackBase, stack.Count() - frame.stackBase);
				}
				return WfRuntimeThreadContextError::Success;
			}

			WfRuntimeTrapFrame& WfRuntimeThreadContext::GetCurrentTrapFrame()
			{
				return trapFrames[trapFrames.Count() - 1];
			}

			WfRuntimeThreadContextError WfRuntimeThreadContext::PushTrapFrame(vint instructionIndex)
			{
				WfRuntimeTrapFrame frame;
				frame.stackFrameIndex = stackFrames.Count() - 1;
				frame.instructionIndex = instructionIndex;
				frame.stackPatternCount = stack.Count();
				trapFrames.Add(frame);
				return WfRuntimeThreadContextError::Success;
			}

			WfRuntimeThreadContextError WfRuntimeThreadContext::PopTrapFrame(vint saveStackPatternCount)
			{
				if (trapFrames.Count() == 0) return WfRuntimeThreadContextError::EmptyTrapFrame;
				WfRuntimeTrapFrame& frame = trapFrames[trapFrames.Count() - 1];
				if (frame.stackFrameIndex != stackFrames.Count() - 1) return WfRuntimeThreadContextError::TrapFrameCorrupted;

				vint stackPopCount = stack.Count() - frame.stackPatternCount - saveStackPatternCount;
				if (stackPopCount < 0)
				{
					return WfRuntimeThreadContextError::StackCorrupted;
				}
				else if (stackPopCount>0)
				{
					stack.RemoveRange(stack.Count() - stackPopCount, stackPopCount);
				}

				trapFrames.RemoveAt(trapFrames.Count() - 1);
				return WfRuntimeThreadContextError::Success;
			}

			WfRuntimeThreadContextError WfRuntimeThreadContext::PushRuntimeValue(const WfRuntimeValue& value)
			{
				if (value.typeDescriptor)
				{
					switch (value.type)
					{
					case WfInsType::Unknown:
						return PushValue(BoxValue(value.typeDescriptor));
					case WfInsType::U8:
						CHECK_ERROR(value.typeDescriptor->GetEnumType(), L"WfRuntimeThreadContext::PushValue(const WfRuntimeValue&)#Missing typeDescriptor in WfRuntimeValue!");
						return PushValue(value.typeDescriptor->GetEnumType()->ToEnum(value.u8Value));
					default:
						CHECK_FAIL(L"WfRuntimeThreadContext::PushValue(const WfRuntimeValue&)#Unexpected type in WfRuntimeValue with typeDescriptor!");
					}
				}
				else
				{
					switch (value.type)
					{
					case WfInsType::Bool:
						return PushValue(BoxValue(value.boolValue));
					case WfInsType::I1:
						return PushValue(BoxValue(value.i1Value));
					case WfInsType::I2:
						return PushValue(BoxValue(value.i2Value));
					case WfInsType::I4:
						return PushValue(BoxValue(value.i4Value));
					case WfInsType::I8:
						return PushValue(BoxValue(value.i8Value));
					case WfInsType::U1:
						return PushValue(BoxValue(value.u1Value));
					case WfInsType::U2:
						return PushValue(BoxValue(value.u2Value));
					case WfInsType::U4:
						return PushValue(BoxValue(value.u4Value));
					case WfInsType::U8:
						return PushValue(BoxValue(value.u8Value));
					case WfInsType::F4:
						return PushValue(BoxValue(value.f4Value));
					case WfInsType::F8:
						return PushValue(BoxValue(value.f8Value));
					case WfInsType::String:
						return PushValue(BoxValue(value.stringValue));
					default:
						return PushValue(Value());
					}
				}
			}

			WfRuntimeThreadContextError WfRuntimeThreadContext::PushValue(const reflection::description::Value& value)
			{
				stack.Add(value);
				return WfRuntimeThreadContextError::Success;
			}

			WfRuntimeThreadContextError WfRuntimeThreadContext::PopValue(reflection::description::Value& value)
			{
				if (stackFrames.Count() == 0)
				{
					if (stack.Count() == 0) return WfRuntimeThreadContextError::EmptyStack;
				}
				else
				{
					WfRuntimeStackFrame& frame = GetCurrentStackFrame();
					if (stack.Count() <= frame.freeStackBase) return WfRuntimeThreadContextError::StackCorrupted;
				}
				value = stack[stack.Count() - 1];
				stack.RemoveAt(stack.Count() - 1);
				return WfRuntimeThreadContextError::Success;
			}

			WfRuntimeThreadContextError WfRuntimeThreadContext::RaiseException(const WString& exception, bool fatalError, bool skipDebugger)
			{
				auto info = Ptr(new WfRuntimeExceptionInfo(exception, fatalError));
				return RaiseException(info, skipDebugger);
			}

			WfRuntimeThreadContextError WfRuntimeThreadContext::RaiseException(Ptr<WfRuntimeExceptionInfo> info, bool skipDebugger)
			{
				exceptionInfo = info;
				status = info->fatal ? WfRuntimeExecutionStatus::FatalError : WfRuntimeExecutionStatus::RaisedException;

				if (info->callStack.Count() == 0)
				{
					if (auto debugger = GetDebuggerForCurrentThread())
					{
						vint contextCount = debugger->GetThreadContexts().Count();
						for (vint i = contextCount - 1; i >= 0; i--)
						{
							auto context = debugger->GetThreadContexts()[i];
							vint stackCount = context->stackFrames.Count();
							for (vint j = stackCount - 1; j >= 0; j--)
							{
								const auto& stackFrame = context->stackFrames[j];
								info->callStack.Add(Ptr(new WfRuntimeCallStackInfo(context, stackFrame)));
							}

							if (i > 0)
							{
								info->callStack.Add(Ptr(new WfRuntimeCallStackInfo));
							}
						}

						if (!skipDebugger)
						{
							if (auto callback = GetDebuggerCallback(debugger.Obj()))
							{
								if (callback->BreakException(info))
								{
									if (!callback->WaitForContinue())
									{
										RaiseException(L"Internal error: Debugger stopped the program.", true, true);
									}
								}
							}
						}
					}
				}

				return WfRuntimeThreadContextError::Success;
			}

			WfRuntimeThreadContextError WfRuntimeThreadContext::LoadStackValue(vint stackItemIndex, reflection::description::Value& value)
			{
				if (stackFrames.Count() == 0) return WfRuntimeThreadContextError::EmptyStackFrame;
				auto frame = GetCurrentStackFrame();
				if (stackItemIndex < frame.freeStackBase || stackItemIndex >= stack.Count())
				{
					return WfRuntimeThreadContextError::WrongVariableIndex;
				}

				value = stack[stackItemIndex];
				return WfRuntimeThreadContextError::Success;
			}

			WfRuntimeThreadContextError WfRuntimeThreadContext::LoadGlobalVariable(vint variableIndex, reflection::description::Value& value)
			{
				if (variableIndex < 0 || variableIndex >= globalContext->globalVariables->variables.Count())
				{
					return WfRuntimeThreadContextError::WrongVariableIndex;
				}

				value = globalContext->globalVariables->variables[variableIndex];
				return WfRuntimeThreadContextError::Success;
			}

			WfRuntimeThreadContextError WfRuntimeThreadContext::StoreGlobalVariable(vint variableIndex, const reflection::description::Value& value)
			{
				if (variableIndex < 0 || variableIndex >= globalContext->globalVariables->variables.Count())
				{
					return WfRuntimeThreadContextError::WrongVariableIndex;
				}

				globalContext->globalVariables->variables[variableIndex] = value;
				return WfRuntimeThreadContextError::Success;
			}

			WfRuntimeThreadContextError WfRuntimeThreadContext::LoadCapturedVariable(vint variableIndex, reflection::description::Value& value)
			{
				if (stackFrames.Count() == 0) return WfRuntimeThreadContextError::EmptyStackFrame;
				auto frame = GetCurrentStackFrame();
				if (variableIndex < 0 || variableIndex >= frame.capturedVariables->variables.Count())
				{
					return WfRuntimeThreadContextError::WrongVariableIndex;
				}

				value = frame.capturedVariables->variables[variableIndex];
				return WfRuntimeThreadContextError::Success;
			}

			WfRuntimeThreadContextError WfRuntimeThreadContext::StoreCapturedVariable(vint variableIndex, const reflection::description::Value& value)
			{
				if (stackFrames.Count() == 0) return WfRuntimeThreadContextError::EmptyStackFrame;
				auto frame = GetCurrentStackFrame();
				if (variableIndex < 0 || variableIndex >= frame.capturedVariables->variables.Count())
				{
					return WfRuntimeThreadContextError::WrongVariableIndex;
				}

				frame.capturedVariables->variables[variableIndex] = value;
				return WfRuntimeThreadContextError::Success;
			}

			WfRuntimeThreadContextError WfRuntimeThreadContext::LoadLocalVariable(vint variableIndex, reflection::description::Value& value)
			{
				if (stackFrames.Count() == 0) return WfRuntimeThreadContextError::EmptyStackFrame;
				auto frame = GetCurrentStackFrame();
				if (variableIndex < 0 || variableIndex >= frame.fixedVariableCount)
				{
					return WfRuntimeThreadContextError::WrongVariableIndex;
				}

				value = stack[frame.stackBase + variableIndex];
				return WfRuntimeThreadContextError::Success;
			}

			WfRuntimeThreadContextError WfRuntimeThreadContext::StoreLocalVariable(vint variableIndex, const reflection::description::Value& value)
			{
				if (stackFrames.Count() == 0) return WfRuntimeThreadContextError::EmptyStackFrame;
				auto frame = GetCurrentStackFrame();
				if (variableIndex < 0 || variableIndex >= frame.fixedVariableCount)
				{
					return WfRuntimeThreadContextError::WrongVariableIndex;
				}

				stack[frame.stackBase + variableIndex] = value;
				return WfRuntimeThreadContextError::Success;
			}

			void WfRuntimeThreadContext::ExecuteToEnd()
			{
				auto callback = GetDebuggerCallback();
				if (callback)
				{
					callback->EnterThreadContext(this);
				}
				while (Execute(callback) != WfRuntimeExecutionAction::Nop);
				if (callback)
				{
					callback->LeaveThreadContext(this);
				}
			}
		}
	}
}

#endif