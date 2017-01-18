/***********************************************************************
Generated from ../Resources/Codegen/CallStaticMethod.txt
***********************************************************************/

#ifndef VCZH_WORKFLOW_CPP_GENERATED_CALLSTATICMETHOD
#define VCZH_WORKFLOW_CPP_GENERATED_CALLSTATICMETHOD

#include "../Source/CppTypes.h"

#if defined(__GNUC__)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wparentheses-equality"
#elif defined(__clang__)
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wparentheses-equality"
#endif

#pragma warning(push)
#pragma warning(disable:4250)
class Calculator;

class Calculator : public ::vl::Object, public ::vl::reflection::Description<Calculator>
{
public:
	class Internal;

	class Internal : public virtual ::vl::reflection::IDescriptable, public ::vl::reflection::Description<Internal>
	{
	public:

		static ::vl::vint32_t Sum(::vl::vint32_t begin, ::vl::vint32_t end);
	};

	static ::vl::vint32_t Sum2(::vl::vint32_t begin, ::vl::vint32_t end, ::vl::vint32_t sum);
	Calculator();
};
#pragma warning(pop)

/***********************************************************************
Global Variables and Functions
***********************************************************************/

namespace vl_workflow_global
{
	class CallStaticMethod
	{
	public:

		::vl::WString main();

		static CallStaticMethod& Instance();
	};
}

#if defined(__GNUC__)
#pragma GCC diagnostic pop
#elif defined(__clang__)
#pragma clang diagnostic pop
#endif

/***********************************************************************
Reflection
***********************************************************************/

namespace vl
{
	namespace reflection
	{
		namespace description
		{
#ifndef VCZH_DEBUG_NO_REFLECTION
			DECL_TYPE_INFO(::Calculator)
			DECL_TYPE_INFO(::Calculator::Internal)
#pragma warning(push)
#pragma warning(disable:4250)

			BEGIN_INTERFACE_PROXY_NOPARENT_SHAREDPTR(::Calculator::Internal)
				::vl::vint32_t Sum(::vl::vint32_t begin, ::vl::vint32_t end) override
				{
					INVOKEGET_INTERFACE_PROXY(Sum, begin, end);
				}
			END_INTERFACE_PROXY(::Calculator::Internal)
#pragma warning(pop)
#endif

			extern bool LoadCallStaticMethodTypes();
		}
	}
}

#endif
