/***********************************************************************
!!!!!! DO NOT MODIFY !!!!!!

Source: ../Resources/Codegen/CallStaticMethod.txt

This file is generated by Workflow compiler
https://github.com/vczh-libraries
***********************************************************************/

#include "CallStaticMethodReflection.h"

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
			IMPL_CPP_TYPE_INFO(Calculator)
			IMPL_CPP_TYPE_INFO(Calculator::Internal)

#ifdef VCZH_DESCRIPTABLEOBJECT_WITH_METADATA
#define _ ,
			BEGIN_CLASS_MEMBER(::Calculator)
				CLASS_MEMBER_BASE(::vl::reflection::DescriptableObject)
				CLASS_MEMBER_CONSTRUCTOR(::vl::Ptr<::Calculator>(), NO_PARAMETER)
				CLASS_MEMBER_STATIC_METHOD(Sum2, { L"begin" _ L"end" _ L"sum" })
			END_CLASS_MEMBER(::Calculator)

			BEGIN_INTERFACE_MEMBER(::Calculator::Internal)
				CLASS_MEMBER_BASE(::vl::reflection::IDescriptable)
				CLASS_MEMBER_STATIC_METHOD(Sum, { L"begin" _ L"end" })
			END_INTERFACE_MEMBER(::Calculator::Internal)

#undef _
			class CallStaticMethodTypeLoader : public Object, public ITypeLoader
			{
			public:
				void Load(ITypeManager* manager)
				{
					ADD_TYPE_INFO(::Calculator)
					ADD_TYPE_INFO(::Calculator::Internal)
				}

				void Unload(ITypeManager* manager)
				{
				}
			};
#endif
#endif

			bool LoadCallStaticMethodTypes()
			{
#ifdef VCZH_DESCRIPTABLEOBJECT_WITH_METADATA
				if (auto manager = GetGlobalTypeManager())
				{
					return manager->AddTypeLoader(Ptr(new CallStaticMethodTypeLoader));
				}
#endif
				return false;
			}
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
