/***********************************************************************
Generated from ../Resources/Codegen/NewInterface.txt
***********************************************************************/

#include "NewInterface.h"

#define GLOBAL_SYMBOL ::vl_workflow_global::NewInterface::
#define GLOBAL_NAME ::vl_workflow_global::NewInterface::Instance().
#define GLOBAL_OBJ &::vl_workflow_global::NewInterface::Instance()

/***********************************************************************
Global Variables and Functions
***********************************************************************/

BEGIN_GLOBAL_STORAGE_CLASS(vl_workflow_global_NewInterface)
	vl_workflow_global::NewInterface instance;
	INITIALIZE_GLOBAL_STORAGE_CLASS
	FINALIZE_GLOBAL_STORAGE_CLASS
END_GLOBAL_STORAGE_CLASS(vl_workflow_global_NewInterface)

namespace vl_workflow_global
{
	class __vwsnc1_NewInterface_main__vl_reflection_description_IValueEnumerable : public ::vl::Object, public virtual ::vl::reflection::description::IValueEnumerable
	{
	public:
		::vl::vint32_t begin;
		::vl::vint32_t end;

		__vwsnc1_NewInterface_main__vl_reflection_description_IValueEnumerable(::vl::vint32_t __vwsnctor_begin, ::vl::vint32_t __vwsnctor_end);

		::vl::Ptr<::vl::reflection::description::IValueEnumerator> CreateEnumerator() override;
	};

	class __vwsnc2_NewInterface_main_CreateEnumerator__vl_reflection_description_IValueEnumerator : public ::vl::Object, public virtual ::vl::reflection::description::IValueEnumerator
	{
	public:
		::vl::vint32_t begin;
		::vl::vint32_t end;
		::vl::reflection::description::IValueEnumerable* __vwsnthis_0;

		__vwsnc2_NewInterface_main_CreateEnumerator__vl_reflection_description_IValueEnumerator(::vl::vint32_t __vwsnctor_begin, ::vl::vint32_t __vwsnctor_end, ::vl::reflection::description::IValueEnumerable* __vwsnctorthis_0);

		::vl::vint32_t index = 0;
		::vl::reflection::description::Value GetCurrent() override;
		::vl::vint32_t GetIndex() override;
		bool Next() override;
	};

	::vl::WString NewInterface::Show1(const ::vl::collections::LazyList<::vl::vint32_t>& xs)
	{
		auto s = ::vl::WString(L"", false);
		using __vwsnt_0 = ::vl::vint32_t;
		FOREACH(__vwsnt_0, x, xs)
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
		return s;
	}

	::vl::WString NewInterface::Show2(const ::vl::collections::LazyList<::vl::vint32_t>& xs)
	{
		auto s = ::vl::WString(L"", false);
		using __vwsnt_0 = ::vl::vint32_t;
		FOREACH(__vwsnt_0, x, xs.Reverse())
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

	__vwsnc1_NewInterface_main__vl_reflection_description_IValueEnumerable::__vwsnc1_NewInterface_main__vl_reflection_description_IValueEnumerable(::vl::vint32_t __vwsnctor_begin, ::vl::vint32_t __vwsnctor_end)
		:begin(__vwsnctor_begin)
		, end(__vwsnctor_end)
	{
	}

	::vl::Ptr<::vl::reflection::description::IValueEnumerator> __vwsnc1_NewInterface_main__vl_reflection_description_IValueEnumerable::CreateEnumerator()
	{
		return ::vl::Ptr<::vl::reflection::description::IValueEnumerator>(new ::vl_workflow_global::__vwsnc2_NewInterface_main_CreateEnumerator__vl_reflection_description_IValueEnumerator(::vl::__vwsn::This(this)->begin, ::vl::__vwsn::This(this)->end, this));
	}

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
