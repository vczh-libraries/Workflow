/***********************************************************************
Vczh Library++ 3.0
Developer: Zihan Chen(vczh)
Workflow::Runtime

Interfaces:
**********************************************************************/

#ifndef VCZH_WORKFLOW_RUNTIME_WFRUNTIME
#define VCZH_WORKFLOW_RUNTIME_WFRUNTIME

#include "../WorkflowVlppReferences.h"

namespace vl
{
	namespace workflow
	{
		namespace runtime
		{
			struct WfRuntimeThreadContext;
			class IWfDebuggerCallback;
			class WfDebugger;

/***********************************************************************
Instruction
***********************************************************************/

			enum class WfInsCode
			{
				// Instruction		// param				: <Stack-Pattern> -> <Stack-Pattern> in the order of <bottom ---- top>
				Nop,				// 						: () -> ()										;
				LoadValue,			// value				: () -> Value									;
				LoadClosure,		// function, count		: Value-1, ..., Value-count -> Value			;
				LoadException,		// 						: () -> Value									;
				LoadLocalVar,		// variable				: () -> Value									;
				LoadCapturedVar,	// variable				: () -> Value									;
				LoadGlobalVar,		// variable				: () -> Value									;
				StoreLocalVar,		// variable				: Value -> ()									;
				StoreGlobalVar,		// variable				: Value -> ()									;
				Duplicate,			// count				: () -> Value									; copy stack[stack.Count()-1-count]
				Pop,				//						: Value -> ()									;
				Return,				// 						: Value -> Value								; (exit function)
				CreateArray,		// count				: Value-count, ..., Value-1 -> <array>			; {1 2 3} -> <3 2 1>
				CreateMap,			// count				: Value-count, ..., Value-1 -> <map>			; {1:2 3:4} -> <3 4 1 2>
				CreateInterface,	// count				: Value-count, ..., Value-1 -> <map>			; {"Get":a "Set":b} -> InterfaceProxy^
				CreateRange,		// I1248/U1248			: Value-begin, Value-end -> <enumerable>		;
				ReverseEnumerable,	//						: Value -> Value								;
				DeleteRawPtr,		//						: Value -> ()									;
				ConvertToType,		// flag, typeDescriptor	: Value -> Value								;
				TryConvertToType,	// flag, typeDescriptor	: Value -> Value								;
				TestType,			// flag, typeDescriptor	: Value -> <bool>								;
				GetType,			//						: Value -> <ITypeDescriptor*>					;
				Jump,				// label				: () -> ()										;
				JumpIf,				// label				: () -> ()										;
				Invoke,				// function, count		: Value-1, ..., Value-n -> Value				;
				GetProperty,		// IPropertyInfo*		: Value-this -> Value							;
				InvokeProxy,		// count				: Value-1, ..., Value-n, Value-this -> Value	;
				InvokeMethod,		// IMethodInfo*, count	: Value-1, ..., Value-n, Value-this -> Value	;
				AttachEvent,		// IEventInfo*			: Value-this, <function> -> <Listener>			;
				DetachEvent,		// 						: <Listener> -> bool							;
				InstallTry,			// label				: () -> ()										;
				UninstallTry,		// count				: () -> ()										;
				RaiseException,		// 						: Value -> ()									; (trap)
				TestElementInSet,	//						: Value-element, Value-set -> bool				;
				CompareLiteral,		// I48/U48/F48/S		: Value, Value -> <int>							;
				CompareStruct,		// 						: Value, Value -> <bool>						;
				CompareReference,	// 						: Value, Value -> <bool>						;
				CompareValue,		// 						: Value, Value -> <bool>						;
				OpNot,				// B/I1248/U1248		: Value -> Value								;
				OpPositive,			// I1248/U1248			: Value -> Value								;
				OpNegative,			// I1248				: Value -> Value								;
				OpConcat,			// 						: <string>, <string> -> <string>				;
				OpExp,				// I48/U48/F48			: Value, Value -> Value							;
				OpAdd,				// I48/U48/F48			: Value, Value -> Value							;
				OpSub,				// I48/U48/F48			: Value, Value -> Value							;
				OpMul,				// I48/U48/F48			: Value, Value -> Value							;
				OpDiv,				// I48/U48/F48			: Value, Value -> Value							;
				OpMod,				// I48/U48				: Value, Value -> Value							;
				OpShl,				// I48/U48				: Value, Value -> Value							;
				OpShr,				// I48/U48				: Value, Value -> Value							;
				OpXor,				// B/I1248/U1248		: <bool>, <bool> -> <bool>						;
				OpAnd,				// B/I1248/U1248		: <bool>, <bool> -> <bool>						;
				OpOr,				// B/I1248/U1248		: <bool>, <bool> -> <bool>						;
				OpLT,				// 						: <int> -> <bool>								;
				OpGT,				// 						: <int> -> <bool>								;
				OpLE,				// 						: <int> -> <bool>								;
				OpGE,				// 						: <int> -> <bool>								;
				OpEQ,				// 						: <int> -> <bool>								;
				OpNE,				// 						: <int> -> <bool>								;
			};

#define INSTRUCTION_CASES(APPLY, APPLY_VALUE, APPLY_FUNCTION, APPLY_FUNCTION_COUNT, APPLY_VARIABLE, APPLY_COUNT, APPLY_FLAG_TYPEDESCRIPTOR, APPLY_PROPERTY, APPLY_METHOD_COUNT, APPLY_EVENT, APPLY_LABEL, APPLY_TYPE)\
			APPLY(Nop)\
			APPLY_VALUE(LoadValue)\
			APPLY_FUNCTION_COUNT(LoadClosure)\
			APPLY(LoadException)\
			APPLY_VARIABLE(LoadLocalVar)\
			APPLY_VARIABLE(LoadCapturedVar)\
			APPLY_VARIABLE(LoadGlobalVar)\
			APPLY_VARIABLE(StoreLocalVar)\
			APPLY_VARIABLE(StoreGlobalVar)\
			APPLY_COUNT(Duplicate)\
			APPLY(Pop)\
			APPLY(Return)\
			APPLY_COUNT(CreateArray)\
			APPLY_COUNT(CreateMap)\
			APPLY_COUNT(CreateInterface)\
			APPLY_TYPE(CreateRange)\
			APPLY(ReverseEnumerable)\
			APPLY(DeleteRawPtr)\
			APPLY_FLAG_TYPEDESCRIPTOR(ConvertToType)\
			APPLY_FLAG_TYPEDESCRIPTOR(TryConvertToType)\
			APPLY_FLAG_TYPEDESCRIPTOR(TestType)\
			APPLY(GetType)\
			APPLY_LABEL(Jump)\
			APPLY_LABEL(JumpIf)\
			APPLY_FUNCTION_COUNT(Invoke)\
			APPLY_PROPERTY(GetProperty)\
			APPLY_COUNT(InvokeProxy)\
			APPLY_METHOD_COUNT(InvokeMethod)\
			APPLY_EVENT(AttachEvent)\
			APPLY(DetachEvent)\
			APPLY_LABEL(InstallTry)\
			APPLY_COUNT(UninstallTry)\
			APPLY(RaiseException)\
			APPLY(TestElementInSet)\
			APPLY_TYPE(CompareLiteral)\
			APPLY(CompareStruct)\
			APPLY(CompareReference)\
			APPLY(CompareValue)\
			APPLY_TYPE(OpNot)\
			APPLY_TYPE(OpPositive)\
			APPLY_TYPE(OpNegative)\
			APPLY(OpConcat)\
			APPLY_TYPE(OpExp)\
			APPLY_TYPE(OpAdd)\
			APPLY_TYPE(OpSub)\
			APPLY_TYPE(OpMul)\
			APPLY_TYPE(OpDiv)\
			APPLY_TYPE(OpMod)\
			APPLY_TYPE(OpShl)\
			APPLY_TYPE(OpShr)\
			APPLY_TYPE(OpXor)\
			APPLY_TYPE(OpAnd)\
			APPLY_TYPE(OpOr)\
			APPLY(OpLT)\
			APPLY(OpGT)\
			APPLY(OpLE)\
			APPLY(OpGE)\
			APPLY(OpEQ)\
			APPLY(OpNE)\

			enum class WfInsType
			{
				Bool,
				I1,
				I2,
				I4,
				I8,
				U1,
				U2,
				U4,
				U8,
				F4,
				F8,
				String,
				Unknown,
			};

			struct WfInstruction
			{
				WfInsCode											code = WfInsCode::Nop;
				reflection::description::Value						valueParameter;
				vint												countParameter = 0;
				union
				{
					struct
					{
						reflection::description::Value::ValueType		flagParameter;
						reflection::description::ITypeDescriptor*		typeDescriptorParameter;
					};
					WfInsType											typeParameter;
					vint												indexParameter;
					reflection::description::IPropertyInfo*				propertyParameter;
					reflection::description::IMethodInfo*				methodParameter;
					reflection::description::IEventInfo*				eventParameter;
				};

				WfInstruction();

				#define CTOR(NAME)						static WfInstruction NAME();
				#define CTOR_VALUE(NAME)				static WfInstruction NAME(const reflection::description::Value& value);
				#define CTOR_FUNCTION(NAME)				static WfInstruction NAME(vint function);
				#define CTOR_FUNCTION_COUNT(NAME)		static WfInstruction NAME(vint function, vint count);
				#define CTOR_VARIABLE(NAME)				static WfInstruction NAME(vint variable);
				#define CTOR_COUNT(NAME)				static WfInstruction NAME(vint count);
				#define CTOR_FLAG_TYPEDESCRIPTOR(NAME)	static WfInstruction NAME(reflection::description::Value::ValueType flag, reflection::description::ITypeDescriptor* typeDescriptor);
				#define CTOR_PROPERTY(NAME)				static WfInstruction NAME(reflection::description::IPropertyInfo* propertyInfo);
				#define CTOR_METHOD_COUNT(NAME)			static WfInstruction NAME(reflection::description::IMethodInfo* methodInfo, vint count);
				#define CTOR_EVENT(NAME)				static WfInstruction NAME(reflection::description::IEventInfo* eventInfo);
				#define CTOR_LABEL(NAME)				static WfInstruction NAME(vint label);
				#define CTOR_TYPE(NAME)					static WfInstruction NAME(WfInsType type);

				INSTRUCTION_CASES(
					CTOR,
					CTOR_VALUE,
					CTOR_FUNCTION,
					CTOR_FUNCTION_COUNT,
					CTOR_VARIABLE,
					CTOR_COUNT,
					CTOR_FLAG_TYPEDESCRIPTOR,
					CTOR_PROPERTY,
					CTOR_METHOD_COUNT,
					CTOR_EVENT,
					CTOR_LABEL,
					CTOR_TYPE)

				#undef CTOR
				#undef CTOR_VALUE
				#undef CTOR_FUNCTION
				#undef CTOR_FUNCTION_COUNT
				#undef CTOR_VARIABLE
				#undef CTOR_COUNT
				#undef CTOR_FLAG_TYPEDESCRIPTOR
				#undef CTOR_PROPERTY
				#undef CTOR_METHOD_COUNT
				#undef CTOR_EVENT
				#undef CTOR_LABEL
				#undef CTOR_TYPE
			};

/***********************************************************************
Assembly
***********************************************************************/

			class WfAssemblyFunction : public Object
			{
			public:
				WString												name;
				collections::List<WString>							argumentNames;
				collections::List<WString>							capturedVariableNames;
				collections::List<WString>							localVariableNames;
				vint												firstInstruction = -1;
				vint												lastInstruction = -1;
			};

			class WfInstructionDebugInfo : public Object
			{
			public:

				collections::List<WString>							moduleCodes;				// codeIndex -> code
				collections::List<parsing::ParsingTextRange>		instructionCodeMapping;		// instruction -> range
				collections::Group<Tuple<vint, vint>, vint>			codeInstructionMapping;		// (codeIndex, row) -> instruction [generated]

				void												Initialize();
			};

			class WfAssembly : public Object, public reflection::Description<WfAssembly>
			{
			protected:
				template<typename TIO>
				void IO(TIO& io);
			public:
				Ptr<WfInstructionDebugInfo>							insBeforeCodegen;
				Ptr<WfInstructionDebugInfo>							insAfterCodegen;
				collections::List<WString>							variableNames;
				collections::Group<WString, vint>					functionByName;
				collections::List<Ptr<WfAssemblyFunction>>			functions;
				collections::List<WfInstruction>					instructions;

				WfAssembly();
				WfAssembly(stream::IStream& input);
				
				void												Initialize();
				void												Serialize(stream::IStream& output);
			};

/***********************************************************************
RuntimeEnvironment
***********************************************************************/

			class WfRuntimeVariableContext : public Object
			{
				typedef collections::Array<reflection::description::Value>		VariableArray;

			public:
				VariableArray					variables;
			};

			class WfRuntimeGlobalContext : public Object
			{
			public:
				Ptr<WfAssembly>					assembly;
				Ptr<WfRuntimeVariableContext>	globalVariables;

				WfRuntimeGlobalContext(Ptr<WfAssembly> _assembly);
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

				Ptr<WfAssembly>					assembly;
				Ptr<WfRuntimeVariableContext>	global;
				Ptr<WfRuntimeVariableContext>	captured;
				Ptr<WfRuntimeVariableContext>	arguments;
				Ptr<WfRuntimeVariableContext>	localVariables;
				vint							functionIndex = -1;
				vint							instruction = -1;

				Ptr<IValueReadonlyDictionary>	GetLocalVariables()override;
				Ptr<IValueReadonlyDictionary>	GetLocalArguments()override;
				Ptr<IValueReadonlyDictionary>	GetCapturedVariables()override;
				Ptr<IValueReadonlyDictionary>	GetGlobalVariables()override;
				WString							GetFunctionName()override;
				WString							GetSourceCodeBeforeCodegen()override;
				WString							GetSourceCodeAfterCodegen()override;
				vint							GetRowBeforeCodegen()override;
				vint							GetRowAfterCodegen()override;
			};

			class WfRuntimeExceptionInfo : public Object, public virtual reflection::description::IValueException
			{
				typedef collections::List<Ptr<WfRuntimeCallStackInfo>>		CallStackList;
				using IValueReadonlyList = reflection::description::IValueReadonlyList;
			protected:
				Ptr<IValueReadonlyList>			cachedCallStack;

			public:
				WString							message;
				bool							fatal = false;
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

				Ptr<WfRuntimeExceptionInfo> GetInfo()const
				{
					return info;
				}

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

			struct WfRuntimeThreadContext
			{
				typedef collections::List<reflection::description::Value>		VariableList;
				typedef collections::List<WfRuntimeStackFrame>					StackFrameList;
				typedef collections::List<WfRuntimeTrapFrame>					TrapFrameList;

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
				WfRuntimeThreadContextError		PushValue(const reflection::description::Value& value);
				WfRuntimeThreadContextError		PopValue(reflection::description::Value& value);
				WfRuntimeThreadContextError		RaiseException(const WString& exception, bool fatalError, bool skipDebugger = false);
				WfRuntimeThreadContextError		RaiseException(Ptr<WfRuntimeExceptionInfo> info, bool skipDebugger = false);

				WfRuntimeThreadContextError		LoadStackValue(vint stackItemIndex, reflection::description::Value& value);
				WfRuntimeThreadContextError		LoadGlobalVariable(vint variableIndex, reflection::description::Value& value);
				WfRuntimeThreadContextError		StoreGlobalVariable(vint variableIndex, const reflection::description::Value& value);
				WfRuntimeThreadContextError		LoadCapturedVariable(vint variableIndex, reflection::description::Value& value);
				WfRuntimeThreadContextError		LoadLocalVariable(vint variableIndex, reflection::description::Value& value);
				WfRuntimeThreadContextError		StoreLocalVariable(vint variableIndex, const reflection::description::Value& value);

				WfRuntimeExecutionAction		ExecuteInternal(WfInstruction& ins, WfRuntimeStackFrame& stackFrame, IWfDebuggerCallback* callback);
				WfRuntimeExecutionAction		Execute(IWfDebuggerCallback* callback);
				void							ExecuteToEnd();
			};

/***********************************************************************
Debugger
***********************************************************************/

			class IWfBreakPointAction : public virtual Interface
			{
			public:
				virtual bool					EvaluateCondition(WfDebugger* debugger) = 0;
				virtual void					PostAction(WfDebugger* debugger) = 0;
			};

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

				static WfBreakPoint								Ins(WfAssembly* assembly, vint instruction);
				static WfBreakPoint								Read(WfAssembly* assembly, vint variable);
				static WfBreakPoint								Write(WfAssembly* assembly, vint variable);
				static WfBreakPoint								Get(reflection::DescriptableObject* thisObject, reflection::description::IPropertyInfo* propertyInfo);
				static WfBreakPoint								Set(reflection::DescriptableObject* thisObject, reflection::description::IPropertyInfo* propertyInfo);
				static WfBreakPoint								Attach(reflection::DescriptableObject* thisObject, reflection::description::IEventInfo* eventInfo);
				static WfBreakPoint								Detach(reflection::DescriptableObject* thisObject, reflection::description::IEventInfo* eventInfo);
				static WfBreakPoint								Invoke(reflection::DescriptableObject* thisObject, reflection::description::IMethodInfo* methodInfo);
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
				virtual bool					BreakInvoke(reflection::DescriptableObject* thisObject, reflection::description::IMethodInfo* methodInfo) = 0;
				virtual bool					BreakCreate(reflection::description::ITypeDescriptor* typeDescriptor) = 0;
				virtual bool					BreakException(Ptr<WfRuntimeExceptionInfo> info) = 0;
				virtual bool					WaitForContinue() = 0;
			};

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
				enum State
				{						//		Run		Pause	Stop	StepOver	StepInto
					Running,			// R			*RTP	*RTS
					PauseByOperation,	// PBO	*C				*RTS	*C			*C
					PauseByBreakPoint,	// PBB	*C				*RTS	*C			*C
					Stopped,			// S			*RTP			*			*
					Continue,			// C	soon becomes Running
					RequiredToPause,	// RTP	soon becomes PauseByOperation
					RequiredToStop,		// RTS	soon becomes Stop
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
				MethodBreakPointMap						invokeMethodBreakPoints;
				TypeBreakPointMap						createObjectBreakPoints;

				virtual void							OnBlockExecution();
				virtual void							OnStartExecution();
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
				bool									BreakInvoke(reflection::DescriptableObject* thisObject, reflection::description::IMethodInfo* methodInfo)override;
				bool									BreakCreate(reflection::description::ITypeDescriptor* typeDescriptor)override;
				bool									BreakException(Ptr<WfRuntimeExceptionInfo> info)override;
				bool									WaitForContinue()override;
			public:
				WfDebugger();
				~WfDebugger();

				vint									AddBreakPoint(const WfBreakPoint& breakPoint);
				vint									AddCodeLineBreakPoint(WfAssembly* assembly, vint codeIndex, vint row, bool beforeCodegen = true);
				vint									GetBreakPointCount();
				const WfBreakPoint&						GetBreakPoint(vint index);
				bool									RemoveBreakPoint(vint index);
				bool									EnableBreakPoint(vint index, bool enabled);
				bool									GetBreakException();
				void									SetBreakException(bool value);

				bool									Run();
				bool									Pause();
				bool									Stop();
				bool									StepOver(bool beforeCodegen = true);
				bool									StepInto(bool beforeCodegen = true);
				State									GetState();
				RunningType								GetRunningType();
				vint									GetLastActivatedBreakPoint();

				const ThreadContextList&				GetThreadContexts();
				WfRuntimeThreadContext*					GetCurrentThreadContext();
				const parsing::ParsingTextRange&		GetCurrentPosition(bool beforeCodegen = true, WfRuntimeThreadContext* context = nullptr, vint callStackIndex = -1);
				reflection::description::Value			GetValueByName(const WString& name, WfRuntimeThreadContext* context = nullptr, vint callStackIndex = -1);
			};

			extern IWfDebuggerCallback*					GetDebuggerCallback();
			extern IWfDebuggerCallback*					GetDebuggerCallback(WfDebugger* debugger);
			extern Ptr<WfDebugger>						GetDebuggerForCurrentThread();
			extern void									SetDebugferForCurrentThread(Ptr<WfDebugger> debugger);

/***********************************************************************
Helper Functions
***********************************************************************/

			extern Ptr<reflection::description::IValueFunctionProxy>		LoadFunction(Ptr<WfRuntimeGlobalContext> context, const WString& name);

			template<typename TFunction>
			Func<TFunction> LoadFunction(Ptr<WfRuntimeGlobalContext> context, const WString& name)
			{
				auto proxy = LoadFunction(context, name);
				Func<TFunction> function;
				reflection::description::UnboxParameter<Func<TFunction>>(reflection::description::Value::From(proxy), function);
				return function;
			}
		}
	}
}

#endif
