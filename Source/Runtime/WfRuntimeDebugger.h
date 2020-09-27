/***********************************************************************
Vczh Library++ 3.0
Developer: Zihan Chen(vczh)
Workflow::Runtime

Interfaces:
**********************************************************************/

#ifndef VCZH_WORKFLOW_RUNTIME_WFRUNTIMEDEBUGGER
#define VCZH_WORKFLOW_RUNTIME_WFRUNTIMEDEBUGGER

#include "WfRuntimeAssembly.h"

namespace vl
{
	namespace workflow
	{
		namespace runtime
		{
			class WfRuntimeThreadContext;
			class WfRuntimeExceptionInfo;
			class IWfDebuggerCallback;
			class WfDebugger;

/***********************************************************************
Debugger
***********************************************************************/

			/// <summary>Break point action. It will </summary>
			class IWfBreakPointAction : public virtual Interface
			{
			public:
				/// <summary>Called a break point is about to activate.</summary>
				/// <returns>Returns false to skip this break point.</returns>
				/// <param name="debugger">The current attached debugger.</param>
				virtual bool					EvaluateCondition(WfDebugger* debugger) = 0;
				/// <summary>Called when a break point is about to activate, even <see cref="EvaluateCondition"/> returns false.</summary>
				/// <param name="debugger">The current attached debugger.</param>
				/// <param name="debugger">The return value from <see cref="EvaluateCondition"/>.</param>
				virtual void					PostAction(WfDebugger* debugger, bool activated) = 0;
			};

			/// <summary>Break point.</summary>
			struct WfBreakPoint
			{
				enum Type
				{
					Instruction,	// assembly, instruction
					ReadGlobalVar,	// assembly, variable
					WriteGlobalVar,	// assembly, variable
					GetProperty,	// [thisObject], propertyInfo
					SetProperty,	// [thisObject], propertyInfo
					AttachEvent,	// [thisObject], eventInfo
					DetachEvent,	// [thisObject], eventInfo
					InvokeEvent,	// [thisObject], eventInfo
					InvokeMethod,	// [thisObject], methodInfo
					CreateObject,	// typeDescriptor
				};

				vint											id = -1;
				bool											available = false;
				bool											enabled = false;
				Ptr<IWfBreakPointAction>						action;

				Type											type;
				WfAssembly*										assembly = nullptr;
				union
				{
					vint										instruction = -1;
					vint										variable;
				};

				reflection::DescriptableObject*					thisObject = nullptr;
				union
				{
					reflection::description::IPropertyInfo*		propertyInfo = nullptr;
					reflection::description::IEventInfo*		eventInfo;
					reflection::description::IMethodInfo*		methodInfo;
					reflection::description::ITypeDescriptor*	typeDescriptor;
				};

				/// <summary>Create an instruction break point.</summary>
				/// <returns>The created break point.</returns>
				/// <param name="assembly">The assembly that contains the instruction.</param>
				/// <param name="instruction">The index of the instruction.</param>
				static WfBreakPoint								Ins(WfAssembly* assembly, vint instruction);
				
				/// <summary>Create an global variable reading break point.</summary>
				/// <returns>The created break point.</returns>
				/// <param name="assembly">The assembly that contains the instruction.</param>
				/// <param name="variable">The index of the global variable.</param>
				static WfBreakPoint								Read(WfAssembly* assembly, vint variable);
				
				/// <summary>Create an global variable writing break point.</summary>
				/// <returns>The created break point.</returns>
				/// <param name="assembly">The assembly that contains the instruction.</param>
				/// <param name="variable">The index of the global variable.</param>
				static WfBreakPoint								Write(WfAssembly* assembly, vint variable);
				
				/// <summary>Create an property reading break point.</summary>
				/// <returns>The created break point.</returns>
				/// <param name="thisObject">The target object. Set to null to apply to every object.</param>
				/// <param name="propertyInfo">The property.</param>
				static WfBreakPoint								Get(reflection::DescriptableObject* thisObject, reflection::description::IPropertyInfo* propertyInfo);
				
				/// <summary>Create an property writing break point.</summary>
				/// <returns>The created break point.</returns>
				/// <param name="thisObject">The target object. Set to null to apply to every object.</param>
				/// <param name="propertyInfo">The property.</param>
				static WfBreakPoint								Set(reflection::DescriptableObject* thisObject, reflection::description::IPropertyInfo* propertyInfo);
				
				/// <summary>Create an event attaching break point.</summary>
				/// <returns>The created break point.</returns>
				/// <param name="thisObject">The target object. Set to null to apply to every object.</param>
				/// <param name="eventInfo">The event.</param>
				static WfBreakPoint								Attach(reflection::DescriptableObject* thisObject, reflection::description::IEventInfo* eventInfo);
				
				/// <summary>Create an event detaching break point.</summary>
				/// <returns>The created break point.</returns>
				/// <param name="thisObject">The target object. Set to null to apply to every object.</param>
				/// <param name="eventInfo">The event.</param>
				static WfBreakPoint								Detach(reflection::DescriptableObject* thisObject, reflection::description::IEventInfo* eventInfo);
				
				/// <summary>Create an event invoking break point.</summary>
				/// <returns>The created break point.</returns>
				/// <param name="thisObject">The target object. Set to null to apply to every object.</param>
				/// <param name="eventInfo">The event.</param>
				static WfBreakPoint								Invoke(reflection::DescriptableObject* thisObject, reflection::description::IEventInfo* eventInfo);
				
				/// <summary>Create an function invoking break point.</summary>
				/// <returns>The created break point.</returns>
				/// <param name="thisObject">The target object. Set to null to apply to every object.</param>
				/// <param name="methodInfo">The function.</param>
				static WfBreakPoint								Invoke(reflection::DescriptableObject* thisObject, reflection::description::IMethodInfo* methodInfo);
				
				/// <summary>Create an object creating break point.</summary>
				/// <returns>The created break point.</returns>
				/// <param name="typeDescriptor">The target object type.</param>
				static WfBreakPoint								Create(reflection::description::ITypeDescriptor* typeDescriptor);
			};

			class IWfDebuggerCallback : public virtual Interface
			{
			public:
				virtual void					EnterThreadContext(WfRuntimeThreadContext* context) = 0;
				virtual void					LeaveThreadContext(WfRuntimeThreadContext* context) = 0;
				virtual bool					BreakIns(WfAssembly* assembly, vint instruction) = 0;
				virtual bool					BreakRead(WfAssembly* assembly, vint variable) = 0;
				virtual bool					BreakWrite(WfAssembly* assembly, vint variable) = 0;
				virtual bool					BreakGet(reflection::DescriptableObject* thisObject, reflection::description::IPropertyInfo* propertyInfo) = 0;
				virtual bool					BreakSet(reflection::DescriptableObject* thisObject, reflection::description::IPropertyInfo* propertyInfo) = 0;
				virtual bool					BreakAttach(reflection::DescriptableObject* thisObject, reflection::description::IEventInfo* eventInfo) = 0;
				virtual bool					BreakDetach(reflection::DescriptableObject* thisObject, reflection::description::IEventInfo* eventInfo) = 0;
				virtual bool					BreakInvoke(reflection::DescriptableObject* thisObject, reflection::description::IEventInfo* eventInfo) = 0;
				virtual bool					BreakInvoke(reflection::DescriptableObject* thisObject, reflection::description::IMethodInfo* methodInfo) = 0;
				virtual bool					BreakCreate(reflection::description::ITypeDescriptor* typeDescriptor) = 0;
				virtual bool					BreakException(Ptr<WfRuntimeExceptionInfo> info) = 0;
				virtual bool					WaitForContinue() = 0;
			};

			/// <summary>Workflow debugger.</summary>
			class WfDebugger : public Object, protected virtual IWfDebuggerCallback
			{
				friend IWfDebuggerCallback* GetDebuggerCallback(WfDebugger* debugger);

				typedef collections::List<WfBreakPoint>					BreakPointList;
				typedef collections::List<WfRuntimeThreadContext*>		ThreadContextList;

				typedef Tuple<WfAssembly*, vint>														AssemblyKey;
				typedef Tuple<reflection::DescriptableObject*, reflection::description::IPropertyInfo*>	PropertyKey;
				typedef Tuple<reflection::DescriptableObject*, reflection::description::IEventInfo*>	EventKey;
				typedef Tuple<reflection::DescriptableObject*, reflection::description::IMethodInfo*>	MethodKey;
				typedef reflection::description::ITypeDescriptor*										TypeKey;

				typedef collections::Dictionary<AssemblyKey, vint>		AssemblyBreakPointMap;
				typedef collections::Dictionary<PropertyKey, vint>		PropertyBreakPointMap;
				typedef collections::Dictionary<EventKey, vint>			EventBreakPointMap;
				typedef collections::Dictionary<MethodKey, vint>		MethodBreakPointMap;
				typedef collections::Dictionary<TypeKey, vint>			TypeBreakPointMap;
			public:
				/// <summary>The state of the debugger.</summary>
				/// <remarks>
				/// <p>
				/// The state is affected by break points and the following operations:
				/// <ul>
				///   <li><b><see cref="Run"/></b>: If the Workflow script is paused, it continues the script.</li>
				///   <li><b><see cref="Pause"/></b>: If the Workflow script is running, it pauses the script, following by a call to <see cref="OnBlockExecution"/>.</li>
				///   <li><b><see cref="Stop"/></b>: If the Workflow script is not stopped, it stops the script by throwing in exception in the script.</li>
				///   <li><b><see cref="StepOver"/></b>: Stop over to the next code line and pause. It doesn't jump into the function to be called.</li>
				///   <li><b><see cref="StepInto"/></b>: Stop into the new code line and pause.</li>
				/// </ul>
				/// Operations are expected to be called in <see cref="OnBlockExecution"/>.
				/// </p>
				/// </remarks>
				enum State
				{						//		Run		Pause	Stop	StepOver	StepInto
					/// <summary>The associated thread is running Workflow script.</summary>
					Running,			// R			*RTP	*RTS
					/// <summary>The target Workflow script is paused by operations other than break points.</summary>
					PauseByOperation,	// PBO	*C				*RTS	*C			*C
					/// <summary>The target Workflow script is paused by break points.</summary>
					PauseByBreakPoint,	// PBB	*C				*RTS	*C			*C
					/// <summary>The associated thread has stopped running Workflow script.</summary>
					Stopped,			// S			*RTP			*			*
					/// <summary>The debugger allows the target Workflow script to continue.</summary>
					Continue,			// C	soon becomes Running
					/// <summary>The target Workflow script is required to pause. This value can be observed in <see cref="OnBlockExecution"/>. Operations to continue executing the Workflow script await to be called.</summary>
					RequiredToPause,	// RTP	soon becomes PauseByOperation (should be triggered in OnBlockExecution)
					/// <summary>The target Workflow script is required to stop. It caused to Workflow script to stop by throwing an exception saying this.</summary>
					RequiredToStop,		// RTS	soon becomes Stopped          (should be triggered in OnBlockExecution)
				};

				enum RunningType
				{
					RunUntilBreakPoint,
					RunStepOver,
					RunStepInto,
				};

				struct InstructionLocation
				{
					vint								contextIndex = -1;
					WfAssembly*							assembly = nullptr;
					vint								stackFrameIndex = -1;
					vint								instruction = -1;

					bool								BreakStepOver(const InstructionLocation& il, bool beforeCodegen);
					bool								BreakStepInto(const InstructionLocation& il, bool beforeCodegen);
				};

				static const vint						InvalidBreakPoint = -1;
				static const vint						PauseBreakPoint = -2;
			protected:
				BreakPointList							breakPoints;
				collections::List<vint>					freeBreakPointIndices;
				volatile bool							evaluatingBreakPoint = false;
				volatile bool							breakException = false;

				ThreadContextList						threadContexts;

				volatile State							state = Stopped;
				volatile RunningType					runningType = RunUntilBreakPoint;
				volatile vint							lastActivatedBreakPoint = InvalidBreakPoint;
				bool									stepBeforeCodegen = true;
				InstructionLocation						instructionLocation;

				AssemblyBreakPointMap					insBreakPoints;
				AssemblyBreakPointMap					getGlobalVarBreakPoints;
				AssemblyBreakPointMap					setGlobalVarBreakPoints;
				PropertyBreakPointMap					getPropertyBreakPoints;
				PropertyBreakPointMap					setPropertyBreakPoints;
				EventBreakPointMap						attachEventBreakPoints;
				EventBreakPointMap						detachEventBreakPoints;
				EventBreakPointMap						invokeEventBreakPoints;
				MethodBreakPointMap						invokeMethodBreakPoints;
				TypeBreakPointMap						createObjectBreakPoints;

				/// <summary>Called for doing something when a break point is activated. This function will be called multiple times before some one let the debugger to continue.</summary>
				/// <remarks>This function must be overrided, or the Workflow script will hang when it is paused by any reason.</remarks>
				virtual void							OnBlockExecution();
				/// <summary>Called when a new Workflow program is about to run.</summary>
				virtual void							OnStartExecution();
				/// <summary>Called when a Workflow program is stopped by any reason.</summary>
				virtual void							OnStopExecution();
				
				InstructionLocation						MakeCurrentInstructionLocation();
				template<typename TKey>
				bool									HandleBreakPoint(const TKey& key, collections::Dictionary<TKey, vint>& breakPointMap);
				bool									SetBreakPoint(const WfBreakPoint& breakPoint, bool available, vint index);
				
				void									EnterThreadContext(WfRuntimeThreadContext* context)override;
				void									LeaveThreadContext(WfRuntimeThreadContext* context)override;
				bool									BreakIns(WfAssembly* assembly, vint instruction)override;
				bool									BreakRead(WfAssembly* assembly, vint variable)override;
				bool									BreakWrite(WfAssembly* assembly, vint variable)override;
				bool									BreakGet(reflection::DescriptableObject* thisObject, reflection::description::IPropertyInfo* propertyInfo)override;
				bool									BreakSet(reflection::DescriptableObject* thisObject, reflection::description::IPropertyInfo* propertyInfo)override;
				bool									BreakAttach(reflection::DescriptableObject* thisObject, reflection::description::IEventInfo* eventInfo)override;
				bool									BreakDetach(reflection::DescriptableObject* thisObject, reflection::description::IEventInfo* eventInfo)override;
				bool									BreakInvoke(reflection::DescriptableObject* thisObject, reflection::description::IEventInfo* eventInfo)override;
				bool									BreakInvoke(reflection::DescriptableObject* thisObject, reflection::description::IMethodInfo* methodInfo)override;
				bool									BreakCreate(reflection::description::ITypeDescriptor* typeDescriptor)override;
				bool									BreakException(Ptr<WfRuntimeExceptionInfo> info)override;
				bool									WaitForContinue()override;
			public:
				/// <summary>Create a debugger.</summary>
				WfDebugger();
				~WfDebugger();

				/// <summary>Add a new break point.</summary>
				/// <returns>Returns the index of this break point. Returns -1 if failed.</returns>
				/// <param name="breakPoint">The break point.</param>
				vint									AddBreakPoint(const WfBreakPoint& breakPoint);
				/// <summary>Add a new source code break point.</summary>
				/// <returns>Returns the index of this break point. Returns -1 if failed.</returns>
				/// <param name="assembly">The assembly.</param>
				/// <param name="codeIndex">The code index of a module.</param>
				/// <param name="row">The row number, starts from 0.</param>
				/// <param name="beforeCodegen">Set to true to apply source code information to original source code.</param>
				vint									AddCodeLineBreakPoint(WfAssembly* assembly, vint codeIndex, vint row, bool beforeCodegen = true);
				/// <summary>Get the number of all break points.</summary>
				/// <returns>The number of all break points.</returns>
				vint									GetBreakPointCount();
				/// <summary>Test if an index is an available break point.</summary>
				/// <returns>Returns true if an index is an available break point. This function returns true for all disabled break points.</returns>
				bool									IsBreakPointAvailable(vint index);
				/// <summary>Get a specified break point. For unavailable break points, <see cref="WfBreakPoint::available"/> is false.</summary>
				/// <returns>The break point.</returns>
				/// <param name="index">The index of the break point.</param>
				const WfBreakPoint&						GetBreakPoint(vint index);
				/// <summary>Delete a specified break point.</summary>
				/// <returns>Returns true if this operation is succeeded.</returns>
				/// <param name="index">The index of the break point.</param>
				/// <remarks>After removing a break point, the break point becomes unavailable. The index will be reused later when a new break point is added to the debugger.</remarks>
				bool									RemoveBreakPoint(vint index);
				/// <summary>Enable or disable a specified break point.</summary>
				/// <returns>Returns true if this operation is succeeded.</returns>
				/// <param name="index">The index of the break point.</param>
				/// <param name="enabled">Set to true to enable a break point.</param>
				bool									EnableBreakPoint(vint index, bool enabled);
				/// <summary>Test if the exception break point is enabled or not.</summary>
				/// <returns>Returns true if the exception break point is enabled.</returns>
				bool									GetBreakException();
				/// <summary>Enable or disable the exception break point.</summary>
				/// <param name="value">Set to true to enable the exception break point.</param>
				void									SetBreakException(bool value);

				/// <summary>Continue to run the Workflow program.</summary>
				/// <returns>Returns true if this operation is succeeded.</returns>
				bool									Run();
				/// <summary>Pause the Workflow program.</summary>
				/// <returns>Returns true if this operation is succeeded.</returns>
				bool									Pause();
				/// <summary>Stop the Workflow program.</summary>
				/// <returns>Returns true if this operation is succeeded.</returns>
				bool									Stop();
				/// <summary>Run until reached the next row in the same function or outside of this function.</summary>
				/// <returns>Returns true if this operation is succeeded.</returns>
				/// <param name="beforeCodegen">Set to true to apply the source code information to original source code.</param>
				bool									StepOver(bool beforeCodegen = true);
				/// <summary>Run until reached the next row.</summary>
				/// <returns>Returns true if this operation is succeeded.</returns>
				/// <param name="beforeCodegen">Set to true to apply the source code information to original source code.</param>
				bool									StepInto(bool beforeCodegen = true);
				/// <summary>Get the current state of the debugger.</summary>
				/// <returns>The state of the debugger.</returns>
				State									GetState();
				/// <summary>Get the running type of the debugger.</summary>
				/// <returns>The running type of the debugger.</returns>
				RunningType								GetRunningType();
				/// <summary>Get the index of the last activated break point.</summary>
				/// <returns>The index of the last activated break point.</returns>
				vint									GetLastActivatedBreakPoint();

				const ThreadContextList&				GetThreadContexts();
				WfRuntimeThreadContext*					GetCurrentThreadContext();
				/// <summary>Get the current position of the executing instruction in the source code.</summary>
				/// <returns>The current position in the source code.</returns>
				/// <param name="beforeCodegen">Set to true to apply the source code information to original source code.</param>
				/// <param name="context">Specify a thread context. Set to null to use the top thread context.</param>
				/// <param name="callStackIndex">Specify a call stack index. Set to null to use the top call stack item of the specified thread context.</param>
				const parsing::ParsingTextRange&		GetCurrentPosition(bool beforeCodegen = true, WfRuntimeThreadContext* context = nullptr, vint callStackIndex = -1);
				/// <summary>Get the variable value by name in the current scope.</summary>
				/// <returns>The value.</returns>
				/// <param name="name">The name.</param>
				/// <param name="context">Specify a thread context. Set to null to use the top thread context.</param>
				/// <param name="callStackIndex">Specify a call stack index. Set to null to use the top call stack item of the specified thread context.</param>
				reflection::description::Value			GetValueByName(const WString& name, WfRuntimeThreadContext* context = nullptr, vint callStackIndex = -1);
			};

			extern IWfDebuggerCallback*					GetDebuggerCallback();
			extern IWfDebuggerCallback*					GetDebuggerCallback(WfDebugger* debugger);
			
			/// <summary>Get the debugger for the current thread.</summary>
			/// <returns>The debugger.</returns>
			extern Ptr<WfDebugger>						GetDebuggerForCurrentThread();
			/// <summary>Set the debugger for the current thread.</summary>
			/// <param name="debugger">The debugger.</param>
			extern void									SetDebuggerForCurrentThread(Ptr<WfDebugger> debugger);
		}
	}
}

#endif
