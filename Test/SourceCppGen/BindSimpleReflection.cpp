/***********************************************************************
!!!!!! DO NOT MODIFY !!!!!!

Source: ../Resources/Codegen/BindSimple.txt

This file is generated by Workflow compiler
https://github.com/vczh-libraries
***********************************************************************/

#include "BindSimpleReflection.h"

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
			IMPL_CPP_TYPE_INFO(Derived)
			IMPL_CPP_TYPE_INFO(OuterClass)
			IMPL_CPP_TYPE_INFO(OuterClass::Base)

#ifdef VCZH_DESCRIPTABLEOBJECT_WITH_METADATA
#define _ ,
			BEGIN_CLASS_MEMBER(::Derived)
				CLASS_MEMBER_BASE(::OuterClass::Base)
				CLASS_MEMBER_CONSTRUCTOR(::vl::Ptr<::Derived>(), NO_PARAMETER)
				CLASS_MEMBER_METHOD(Execute, NO_PARAMETER)
				CLASS_MEMBER_FIELD(x)
				CLASS_MEMBER_FIELD(y)
				CLASS_MEMBER_FIELD(z)
			END_CLASS_MEMBER(::Derived)

			BEGIN_CLASS_MEMBER(::OuterClass)
				CLASS_MEMBER_BASE(::vl::reflection::DescriptableObject)
				CLASS_MEMBER_CONSTRUCTOR(::vl::Ptr<::OuterClass>(), NO_PARAMETER)
			END_CLASS_MEMBER(::OuterClass)

			BEGIN_CLASS_MEMBER(::OuterClass::Base)
				CLASS_MEMBER_BASE(::vl::reflection::DescriptableObject)
				CLASS_MEMBER_CONSTRUCTOR(::vl::Ptr<::OuterClass::Base>(), NO_PARAMETER)
				CLASS_MEMBER_METHOD(Initialize, { L"derived" })
				CLASS_MEMBER_FIELD(s)
				CLASS_MEMBER_FIELD(subscription)
			END_CLASS_MEMBER(::OuterClass::Base)

#undef _
			class BindSimpleTypeLoader : public Object, public ITypeLoader
			{
			public:
				void Load(ITypeManager* manager)
				{
					ADD_TYPE_INFO(::Derived)
					ADD_TYPE_INFO(::OuterClass)
					ADD_TYPE_INFO(::OuterClass::Base)
				}

				void Unload(ITypeManager* manager)
				{
				}
			};
#endif
#endif

			bool LoadBindSimpleTypes()
			{
#ifdef VCZH_DESCRIPTABLEOBJECT_WITH_METADATA
				if (auto manager = GetGlobalTypeManager())
				{
					return manager->AddTypeLoader(Ptr(new BindSimpleTypeLoader));
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
