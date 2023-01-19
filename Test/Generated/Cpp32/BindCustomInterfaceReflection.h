/***********************************************************************
!!!!!! DO NOT MODIFY !!!!!!

Source: ../Resources/Codegen/BindCustomInterface.txt

This file is generated by Workflow compiler
https://github.com/vczh-libraries
***********************************************************************/

#ifndef VCZH_WORKFLOW_COMPILER_GENERATED_BINDCUSTOMINTERFACEREFLECTION
#define VCZH_WORKFLOW_COMPILER_GENERATED_BINDCUSTOMINTERFACEREFLECTION

#include "BindCustomInterface.h"

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
			DECL_TYPE_INFO(::IAdder)
			DECL_TYPE_INFO(::ISummer)

#ifdef VCZH_DESCRIPTABLEOBJECT_WITH_METADATA

			BEGIN_INTERFACE_PROXY_NOPARENT_SHAREDPTR(::ISummer)
				::vl::vint32_t GetSum() override
				{
					INVOKEGET_INTERFACE_PROXY_NOPARAMS(GetSum);
				}
			END_INTERFACE_PROXY(::ISummer)

			BEGIN_INTERFACE_PROXY_SHAREDPTR(::IAdder, ::ISummer)
				void Add(::vl::vint32_t value) override
				{
					INVOKE_INTERFACE_PROXY(Add, value);
				}
			END_INTERFACE_PROXY(::IAdder)
#endif
#endif

			extern bool LoadBindCustomInterfaceTypes();
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
