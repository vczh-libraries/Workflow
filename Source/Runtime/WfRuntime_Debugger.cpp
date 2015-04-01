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

/***********************************************************************
IWfDebuggerCallback
***********************************************************************/

			WfBreakPoint WfBreakPoint::Ins(WfAssembly* assembly, vint instruction)
			{
				WfBreakPoint breakPoint;
				breakPoint.type = Instruction;
				breakPoint.assembly = assembly;
				breakPoint.instruction = instruction;
				return breakPoint;
			}

			WfBreakPoint WfBreakPoint::Read(WfAssembly* assembly, vint variable)
			{
				WfBreakPoint breakPoint;
				breakPoint.type = ReadGlobalVar;
				breakPoint.assembly = assembly;
				breakPoint.variable = variable;
				return breakPoint;
			}

			WfBreakPoint WfBreakPoint::Write(WfAssembly* assembly, vint variable)
			{
				WfBreakPoint breakPoint;
				breakPoint.type = WriteGlobalVar;
				breakPoint.assembly = assembly;
				breakPoint.variable = variable;
				return breakPoint;
			}

			WfBreakPoint WfBreakPoint::Get(reflection::DescriptableObject* thisObject, reflection::description::IPropertyInfo* propertyInfo)
			{
				WfBreakPoint breakPoint;
				breakPoint.type = GetProperty;
				breakPoint.thisObject = thisObject;
				breakPoint.propertyInfo = propertyInfo;
				return breakPoint;
			}

			WfBreakPoint WfBreakPoint::Set(reflection::DescriptableObject* thisObject, reflection::description::IPropertyInfo* propertyInfo)
			{
				WfBreakPoint breakPoint;
				breakPoint.type = SetProperty;
				breakPoint.thisObject = thisObject;
				breakPoint.propertyInfo = propertyInfo;
				return breakPoint;
			}

			WfBreakPoint WfBreakPoint::Attach(reflection::DescriptableObject* thisObject, reflection::description::IEventInfo* eventInfo)
			{
				WfBreakPoint breakPoint;
				breakPoint.type = AttachEvent;
				breakPoint.thisObject = thisObject;
				breakPoint.eventInfo = eventInfo;
				return breakPoint;
			}

			WfBreakPoint WfBreakPoint::Detach(reflection::DescriptableObject* thisObject, reflection::description::IEventInfo* eventInfo)
			{
				WfBreakPoint breakPoint;
				breakPoint.type = DetachEvent;
				breakPoint.thisObject = thisObject;
				breakPoint.eventInfo = eventInfo;
				return breakPoint;
			}

			WfBreakPoint WfBreakPoint::Invoke(reflection::DescriptableObject* thisObject, reflection::description::IMethodInfo* methodInfo)
			{
				WfBreakPoint breakPoint;
				breakPoint.type = InvokeMethod;
				breakPoint.thisObject = thisObject;
				breakPoint.methodInfo = methodInfo;
				return breakPoint;
			}

			WfBreakPoint WfBreakPoint::Create(reflection::description::ITypeDescriptor* typeDescriptor)
			{
				WfBreakPoint breakPoint;
				breakPoint.type = CreateObject;
				breakPoint.typeDescriptor = typeDescriptor;
				return breakPoint;
			}

/***********************************************************************
InstructionLocation
***********************************************************************/

			bool WfDebugger::InstructionLocation::BreakStepOver(const InstructionLocation& il, bool beforeCodegen)
			{
				if (contextIndex != il.contextIndex) return contextIndex > il.contextIndex;
				if (assembly != il.assembly) return true;
				if (stackFrameIndex != il.stackFrameIndex) return stackFrameIndex > il.stackFrameIndex;

				auto debugInfo = (beforeCodegen ? assembly->insBeforeCodegen : assembly->insAfterCodegen);
				auto& range1 = debugInfo->instructionCodeMapping[instruction];
				auto& range2 = debugInfo->instructionCodeMapping[il.instruction];

				if (range1.codeIndex != range2.codeIndex) return true;
				if (range1.start.row != range2.start.row) return true;

				return false;
			}

			bool WfDebugger::InstructionLocation::BreakStepInto(const InstructionLocation& il, bool beforeCodegen)
			{
				if (contextIndex != il.contextIndex) return true;
				if (assembly != il.assembly) return true;
				if (stackFrameIndex != il.stackFrameIndex) return true;

				auto debugInfo = (beforeCodegen ? assembly->insBeforeCodegen : assembly->insAfterCodegen);
				auto& range1 = debugInfo->instructionCodeMapping[instruction];
				auto& range2 = debugInfo->instructionCodeMapping[il.instruction];

				if (range1.codeIndex != range2.codeIndex) return true;
				if (range1.start.row != range2.start.row) return true;

				return false;
			}

/***********************************************************************
IWfDebuggerCallback
***********************************************************************/

			void WfDebugger::OnBlockExecution()
			{
			}

			void WfDebugger::OnStartExecution()
			{
			}

			void WfDebugger::OnStopExecution()
			{
			}

			WfDebugger::InstructionLocation WfDebugger::MakeCurrentInstructionLocation()
			{
				auto context = threadContexts[threadContexts.Count() - 1];
				InstructionLocation il;
				il.contextIndex = threadContexts.Count() - 1;
				il.assembly = context->globalContext->assembly.Obj();
				il.stackFrameIndex = context->stackFrames.Count() - 1;
				il.instruction = context->stackFrames[context->stackFrames.Count() - 1].nextInstructionIndex;
				return il;
			}
				
			template<typename TKey>
			bool WfDebugger::HandleBreakPoint(const TKey& key, collections::Dictionary<TKey, vint>& breakPointMap)
			{
				if (evaluatingBreakPoint)
				{
					return false;
				}

				evaluatingBreakPoint = true;
				bool activated = false;
				vint index = breakPointMap.Keys().IndexOf(key);
				if (index != -1)
				{
					index = breakPointMap.Values()[index];
					const auto& breakPoint = breakPoints[index];
					if (breakPoint.available && breakPoint.enabled)
					{
						if (breakPoint.action)
						{
							activated = breakPoint.action->EvaluateCondition(this);
							breakPoint.action->PostAction(this);
						}
						else
						{
							activated = true;
						}
					}

					if (activated)
					{
						lastActivatedBreakPoint = index;
					}
				}
				evaluatingBreakPoint = false;
				return activated;
			}

			void WfDebugger::EnterThreadContext(WfRuntimeThreadContext* context)
			{
				if (threadContexts.Count() == 0)
				{
					lastActivatedBreakPoint = InvalidBreakPoint;
					instructionLocation = InstructionLocation();
					OnStartExecution();
					if (state == Stopped)
					{
						state = Running;
					}
				}
				threadContexts.Add(context);
			}

			void WfDebugger::LeaveThreadContext(WfRuntimeThreadContext* context)
			{
				auto oldContext = threadContexts[threadContexts.Count() - 1];
				threadContexts.RemoveAt(threadContexts.Count() - 1);
				CHECK_ERROR(context == oldContext, L"vl::workflow::runtime::WfDebugger::LeaveThreadContext(WfRuntimeThreadContext*)#EnterThreadContext and LeaveThreadContext should be called in pairs.");

				if (threadContexts.Count() == 0)
				{
					state = Stopped;
					OnStopExecution();
				}
			}

			bool WfDebugger::BreakIns(WfAssembly* assembly, vint instruction)
			{
				if (runningType != RunUntilBreakPoint)
				{
					auto il = MakeCurrentInstructionLocation();
					bool needToBreak = false;
					switch (runningType)
					{
					case RunStepOver:
						needToBreak = instructionLocation.BreakStepOver(il, stepBeforeCodegen);
						break;
					case RunStepInto:
						needToBreak = instructionLocation.BreakStepInto(il, stepBeforeCodegen);
						break;
					default:;
					}
					if (needToBreak)
					{
						instructionLocation = il;
						lastActivatedBreakPoint = WfDebugger::PauseBreakPoint;
						return true;
					}
				}

				switch (state)
				{
				case RequiredToPause:
				case RequiredToStop:
					lastActivatedBreakPoint = WfDebugger::PauseBreakPoint;
					return true;
				default:;
				}

				AssemblyKey key(assembly, instruction);
				return HandleBreakPoint(key, insBreakPoints);
			}

			bool WfDebugger::BreakRead(WfAssembly* assembly, vint variable)
			{
				AssemblyKey key(assembly, variable);
				return HandleBreakPoint(key, getGlobalVarBreakPoints);
			}

			bool WfDebugger::BreakWrite(WfAssembly* assembly, vint variable)
			{
				AssemblyKey key(assembly, variable);
				return HandleBreakPoint(key, setGlobalVarBreakPoints);
			}

			bool WfDebugger::BreakGet(reflection::DescriptableObject* thisObject, reflection::description::IPropertyInfo* propertyInfo)
			{
				PropertyKey key1(thisObject, propertyInfo);
				PropertyKey key2(nullptr, propertyInfo);
				return HandleBreakPoint(key1, getPropertyBreakPoints) || HandleBreakPoint(key2, getPropertyBreakPoints);
			}

			bool WfDebugger::BreakSet(reflection::DescriptableObject* thisObject, reflection::description::IPropertyInfo* propertyInfo)
			{
				PropertyKey key1(thisObject, propertyInfo);
				PropertyKey key2(nullptr, propertyInfo);
				return HandleBreakPoint(key1, setPropertyBreakPoints) || HandleBreakPoint(key2, setPropertyBreakPoints);
			}

			bool WfDebugger::BreakAttach(reflection::DescriptableObject* thisObject, reflection::description::IEventInfo* eventInfo)
			{
				EventKey key1(thisObject, eventInfo);
				EventKey key2(nullptr, eventInfo);
				return HandleBreakPoint(key1, attachEventBreakPoints) || HandleBreakPoint(key2, attachEventBreakPoints);
			}

			bool WfDebugger::BreakDetach(reflection::DescriptableObject* thisObject, reflection::description::IEventInfo* eventInfo)
			{
				EventKey key1(thisObject, eventInfo);
				EventKey key2(nullptr, eventInfo);
				return HandleBreakPoint(key1, detachEventBreakPoints) || HandleBreakPoint(key2, detachEventBreakPoints);
			}

			bool WfDebugger::BreakInvoke(reflection::DescriptableObject* thisObject, reflection::description::IMethodInfo* methodInfo)
			{
				MethodKey key1(thisObject, methodInfo);
				MethodKey key2(nullptr, methodInfo);
				return HandleBreakPoint(key1, invokeMethodBreakPoints) || HandleBreakPoint(key2, invokeMethodBreakPoints);
			}

			bool WfDebugger::BreakCreate(reflection::description::ITypeDescriptor* typeDescriptor)
			{
				return HandleBreakPoint(typeDescriptor, createObjectBreakPoints);
			}

			bool WfDebugger::BreakException(Ptr<WfRuntimeExceptionInfo> info)
			{
				if (breakException)
				{
					lastActivatedBreakPoint = PauseBreakPoint;
					return true;
				}
				else
				{
					return false;
				}
			}

			bool WfDebugger::WaitForContinue()
			{
				if (state == RequiredToStop)
				{
					return false;
				}

				state = lastActivatedBreakPoint >= 0 ? PauseByBreakPoint : PauseByOperation;
				while (state == PauseByBreakPoint || state == PauseByOperation)
				{
					OnBlockExecution();
				}
				
				if (state == Continue)
				{
					state = Running;
				}
				return true;
			}

/***********************************************************************
WfDebugger
***********************************************************************/

#define TEST(AVAILABLE, KEY, MAP) if (AVAILABLE && available == MAP.Keys().Contains(KEY)) return false;
#define SET(KEY, MAP) if (available) MAP.Add(KEY, index); else MAP.Remove(KEY);
#define SETC(AVAILABLE, KEY, MAP) if (AVAILABLE) {if (available) MAP.Add(KEY, index); else MAP.Remove(KEY);}

			bool WfDebugger::SetBreakPoint(const WfBreakPoint& breakPoint, bool available, vint index)
			{
				switch (breakPoint.type)
				{
				case WfBreakPoint::Instruction:
					{
						AssemblyKey key(breakPoint.assembly, breakPoint.instruction);
						TEST(true, key, insBreakPoints);
						SET(key, insBreakPoints);
					}
					break;
				case WfBreakPoint::ReadGlobalVar:
					{
						AssemblyKey key(breakPoint.assembly, breakPoint.variable);
						TEST(true, key, getGlobalVarBreakPoints);
						SET(key, getGlobalVarBreakPoints);
					}
					break;
				case WfBreakPoint::WriteGlobalVar:
					{
						AssemblyKey key(breakPoint.assembly, breakPoint.instruction);
						TEST(true, key, setGlobalVarBreakPoints);
						SET(key, setGlobalVarBreakPoints);
					}
					break;
				case WfBreakPoint::GetProperty:
					{
						PropertyKey key1(breakPoint.thisObject, breakPoint.propertyInfo);
						MethodKey key2(breakPoint.thisObject, breakPoint.propertyInfo->GetGetter());
						TEST(true, key1, getPropertyBreakPoints);
						TEST(key2.f1, key2, invokeMethodBreakPoints);
						SET(key1, getPropertyBreakPoints);
						SETC(key2.f1, key2, invokeMethodBreakPoints);
					}
					break;
				case WfBreakPoint::SetProperty:
					{
						PropertyKey key1(breakPoint.thisObject, breakPoint.propertyInfo);
						MethodKey key2(breakPoint.thisObject, breakPoint.propertyInfo->GetSetter());
						TEST(true, key1, setPropertyBreakPoints);
						TEST(key2.f1, key2, invokeMethodBreakPoints);
						SET(key1, setPropertyBreakPoints);
						SETC(key2.f1, key2, invokeMethodBreakPoints);
					}
					break;
				case WfBreakPoint::AttachEvent:
					{
						EventKey key(breakPoint.thisObject, breakPoint.eventInfo);
						TEST(true, key, attachEventBreakPoints);
						SET(key, attachEventBreakPoints);
					}
					break;
				case WfBreakPoint::DetachEvent:
					{
						EventKey key(breakPoint.thisObject, breakPoint.eventInfo);
						TEST(true, key, detachEventBreakPoints);
						SET(key, detachEventBreakPoints);
					}
					break;
				case WfBreakPoint::InvokeMethod:
					{
						// get property, set property and new object are all compiled to invoke method
						// so here it is not noecessary to generate other keys
						MethodKey key(breakPoint.thisObject, breakPoint.methodInfo);
						TEST(true, key, invokeMethodBreakPoints);
						SET(key, invokeMethodBreakPoints);
					}
					break;
				case WfBreakPoint::CreateObject:
					{
						auto group = breakPoint.typeDescriptor->GetConstructorGroup();
						vint count = group ? group->GetMethodCount() : 0;

						TEST(true, breakPoint.typeDescriptor, createObjectBreakPoints);
						for (vint i = 0; i < count; i++)
						{
							MethodKey key(nullptr, group->GetMethod(i));
							TEST(true, key, invokeMethodBreakPoints);
						}
						SET(breakPoint.typeDescriptor, createObjectBreakPoints);
						for (vint i = 0; i < count; i++)
						{
							MethodKey key(nullptr, group->GetMethod(i));
							SET(key, invokeMethodBreakPoints);
						}
					}
					break;
				default:
					return false;
				}
				return true;
			}

#undef TEST
#undef SET
#undef SETC

			WfDebugger::WfDebugger()
			{
			}

			WfDebugger::~WfDebugger()
			{
			}

			vint WfDebugger::AddBreakPoint(const WfBreakPoint& breakPoint)
			{
				vint index = breakPoints.Count();
				if (freeBreakPointIndices.Count() > 0)
				{
					index = freeBreakPointIndices[freeBreakPointIndices.Count() - 1];
				}

				if (!SetBreakPoint(breakPoint, true, index))
				{
					return -1;
				}
				
				if (index == breakPoints.Count())
				{
					breakPoints.Add(breakPoint);
				}
				else
				{
					freeBreakPointIndices.RemoveAt(freeBreakPointIndices.Count() - 1);
					breakPoints[index] = breakPoint;
				}

				breakPoints[index].id = index;
				breakPoints[index].available = true;
				breakPoints[index].enabled = true;
				return index;
			}

			vint WfDebugger::AddCodeLineBreakPoint(WfAssembly* assembly, vint codeIndex, vint row, bool beforeCodegen)
			{
				auto& codeInsMap = (beforeCodegen ? assembly->insBeforeCodegen : assembly->insAfterCodegen)->codeInstructionMapping;
				Tuple<vint, vint> key(codeIndex, row);
				vint index = codeInsMap.Keys().IndexOf(key);
				if (index == -1)
				{
					return -1;
				}

				vint ins = codeInsMap.GetByIndex(index)[0];
				return AddBreakPoint(WfBreakPoint::Ins(assembly, ins));
			}

			vint WfDebugger::GetBreakPointCount()
			{
				return breakPoints.Count();
			}

			const WfBreakPoint& WfDebugger::GetBreakPoint(vint index)
			{
				return breakPoints[index];
			}

			bool WfDebugger::RemoveBreakPoint(vint index)
			{
				if (index < 0 || index >= breakPoints.Count())
				{
					return false;
				}

				auto& breakPoint = breakPoints[index];
				if (!breakPoint.available || !SetBreakPoint(breakPoint, false, -1))
				{
					return false;
				}

				breakPoint.available = false;
				freeBreakPointIndices.Add(index);
				return true;
			}

			bool WfDebugger::EnableBreakPoint(vint index, bool enabled)
			{
				if (0 <= index && index <= breakPoints.Count())
				{
					auto& breakPoint = breakPoints[index];
					if (breakPoint.available)
					{
						breakPoint.enabled = enabled;
						return true;
					}
				}
				return false;
			}

			bool WfDebugger::GetBreakException()
			{
				return breakException;
			}

			void WfDebugger::SetBreakException(bool value)
			{
				breakException = value;
			}

			bool WfDebugger::Run()
			{
				if (state != PauseByOperation && state != PauseByBreakPoint)
				{
					return false;
				}
				state = Continue;
				runningType = RunUntilBreakPoint;
				return true;
			}

			bool WfDebugger::Pause()
			{
				if (state != Running && state != Stopped)
				{
					return false;
				}
				state = RequiredToPause;
				return true;
			}

			bool WfDebugger::Stop()
			{
				if (state != PauseByOperation && state != PauseByBreakPoint && state != Running)
				{
					return false;
				}
				state = RequiredToStop;
				return true;
			}

			bool WfDebugger::StepOver(bool beforeCodegen)
			{
				if (state != PauseByOperation && state != PauseByBreakPoint && state != Stopped)
				{
					return false;
				}
				if (state != Stopped)
				{
					state = Continue;
					instructionLocation = MakeCurrentInstructionLocation();
				}
				runningType = RunStepOver;
				stepBeforeCodegen = beforeCodegen;
				return true;
			}

			bool WfDebugger::StepInto(bool beforeCodegen)
			{
				if (state != PauseByOperation && state != PauseByBreakPoint && state != Stopped)
				{
					return false;
				}
				if (state != Stopped)
				{
					state = Continue;
					instructionLocation = MakeCurrentInstructionLocation();
				}
				state = Continue;
				runningType = RunStepInto;
				stepBeforeCodegen = beforeCodegen;
				return true;
			}

			WfDebugger::State WfDebugger::GetState()
			{
				return state;
			}

			WfDebugger::RunningType WfDebugger::GetRunningType()
			{
				return runningType;
			}

			vint WfDebugger::GetLastActivatedBreakPoint()
			{
				return lastActivatedBreakPoint;
			}

			const WfDebugger::ThreadContextList& WfDebugger::GetThreadContexts()
			{
				return threadContexts;
			}

			WfRuntimeThreadContext* WfDebugger::GetCurrentThreadContext()
			{
				if (threadContexts.Count() == 0)
				{
					return nullptr;
				}
				return threadContexts[threadContexts.Count() - 1];
			}

			const parsing::ParsingTextRange& WfDebugger::GetCurrentPosition(bool beforeCodegen, WfRuntimeThreadContext* context, vint callStackIndex)
			{
				if (!context)
				{
					context = GetCurrentThreadContext();
				}
				if (callStackIndex == -1)
				{
					callStackIndex = context->stackFrames.Count() - 1;
				}

				auto& stackFrame = context->stackFrames[callStackIndex];
				auto ins = stackFrame.nextInstructionIndex;
				auto debugInfo = (beforeCodegen ? context->globalContext->assembly->insBeforeCodegen : context->globalContext->assembly->insAfterCodegen);
				return debugInfo->instructionCodeMapping[ins];
			}

			reflection::description::Value WfDebugger::GetValueByName(const WString& name, WfRuntimeThreadContext* context, vint callStackIndex)
			{
				if (!context)
				{
					context = GetCurrentThreadContext();
				}
				if (callStackIndex == -1)
				{
					callStackIndex = context->stackFrames.Count() - 1;
				}

				auto& stackFrame = context->stackFrames[callStackIndex];
				auto function = context->globalContext->assembly->functions[stackFrame.functionIndex];

				vint index = function->argumentNames.IndexOf(name);
				if (index != -1)
				{
					return context->stack[stackFrame.stackBase + index];
				}

				index = function->localVariableNames.IndexOf(name);
				if (index != -1)
				{
					return context->stack[stackFrame.stackBase + function->argumentNames.Count() + index];
				}

				index = function->capturedVariableNames.IndexOf(name);
				if (index != -1)
				{
					return stackFrame.capturedVariables->variables[index];
				}

				index = context->globalContext->assembly->variableNames.IndexOf(name);
				if (index != -1)
				{
					return context->globalContext->globalVariables->variables[index];
				}

				return Value();
			}

/***********************************************************************
Helper Functions
***********************************************************************/

			ThreadVariable<Ptr<WfDebugger>> threadDebugger;

			IWfDebuggerCallback* GetDebuggerCallback()
			{
				return GetDebuggerCallback(GetDebuggerForCurrentThread().Obj());
			}

			IWfDebuggerCallback* GetDebuggerCallback(WfDebugger* debugger)
			{
				return debugger;
			}

			Ptr<WfDebugger> GetDebuggerForCurrentThread()
			{
				return threadDebugger.HasData() ? threadDebugger.Get() : nullptr;
			}

			void SetDebugferForCurrentThread(Ptr<WfDebugger> debugger)
			{
				threadDebugger.Set(debugger);
			}
		}
	}
}