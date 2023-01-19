/***********************************************************************
!!!!!! DO NOT MODIFY !!!!!!

Source: ../Resources/Codegen/BindCustomInterface2.txt

This file is generated by Workflow compiler
https://github.com/vczh-libraries
***********************************************************************/

#ifndef VCZH_WORKFLOW_COMPILER_GENERATED_BINDCUSTOMINTERFACE2REFLECTION
#define VCZH_WORKFLOW_COMPILER_GENERATED_BINDCUSTOMINTERFACE2REFLECTION

#include "BindCustomInterface2.h"

#if defined( _MSC_VER)
#pragma warning(push)
#pragma warning(disable:4250)
#elif defined(__clang__)
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wparentheses-equality"
#elif defined(__GNUC__)
#pragma GCC diagnostic push
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
			DECL_TYPE_INFO(::IAdder2)
			DECL_TYPE_INFO(::ISummer2)

#ifdef VCZH_DESCRIPTABLEOBJECT_WITH_METADATA

			BEGIN_INTERFACE_PROXY_NOPARENT_SHAREDPTR(::ISummer2)
				::vl::vint64_t GetSum() override
				{
					INVOKEGET_INTERFACE_PROXY_NOPARAMS(GetSum);
				}
			END_INTERFACE_PROXY(::ISummer2)

			BEGIN_INTERFACE_PROXY_SHAREDPTR(::IAdder2, ::ISummer2)
				void Add(::vl::vint64_t value) override
				{
					INVOKE_INTERFACE_PROXY(Add, value);
				}
			END_INTERFACE_PROXY(::IAdder2)
#endif
#endif

			extern bool LoadBindCustomInterface2Types();
		}
	}
}

#if defined( _MSC_VER)
#pragma warning(pop)
#elif defined(__clang__)
#pragma clang diagnostic pop
#elif defined(__GNUC__)
#pragma GCC diagnostic pop
#endif

#endif
