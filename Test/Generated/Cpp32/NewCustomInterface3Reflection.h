/***********************************************************************
!!!!!! DO NOT MODIFY !!!!!!

Source: ../Resources/Codegen/NewCustomInterface3.txt

This file is generated by Workflow compiler
https://github.com/vczh-libraries
***********************************************************************/

#ifndef VCZH_WORKFLOW_COMPILER_GENERATED_NEWCUSTOMINTERFACE3REFLECTION
#define VCZH_WORKFLOW_COMPILER_GENERATED_NEWCUSTOMINTERFACE3REFLECTION

#include "NewCustomInterface3.h"

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
			DECL_TYPE_INFO(::IMyInterface3)
			DECL_TYPE_INFO(::MyClass)

#ifdef VCZH_DESCRIPTABLEOBJECT_WITH_METADATA

			BEGIN_INTERFACE_PROXY_SHAREDPTR(::IMyInterface3, ::vl::reflection::description::IValueEnumerable)
				::vl::vint32_t Get100() override
				{
					INVOKEGET_INTERFACE_PROXY_NOPARAMS(Get100);
				}
			END_INTERFACE_PROXY(::IMyInterface3)
#endif
#endif

			extern bool LoadNewCustomInterface3Types();
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
