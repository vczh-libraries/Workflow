/***********************************************************************
!!!!!! DO NOT MODIFY !!!!!!

Source: ../Resources/Codegen/BindCustomInterface2.txt

This file is generated by Workflow compiler
https://github.com/vczh-libraries
***********************************************************************/

#include "BindCustomInterface2Reflection.h"

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
			IMPL_CPP_TYPE_INFO(IAdder2)
			IMPL_CPP_TYPE_INFO(ISummer2)

#ifdef VCZH_DESCRIPTABLEOBJECT_WITH_METADATA
#define _ ,
			BEGIN_INTERFACE_MEMBER(::IAdder2)
				CLASS_MEMBER_BASE(::ISummer2)
				CLASS_MEMBER_METHOD(Add, { L"value" })
			END_INTERFACE_MEMBER(::IAdder2)

			BEGIN_INTERFACE_MEMBER(::ISummer2)
				CLASS_MEMBER_BASE(::vl::reflection::IDescriptable)
				CLASS_MEMBER_METHOD(GetSum, NO_PARAMETER)
				CLASS_MEMBER_EVENT(SumChanged)
				CLASS_MEMBER_PROPERTY_EVENT_READONLY(Sum, GetSum, SumChanged)
			END_INTERFACE_MEMBER(::ISummer2)

#undef _
			class BindCustomInterface2TypeLoader : public Object, public ITypeLoader
			{
			public:
				void Load(ITypeManager* manager)
				{
					ADD_TYPE_INFO(::IAdder2)
					ADD_TYPE_INFO(::ISummer2)
				}

				void Unload(ITypeManager* manager)
				{
				}
			};
#endif
#endif

			bool LoadBindCustomInterface2Types()
			{
#ifdef VCZH_DESCRIPTABLEOBJECT_WITH_METADATA
				if (auto manager = GetGlobalTypeManager())
				{
					return manager->AddTypeLoader(Ptr(new BindCustomInterface2TypeLoader));
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
