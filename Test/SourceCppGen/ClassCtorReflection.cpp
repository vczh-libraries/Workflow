/***********************************************************************
!!!!!! DO NOT MODIFY !!!!!!

Source: ../Resources/Codegen/ClassCtor.txt

This file is generated by Workflow compiler
https://github.com/vczh-libraries
***********************************************************************/

#include "ClassCtorReflection.h"

#if defined( _MSC_VER)
#pragma warning(push)
#pragma warning(disable:4250)
#elif defined(__GNUC__)
#pragma GCC diagnostic push
#elif defined(__clang__)
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wparentheses-equality"
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
			IMPL_CPP_TYPE_INFO(Base)
			IMPL_CPP_TYPE_INFO(Ctor)

#ifdef VCZH_DESCRIPTABLEOBJECT_WITH_METADATA
#define _ ,
			BEGIN_CLASS_MEMBER(::Base)
				CLASS_MEMBER_BASE(::vl::reflection::DescriptableObject)
				CLASS_MEMBER_CONSTRUCTOR(::vl::Ptr<::Base>(), NO_PARAMETER)
				CLASS_MEMBER_CONSTRUCTOR(::vl::Ptr<::Base>(::vl::vint), { L"_x" })
				CLASS_MEMBER_METHOD(GetX, NO_PARAMETER)
				CLASS_MEMBER_METHOD(SetX, { L"_x" })
				CLASS_MEMBER_EVENT(XChanged)
				CLASS_MEMBER_PROPERTY_EVENT(X, GetX, SetX, XChanged)
				CLASS_MEMBER_FIELD(x)
			END_CLASS_MEMBER(::Base)

			BEGIN_CLASS_MEMBER(::Ctor)
				CLASS_MEMBER_BASE(::Base)
				CLASS_MEMBER_BASE(::test::ObservableValue)
				CLASS_MEMBER_CONSTRUCTOR(::vl::Ptr<::Ctor>(), NO_PARAMETER)
				CLASS_MEMBER_CONSTRUCTOR(::vl::Ptr<::Ctor>(::vl::vint, bool), { L"y" _ L"b" })
				CLASS_MEMBER_CONSTRUCTOR(::vl::Ptr<::Ctor>(bool, ::vl::vint), { L"b" _ L"value" })
				CLASS_MEMBER_CONSTRUCTOR(::vl::Ptr<::Ctor>(::vl::vint, ::vl::vint), { L"y" _ L"value" })
			END_CLASS_MEMBER(::Ctor)

#undef _
			class ClassCtorTypeLoader : public Object, public ITypeLoader
			{
			public:
				void Load(ITypeManager* manager)
				{
					ADD_TYPE_INFO(::Base)
					ADD_TYPE_INFO(::Ctor)
				}

				void Unload(ITypeManager* manager)
				{
				}
			};
#endif
#endif

			bool LoadClassCtorTypes()
			{
#ifdef VCZH_DESCRIPTABLEOBJECT_WITH_METADATA
				if (auto manager = GetGlobalTypeManager())
				{
					return manager->AddTypeLoader(MakePtr<ClassCtorTypeLoader>());
				}
#endif
				return false;
			}
		}
	}
}

#if defined( _MSC_VER)
#pragma warning(pop)
#elif defined(__GNUC__)
#pragma GCC diagnostic pop
#elif defined(__clang__)
#pragma clang diagnostic pop
#endif
