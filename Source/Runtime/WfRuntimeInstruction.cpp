#include "WfRuntimeInstruction.h"

namespace vl
{
	namespace workflow
	{
		namespace runtime
		{
			using namespace reflection::description;

/***********************************************************************
WfInstruction
***********************************************************************/

			WfInstruction::WfInstruction()
				:flagParameter(Value::Null)
				, typeDescriptorParameter(0)
			{

			}

#define CTOR(NAME)\
	WfInstruction WfInstruction::NAME()\
			{\
			WfInstruction ins; \
			ins.code = WfInsCode::NAME; \
			return ins; \
			}\

#define CTOR_VALUE(NAME)\
	WfInstruction WfInstruction::NAME(const reflection::description::Value& value)\
			{\
			WfInstruction ins; \
			ins.code = WfInsCode::NAME; \
			ins.valueParameter = value; \
			return ins; \
			}\

#define CTOR_FUNCTION_COUNT(NAME)\
	WfInstruction WfInstruction::NAME(vint function, vint count)\
			{\
			WfInstruction ins; \
			ins.code = WfInsCode::NAME; \
			ins.indexParameter = function; \
			ins.countParameter = count; \
			return ins; \
			}\

#define CTOR_VARIABLE(NAME)\
	WfInstruction WfInstruction::NAME(vint variable)\
			{\
			WfInstruction ins; \
			ins.code = WfInsCode::NAME; \
			ins.indexParameter = variable; \
			return ins; \
			}\

#define CTOR_COUNT(NAME)\
	WfInstruction WfInstruction::NAME(vint count)\
			{\
			WfInstruction ins; \
			ins.code = WfInsCode::NAME; \
			ins.countParameter = count; \
			return ins; \
			}\

#define CTOR_FLAG_TYPEDESCRIPTOR(NAME)\
	WfInstruction WfInstruction::NAME(reflection::description::Value::ValueType flag, reflection::description::ITypeDescriptor* typeDescriptor)\
			{\
			WfInstruction ins; \
			ins.code = WfInsCode::NAME; \
			ins.flagParameter = flag; \
			ins.typeDescriptorParameter = typeDescriptor; \
			return ins; \
			}\

#define CTOR_PROPERTY(NAME)\
	WfInstruction WfInstruction::NAME(reflection::description::IPropertyInfo* propertyInfo)\
			{\
			WfInstruction ins; \
			ins.code = WfInsCode::NAME; \
			ins.propertyParameter = propertyInfo; \
			return ins; \
			}\

#define CTOR_METHOD(NAME)\
	WfInstruction WfInstruction::NAME(reflection::description::IMethodInfo* methodInfo)\
			{\
			WfInstruction ins; \
			ins.code = WfInsCode::NAME; \
			ins.methodParameter = methodInfo; \
			return ins; \
			}\

#define CTOR_METHOD_COUNT(NAME)\
	WfInstruction WfInstruction::NAME(reflection::description::IMethodInfo* methodInfo, vint count)\
			{\
			WfInstruction ins; \
			ins.code = WfInsCode::NAME; \
			ins.methodParameter = methodInfo; \
			ins.countParameter = count; \
			return ins; \
			}\

#define CTOR_EVENT(NAME)\
	WfInstruction WfInstruction::NAME(reflection::description::IEventInfo* eventInfo)\
			{\
			WfInstruction ins; \
			ins.code = WfInsCode::NAME; \
			ins.eventParameter = eventInfo; \
			return ins; \
			}\

#define CTOR_LABEL(NAME)\
	WfInstruction WfInstruction::NAME(vint label)\
			{\
			WfInstruction ins; \
			ins.code = WfInsCode::NAME; \
			ins.indexParameter = label; \
			return ins; \
			}\

#define CTOR_TYPE(NAME)\
	WfInstruction WfInstruction::NAME(WfInsType type)\
			{\
			WfInstruction ins; \
			ins.code = WfInsCode::NAME; \
			ins.typeParameter = type; \
			return ins; \
			}\

			INSTRUCTION_CASES(
				CTOR,
				CTOR_VALUE,
				CTOR_FUNCTION_COUNT,
				CTOR_VARIABLE,
				CTOR_COUNT,
				CTOR_FLAG_TYPEDESCRIPTOR,
				CTOR_PROPERTY,
				CTOR_METHOD,
				CTOR_METHOD_COUNT,
				CTOR_EVENT,
				CTOR_LABEL,
				CTOR_TYPE)

#undef CTOR
#undef CTOR_VALUE
#undef CTOR_FUNCTION_COUNT
#undef CTOR_VARIABLE
#undef CTOR_COUNT
#undef CTOR_FLAG_TYPEDESCRIPTOR
#undef CTOR_PROPERTY
#undef CTOR_METHOD
#undef CTOR_METHOD_COUNT
#undef CTOR_EVENT
#undef CTOR_LABEL
#undef CTOR_TYPE
		}
	}
}