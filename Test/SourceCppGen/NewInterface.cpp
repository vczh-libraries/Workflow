/***********************************************************************
!!!!!! DO NOT MODIFY !!!!!!

Source: ../Resources/Codegen/NewInterface.txt

This file is generated by Workflow compiler
https://github.com/vczh-libraries
***********************************************************************/

#include "NewInterface.h"

#if defined( _MSC_VER)
#pragma warning(push)
#pragma warning(disable:4250)
#elif defined(__GNUC__)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wparentheses-equality"
#elif defined(__clang__)
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wparentheses-equality"
#endif

#define GLOBAL_SYMBOL ::vl_workflow_global::NewInterface::
#define GLOBAL_NAME ::vl_workflow_global::NewInterface::Instance().
#define GLOBAL_OBJ &::vl_workflow_global::NewInterface::Instance()
#define USERIMPL(...)

/***********************************************************************
Global Variables
***********************************************************************/

BEGIN_GLOBAL_STORAGE_CLASS(vl_workflow_global_NewInterface)
	vl_workflow_global::NewInterface instance;
	INITIALIZE_GLOBAL_STORAGE_CLASS
	FINALIZE_GLOBAL_STORAGE_CLASS
END_GLOBAL_STORAGE_CLASS(vl_workflow_global_NewInterface)

namespace vl_workflow_global
{
/***********************************************************************
Global Functions
***********************************************************************/

	::vl::WString NewInterface::Show1(const ::vl::collections::LazyList<::vl::vint32_t>& xs)
	{
		auto s = ::vl::WString(L"", false);
		{
			auto __vwsn_for_enumerable_x = xs;
			auto __vwsn_for_enumerator_x = ::vl::__vwsn::This(__vwsn_for_enumerable_x.Obj())->CreateEnumerator();
			while (::vl::__vwsn::This(__vwsn_for_enumerator_x.Obj())->Next())
			{
				auto x = ::vl::__vwsn::Unbox<::vl::vint32_t>(::vl::__vwsn::This(__vwsn_for_enumerator_x.Obj())->GetCurrent());
				{
					if ((s == ::vl::WString(L"", false)))
					{
						(s = (s + ::vl::__vwsn::ToString(x)));
					}
					else
					{
						(s = ((s + ::vl::WString(L", ", false)) + ::vl::__vwsn::ToString(x)));
					}
				}
			}
		}
		return s;
	}

	::vl::WString NewInterface::Show2(const ::vl::collections::LazyList<::vl::vint32_t>& xs)
	{
		auto s = ::vl::WString(L"", false);
		{
			auto __vwsn_for_enumerable_x = ::vl::reflection::description::Sys::ReverseEnumerable(::vl::__vwsn::UnboxCollection<::vl::reflection::description::IValueEnumerable>(xs));
			auto __vwsn_for_enumerator_x = ::vl::__vwsn::This(__vwsn_for_enumerable_x.Obj())->CreateEnumerator();
			while (::vl::__vwsn::This(__vwsn_for_enumerator_x.Obj())->Next())
			{
				auto x = ::vl::__vwsn::Unbox<::vl::vint32_t>(::vl::__vwsn::This(__vwsn_for_enumerator_x.Obj())->GetCurrent());
				{
					if ((s == ::vl::WString(L"", false)))
					{
						(s = (s + ::vl::__vwsn::ToString(x)));
					}
					else
					{
						(s = ((s + ::vl::WString(L", ", false)) + ::vl::__vwsn::ToString(x)));
					}
				}
			}
		}
		return s;
	}

	::vl::WString NewInterface::main()
	{
		auto begin = 1;
		auto end = 5;
		auto xs = ::vl::reflection::description::GetLazyList<::vl::vint32_t>(::vl::Ptr<::vl::reflection::description::IValueEnumerable>(new ::vl_workflow_global::__vwsnc1_NewInterface_main__vl_reflection_description_IValueEnumerable(begin, end)));
		return ((((::vl::WString(L"[", false) + GLOBAL_NAME Show1(xs)) + ::vl::WString(L"][", false)) + GLOBAL_NAME Show2(xs)) + ::vl::WString(L"]", false));
	}

	NewInterface& NewInterface::Instance()
	{
		return Getvl_workflow_global_NewInterface().instance;
	}

/***********************************************************************
Closures
***********************************************************************/

	//-------------------------------------------------------------------

	__vwsnc1_NewInterface_main__vl_reflection_description_IValueEnumerable::__vwsnc1_NewInterface_main__vl_reflection_description_IValueEnumerable(::vl::vint32_t __vwsnctor_begin, ::vl::vint32_t __vwsnctor_end)
		:begin(__vwsnctor_begin)
		, end(__vwsnctor_end)
	{
	}

	::vl::Ptr<::vl::reflection::description::IValueEnumerator> __vwsnc1_NewInterface_main__vl_reflection_description_IValueEnumerable::CreateEnumerator()
	{
		return ::vl::Ptr<::vl::reflection::description::IValueEnumerator>(new ::vl_workflow_global::__vwsnc2_NewInterface_main_CreateEnumerator__vl_reflection_description_IValueEnumerator(::vl::__vwsn::This(this)->begin, ::vl::__vwsn::This(this)->end, this));
	}

	//-------------------------------------------------------------------

	__vwsnc2_NewInterface_main_CreateEnumerator__vl_reflection_description_IValueEnumerator::__vwsnc2_NewInterface_main_CreateEnumerator__vl_reflection_description_IValueEnumerator(::vl::vint32_t __vwsnctor_begin, ::vl::vint32_t __vwsnctor_end, ::vl::reflection::description::IValueEnumerable* __vwsnctorthis_0)
		:begin(__vwsnctor_begin)
		, end(__vwsnctor_end)
		, __vwsnthis_0(__vwsnctorthis_0)
	{
		this->index = (- 1);
	}

	::vl::reflection::description::Value __vwsnc2_NewInterface_main_CreateEnumerator__vl_reflection_description_IValueEnumerator::GetCurrent()
	{
		return ::vl::__vwsn::Box((::vl::__vwsn::This(this)->begin + index));
	}

	::vl::vint32_t __vwsnc2_NewInterface_main_CreateEnumerator__vl_reflection_description_IValueEnumerator::GetIndex()
	{
		return index;
	}

	bool __vwsnc2_NewInterface_main_CreateEnumerator__vl_reflection_description_IValueEnumerator::Next()
	{
		if ((index == (::vl::__vwsn::This(this)->end - ::vl::__vwsn::This(this)->begin)))
		{
			return false;
		}
		else
		{
			(index = (index + 1));
			return true;
		}
	}

}

#undef GLOBAL_SYMBOL
#undef GLOBAL_NAME
#undef GLOBAL_OBJ
#undef USERIMPL

#if defined( _MSC_VER)
#pragma warning(pop)
#elif defined(__GNUC__)
#pragma GCC diagnostic pop
#elif defined(__clang__)
#pragma clang diagnostic pop
#endif
