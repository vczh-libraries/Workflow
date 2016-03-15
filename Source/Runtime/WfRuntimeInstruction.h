/***********************************************************************
Vczh Library++ 3.0
Developer: Zihan Chen(vczh)
Workflow::Runtime

Interfaces:
**********************************************************************/

#ifndef VCZH_WORKFLOW_RUNTIME_WFRUNTIMEINSTRUCTION
#define VCZH_WORKFLOW_RUNTIME_WFRUNTIMEINSTRUCTION

#include "../WorkflowVlppReferences.h"

namespace vl
{
	namespace workflow
	{
		namespace runtime
		{

/***********************************************************************
Instruction
***********************************************************************/

			enum class WfInsCode
			{
				// Instruction			// param				: <Stack-Pattern> -> <Stack-Pattern> in the order of <bottom ---- top>
				Nop,					//						: () -> ()															;
				LoadValue,				// value				: () -> Value														;
				LoadFunction,			// function				: () -> Value														; push the function index
				LoadException,			// 						: () -> Value														;
				LoadLocalVar,			// variable				: () -> Value														;
				LoadCapturedVar,		// variable				: () -> Value														;
				LoadGlobalVar,			// variable				: () -> Value														;
				LoadMethodInfo,			// IMethodInfo*			: () -> IMethodInfo*												;
				LoadMethodClosure,		// IMethodInfo*			: Value-this -> <function>											;
				LoadClosureContext,		//						: () -> <closure-context>											; load the current closure context
				StoreLocalVar,			// variable				: Value -> ()														;
				StoreCapturedVar,		// variable				: Value -> ()														;
				StoreGlobalVar,			// variable				: Value -> ()														;
				Duplicate,				// count				: () -> Value														; copy stack[stack.Count()-1-count]
				Pop,					//						: Value -> ()														;
				Return,					//						: Value -> Value													; (exit function)
				CreateArray,			// count				: Value-count, ..., Value-1 -> <array>								; {1 2 3} -> <3 2 1>
				CreateMap,				// count				: Value-count, ..., Value-1 -> <map>								; {1:2 3:4} -> <3 4 1 2>
				CreateClosureContext,	// count				: Value-1, ..., Value-count -> <closure-context>					;
				CreateClosure,			//						: <closure-context>, Value-function-index -> <closure>				;
				CreateInterface,		// IMethodInfo*, count	: <closure-context>, Value-count, ..., Value-1 -> <map>				; {"Get":a "Set":b} -> new TInterface(InterfaceProxy^)
				CreateRange,			// I1248/U1248			: Value-begin, Value-end -> <enumerable>							;
				ReverseEnumerable,		//						: Value -> Value													;
				DeleteRawPtr,			//						: Value -> ()														;
				ConvertToType,			// flag, typeDescriptor	: Value -> Value													;
				TryConvertToType,		// flag, typeDescriptor	: Value -> Value													;
				TestType,				// flag, typeDescriptor	: Value -> <bool>													;
				GetType,				//						: Value -> <ITypeDescriptor*>										;
				Jump,					// label				: () -> ()															;
				JumpIf,					// label				: () -> ()															;
				Invoke,					// function, count		: Value-1, ..., Value-n -> Value									;
				InvokeWithContext,		// function, count		: Value-1, ..., Value-n -> Value									;
				GetProperty,			// IPropertyInfo*		: Value-this -> Value												;
				SetProperty,			// IPropertyInfo*		: Value, Value-this -> ()											;
				InvokeProxy,			// count				: Value-1, ..., Value-n, Value-this -> Value						;
				InvokeMethod,			// IMethodInfo*, count	: Value-1, ..., Value-n, Value-this -> Value						;
				InvokeEvent,			// IEventInfo*, count	: Value-1, ..., Value-n, Value-this -> Value						;
				AttachEvent,			// IEventInfo*			: Value-this, <function> -> <Listener>								;
				DetachEvent,			// 						: <Listener> -> bool												;
				InstallTry,				// label				: () -> ()															;
				UninstallTry,			// count				: () -> ()															;
				RaiseException,			// 						: Value -> ()														; (trap)
				TestElementInSet,		//						: Value-element, Value-set -> bool									;
				CompareLiteral,			// I48/U48/F48/S		: Value, Value -> <int>												;
				CompareStruct,			// 						: Value, Value -> <bool>											;
				CompareReference,		// 						: Value, Value -> <bool>											;
				CompareValue,			// 						: Value, Value -> <bool>											;
				OpNot,					// B/I1248/U1248		: Value -> Value													;
				OpPositive,				// I1248/U1248			: Value -> Value													;
				OpNegative,				// I1248				: Value -> Value													;
				OpConcat,				// 						: <string>, <string> -> <string>									;
				OpExp,					// I48/U48/F48			: Value, Value -> Value												;
				OpAdd,					// I48/U48/F48			: Value, Value -> Value												;
				OpSub,					// I48/U48/F48			: Value, Value -> Value												;
				OpMul,					// I48/U48/F48			: Value, Value -> Value												;
				OpDiv,					// I48/U48/F48			: Value, Value -> Value												;
				OpMod,					// I48/U48				: Value, Value -> Value												;
				OpShl,					// I48/U48				: Value, Value -> Value												;
				OpShr,					// I48/U48				: Value, Value -> Value												;
				OpXor,					// B/I1248/U1248		: <bool>, <bool> -> <bool>											;
				OpAnd,					// B/I1248/U1248		: <bool>, <bool> -> <bool>											;
				OpOr,					// B/I1248/U1248		: <bool>, <bool> -> <bool>											;
				OpLT,					// 						: <int> -> <bool>													;
				OpGT,					// 						: <int> -> <bool>													;
				OpLE,					// 						: <int> -> <bool>													;
				OpGE,					// 						: <int> -> <bool>													;
				OpEQ,					// 						: <int> -> <bool>													;
				OpNE,					// 						: <int> -> <bool>													;
			};

#define INSTRUCTION_CASES(APPLY, APPLY_VALUE, APPLY_FUNCTION, APPLY_FUNCTION_COUNT, APPLY_VARIABLE, APPLY_COUNT, APPLY_FLAG_TYPEDESCRIPTOR, APPLY_PROPERTY, APPLY_METHOD, APPLY_METHOD_COUNT, APPLY_EVENT, APPLY_EVENT_COUNT, APPLY_LABEL, APPLY_TYPE)\
			APPLY(Nop)\
			APPLY_VALUE(LoadValue)\
			APPLY_FUNCTION(LoadFunction)\
			APPLY(LoadException)\
			APPLY_VARIABLE(LoadLocalVar)\
			APPLY_VARIABLE(LoadCapturedVar)\
			APPLY_VARIABLE(LoadGlobalVar)\
			APPLY_METHOD(LoadMethodInfo)\
			APPLY_METHOD(LoadMethodClosure)\
			APPLY(LoadClosureContext)\
			APPLY_VARIABLE(StoreLocalVar)\
			APPLY_VARIABLE(StoreCapturedVar)\
			APPLY_VARIABLE(StoreGlobalVar)\
			APPLY_COUNT(Duplicate)\
			APPLY(Pop)\
			APPLY(Return)\
			APPLY_COUNT(CreateArray)\
			APPLY_COUNT(CreateMap)\
			APPLY_COUNT(CreateClosureContext)\
			APPLY(CreateClosure)\
			APPLY_METHOD_COUNT(CreateInterface)\
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
			APPLY_FUNCTION_COUNT(InvokeWithContext)\
			APPLY_PROPERTY(GetProperty)\
			APPLY_PROPERTY(SetProperty)\
			APPLY_COUNT(InvokeProxy)\
			APPLY_METHOD_COUNT(InvokeMethod)\
			APPLY_EVENT_COUNT(InvokeEvent)\
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
				#define CTOR_METHOD(NAME)				static WfInstruction NAME(reflection::description::IMethodInfo* methodInfo);
				#define CTOR_METHOD_COUNT(NAME)			static WfInstruction NAME(reflection::description::IMethodInfo* methodInfo, vint count);
				#define CTOR_EVENT(NAME)				static WfInstruction NAME(reflection::description::IEventInfo* eventInfo);
				#define CTOR_EVENT_COUNT(NAME)			static WfInstruction NAME(reflection::description::IEventInfo* eventInfo, vint count);
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
					CTOR_METHOD,
					CTOR_METHOD_COUNT,
					CTOR_EVENT,
					CTOR_EVENT_COUNT,
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
				#undef CTOR_METHOD
				#undef CTOR_METHOD_COUNT
				#undef CTOR_EVENT
				#undef CTOR_EVENT_COUNT
				#undef CTOR_LABEL
				#undef CTOR_TYPE
			};
		}
	}
}

#endif
