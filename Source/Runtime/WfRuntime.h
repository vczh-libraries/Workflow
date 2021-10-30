/***********************************************************************
Vczh Library++ 3.0
Developer: Zihan Chen(vczh)
Workflow::Runtime

Interfaces:
**********************************************************************/

#ifndef VCZH_WORKFLOW_RUNTIME_WFRUNTIME
#define VCZH_WORKFLOW_RUNTIME_WFRUNTIME

#include "WfRuntimeAssembly.h"
#include "WfRuntimeConstructions.h"
#include "WfRuntimeDebugger.h"

#ifdef VCZH_DESCRIPTABLEOBJECT_WITH_METADATA

namespace vl
{
	namespace workflow
	{
		namespace runtime
		{

/***********************************************************************
RuntimeEnvironment
***********************************************************************/

			/// <summary>Variable storage.</summary>
			class WfRuntimeVariableContext : public Object, public reflection::Description<WfRuntimeVariableContext>
			{
				typedef collections::Array<reflection::description::Value>		VariableArray;

			public:
				/// <summary>Values of variables in runtime.</summary>
				VariableArray					variables;
			};

			/// <summary>Global context for executing a Workflow program. After the context is prepared, use [M:vl.workflow.runtime.LoadFunction`1] to call any functions inside the assembly. Function "<b>&lt;initialize&gt;</b>" should be the first to execute.</summary>
			class WfRuntimeGlobalContext : public Object, public reflection::Description<WfRuntimeGlobalContext>
			{
			public:
				/// <summary>The loaded assembly.</summary>
				Ptr<WfAssembly>					assembly;
				/// <summary>Global variable storages.</summary>
				Ptr<WfRuntimeVariableContext>	globalVariables;
				
				/// <summary>Create a global context for executing a Workflow program.</summary>
				/// <param name="_assembly">The assembly.</param>
				WfRuntimeGlobalContext(Ptr<WfAssembly> _assembly);
				~WfRuntimeGlobalContext();
			};

			struct WfRuntimeStackFrame
			{
				Ptr<WfRuntimeVariableContext>	capturedVariables;
				vint							functionIndex = -1;
				vint							nextInstructionIndex = -1;
				vint							stackBase = 0;
				vint							fixedVariableCount = 0;
				vint							freeStackBase = 0;
			};

			struct WfRuntimeTrapFrame
			{
				vint							stackFrameIndex = -1;
				vint							instructionIndex = -1;
				vint							stackPatternCount = -1;
			};

/***********************************************************************
RuntimeException
***********************************************************************/

			/// <summary>Representing a call stack item.</summary>
			class WfRuntimeCallStackInfo : public Object, public virtual reflection::description::IValueCallStack
			{
				using IValueReadonlyDictionary = reflection::description::IValueReadonlyDictionary;
			protected:
				Ptr<IValueReadonlyDictionary>	cachedLocalVariables;
				Ptr<IValueReadonlyDictionary>	cachedLocalArguments;
				Ptr<IValueReadonlyDictionary>	cachedCapturedVariables;
				Ptr<IValueReadonlyDictionary>	cachedGlobalVariables;

				Ptr<IValueReadonlyDictionary>	GetVariables(collections::List<WString>& names, Ptr<WfRuntimeVariableContext> context, Ptr<IValueReadonlyDictionary>& cache);
			public:
				WfRuntimeCallStackInfo();
				WfRuntimeCallStackInfo(WfRuntimeThreadContext* context, const WfRuntimeStackFrame& stackFrame);
				~WfRuntimeCallStackInfo();

				/// <summary>The executing assembly.</summary>
				Ptr<WfAssembly>					assembly;
				/// <summary>Global variable values.</summary>
				Ptr<WfRuntimeVariableContext>	global;
				/// <summary>Captured variable values.</summary>
				Ptr<WfRuntimeVariableContext>	captured;
				/// <summary>Argument values.</summary>
				Ptr<WfRuntimeVariableContext>	arguments;
				/// <summary>Local variable values.</summary>
				Ptr<WfRuntimeVariableContext>	localVariables;
				/// <summary>The executing function.</summary>
				vint							functionIndex = -1;
				/// <summary>The executing instruction.</summary>
				vint							instruction = -1;

				Ptr<IValueReadonlyDictionary>	GetLocalVariables()override;
				Ptr<IValueReadonlyDictionary>	GetLocalArguments()override;
				Ptr<IValueReadonlyDictionary>	GetCapturedVariables()override;
				Ptr<IValueReadonlyDictionary>	GetGlobalVariables()override;

				/// <summary>Get the name of the executing function.</summary>
				/// <returns>The name of the execution function.</returns>
				WString							GetFunctionName()override;
				/// <summary>Get the source code of the executing module.</summary>
				/// <returns>The source code.</returns>
				WString							GetSourceCodeBeforeCodegen()override;
				/// <summary>Get the source code of the executing module from generated syntax trees from the final compiling pass.</summary>
				/// <returns>The source code.</returns>
				WString							GetSourceCodeAfterCodegen()override;
				/// <summary>Get the row number (starts at 0) of the source code of the executing module.</summary>
				/// <returns>The row number.</returns>
				vint							GetRowBeforeCodegen()override;
				/// <summary>Get the row number (starts at 0) of the source code of the executing module from generated syntax trees from the final compiling pass.</summary>
				/// <returns>The row number.</returns>
				vint							GetRowAfterCodegen()override;
			};
			
			/// <summary>Representing an raised exception.</summary>
			class WfRuntimeExceptionInfo : public Object, public virtual reflection::description::IValueException
			{
				typedef collections::List<Ptr<WfRuntimeCallStackInfo>>		CallStackList;
				using IValueReadonlyList = reflection::description::IValueReadonlyList;
			protected:
				Ptr<IValueReadonlyList>			cachedCallStack;

			public:
				/// <summary>Exception message.</summary>
				WString							message;
				/// <summary>Fatal error flag.</summary>
				bool							fatal = false;
				/// <summary>All call stack items.</summary>
				CallStackList					callStack;

				WfRuntimeExceptionInfo(const WString& _message, bool _fatal);
				~WfRuntimeExceptionInfo();
				
#pragma push_macro("GetMessage")
#if defined GetMessage
#undef GetMessage
#endif
				WString							GetMessage()override;
#pragma pop_macro("GetMessage")
				bool							GetFatal()override;
				Ptr<IValueReadonlyList>			GetCallStack()override;
			};
			
			/// <summary>Representing an raised exception object for upper level C++ code.</summary>
			class WfRuntimeException : public reflection::description::TypeDescriptorException
			{
			protected:
				Ptr<WfRuntimeExceptionInfo>		info;
				bool							fatal = false;
			public:
				WfRuntimeException(Ptr<WfRuntimeExceptionInfo> _info)
					:reflection::description::TypeDescriptorException(_info->message)
					, info(_info)
					, fatal(_info->fatal)
				{
				}

				WfRuntimeException(const WString& _message, bool _fatal)
					:reflection::description::TypeDescriptorException(_message)
					, fatal(_fatal)
				{
				}

				/// <summary>Get the detailed information.</summary>
				/// <returns>The detailed information.</returns>
				Ptr<WfRuntimeExceptionInfo> GetInfo()const
				{
					return info;
				}

				/// <summary>Get the fatal error flag.</summary>
				/// <returns>Returns true if this exception is a fatal error, which normally means state corruption in a Workflow runtime.</returns>
				bool IsFatal()const
				{
					return fatal;
				}
			};

/***********************************************************************
RuntimeThreadContext
***********************************************************************/

			enum class WfRuntimeExecutionStatus
			{
				Ready,
				Executing,
				RaisedException,
				Finished,
				FatalError,
			};

			enum class WfRuntimeExecutionAction
			{
				ExecuteInstruction,
				UnwrapStack,
				EnterStackFrame,
				ExitStackFrame,
				Nop,
			};

			enum class WfRuntimeThreadContextError
			{
				Success,
				WrongStackItemIndex,
				WrongVariableIndex,
				WrongFunctionIndex,
				WrongArgumentCount,
				WrongCapturedVariableCount,
				EmptyStackFrame,
				EmptyTrapFrame,
				EmptyStack,
				TrapFrameCorrupted,
				StackCorrupted,
			};

			/// <summary>A Workflow script call stack.</summary>
			/// <remarks>
			/// <p>
			/// This object could be obtained by <see cref="WfDebugger::GetCurrentThreadContext"/>.
			/// A thread could have multiple thread contexts,
			/// a full list could be obtained by <see cref="WfDebugger::GetThreadContexts"/>.
			/// </p>
			/// <p>
			/// You are not recommended to update the call stack using this object.
			/// </p>
			/// <p>
			/// In the current version,
			/// the debug information doesn't contain enough data,
			/// so that it could be difficult to read local variables in high-level function constructions,
			/// like lambda expression or coroutines.
			/// </p>
			/// </remakrs>
			class WfRuntimeThreadContext
			{
				typedef collections::List<reflection::description::Value>		VariableList;
				typedef collections::List<WfRuntimeStackFrame>					StackFrameList;
				typedef collections::List<WfRuntimeTrapFrame>					TrapFrameList;
			public:

				Ptr<WfRuntimeGlobalContext>		globalContext;
				Ptr<WfRuntimeExceptionInfo>		exceptionInfo;
				VariableList					stack;
				StackFrameList					stackFrames;
				TrapFrameList					trapFrames;
				WfRuntimeExecutionStatus		status = WfRuntimeExecutionStatus::Finished;

				WfRuntimeThreadContext(Ptr<WfRuntimeGlobalContext> _context);
				WfRuntimeThreadContext(Ptr<WfAssembly> _assembly);

				WfRuntimeStackFrame&			GetCurrentStackFrame();
				WfRuntimeThreadContextError		PushStackFrame(vint functionIndex, vint argumentCount, Ptr<WfRuntimeVariableContext> capturedVariables = 0);
				WfRuntimeThreadContextError		PopStackFrame();
				WfRuntimeTrapFrame&				GetCurrentTrapFrame();
				WfRuntimeThreadContextError		PushTrapFrame(vint instructionIndex);
				WfRuntimeThreadContextError		PopTrapFrame(vint saveStackPatternCount);
				WfRuntimeThreadContextError		PushRuntimeValue(const WfRuntimeValue& value);
				WfRuntimeThreadContextError		PushValue(const reflection::description::Value& value);
				WfRuntimeThreadContextError		PopValue(reflection::description::Value& value);
				WfRuntimeThreadContextError		RaiseException(const WString& exception, bool fatalError, bool skipDebugger = false);
				WfRuntimeThreadContextError		RaiseException(Ptr<WfRuntimeExceptionInfo> info, bool skipDebugger = false);

				WfRuntimeThreadContextError		LoadStackValue(vint stackItemIndex, reflection::description::Value& value);
				WfRuntimeThreadContextError		LoadGlobalVariable(vint variableIndex, reflection::description::Value& value);
				WfRuntimeThreadContextError		StoreGlobalVariable(vint variableIndex, const reflection::description::Value& value);
				WfRuntimeThreadContextError		LoadCapturedVariable(vint variableIndex, reflection::description::Value& value);
				WfRuntimeThreadContextError		StoreCapturedVariable(vint variableIndex, const reflection::description::Value& value);
				WfRuntimeThreadContextError		LoadLocalVariable(vint variableIndex, reflection::description::Value& value);
				WfRuntimeThreadContextError		StoreLocalVariable(vint variableIndex, const reflection::description::Value& value);

				WfRuntimeExecutionAction		ExecuteInternal(WfInstruction& ins, WfRuntimeStackFrame& stackFrame, IWfDebuggerCallback* callback);
				WfRuntimeExecutionAction		Execute(IWfDebuggerCallback* callback);
				void							ExecuteToEnd();
			};

/***********************************************************************
Helper Functions
***********************************************************************/
			
			/// <summary>Load a function from a global context, raise an exception if multiple functions are found under the same name. Function "&lt;initialize&gt;" should be the first to execute.</summary>
			/// <returns>The loaded function.</returns>
			/// <param name="context">The context to the evaluation environment.</param>
			/// <param name="name">The function name.</param>
			/// <remarks>"<b>&lt;initialize&gt;</b>" must be the first function that is executed after an assembly is loaded. It has no argument or return value.</remarks>
			extern Ptr<reflection::description::IValueFunctionProxy>		LoadFunction(Ptr<WfRuntimeGlobalContext> context, const WString& name);
			
			/// <summary>Load a C++ friendly function from a global context, raise an exception if multiple functions are found under the same name. Function "&lt;initialize&gt;" should be the first to execute.</summary>
			/// <typeparam name="TFunction">Type of the function.</typeparam>
			/// <returns>The loaded C++ friendly function.</returns>
			/// <param name="context">The context to the evaluation environment.</param>
			/// <param name="name">The function name.</param>
			/// <remarks>"<b>&lt;initialize&gt;</b>" must be the first function that is executed after an assembly is loaded. Its type is <b>void()</b>.</remarks>
			template<typename TFunction>
			Func<TFunction> LoadFunction(Ptr<WfRuntimeGlobalContext> context, const WString& name)
			{
				auto proxy = LoadFunction(context, name);
				return reflection::description::UnboxParameter<Func<TFunction>>(reflection::description::Value::From(proxy)).Ref();
			}
		}
	}
}

#endif
#endif
