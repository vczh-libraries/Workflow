/***********************************************************************
Generated from ../Resources/Codegen/NewCustomInterface2.txt
***********************************************************************/

#include "NewCustomInterface2.h"

#if defined(__GNUC__)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wparentheses-equality"
#elif defined(__clang__)
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wparentheses-equality"
#endif
#define GLOBAL_SYMBOL ::vl_workflow_global::NewCustomInterface2::
#define GLOBAL_NAME ::vl_workflow_global::NewCustomInterface2::Instance().
#define GLOBAL_OBJ &::vl_workflow_global::NewCustomInterface2::Instance()

/***********************************************************************
Global Variables and Functions
***********************************************************************/

BEGIN_GLOBAL_STORAGE_CLASS(vl_workflow_global_NewCustomInterface2)
	vl_workflow_global::NewCustomInterface2 instance;
	INITIALIZE_GLOBAL_STORAGE_CLASS
	FINALIZE_GLOBAL_STORAGE_CLASS
END_GLOBAL_STORAGE_CLASS(vl_workflow_global_NewCustomInterface2)

namespace vl_workflow_global
{
	class __vwsnc1_NewCustomInterface2_main__IMyInterface2 : public ::vl::Object, public virtual ::IMyInterface2
	{
	public:
		::vl::vint32_t begin;
		::vl::vint32_t end;

		__vwsnc1_NewCustomInterface2_main__IMyInterface2(::vl::vint32_t __vwsnctor_begin, ::vl::vint32_t __vwsnctor_end);

		::vl::Ptr<::vl::reflection::description::IValueEnumerator> CreateEnumerator() override;
		::vl::vint32_t Get50();
		::vl::vint32_t Get100() override;
	};

	class __vwsnc2_NewCustomInterface2_main_CreateEnumerator__vl_reflection_description_IValueEnumerator : public ::vl::Object, public virtual ::vl::reflection::description::IValueEnumerator
	{
	public:
		::vl::vint32_t begin;
		::vl::vint32_t end;
		::IMyInterface2* __vwsnthis_0;

		__vwsnc2_NewCustomInterface2_main_CreateEnumerator__vl_reflection_description_IValueEnumerator(::vl::vint32_t __vwsnctor_begin, ::vl::vint32_t __vwsnctor_end, ::IMyInterface2* __vwsnctorthis_0);

		::vl::vint32_t index = 0;
		::vl::reflection::description::Value GetCurrent() override;
		::vl::vint32_t GetIndex() override;
		bool Next() override;
	};

	::vl::WString NewCustomInterface2::main()
	{
		auto begin = 1;
		auto end = 5;
		auto xs = ::vl::Ptr<::IMyInterface2>(new ::vl_workflow_global::__vwsnc1_NewCustomInterface2_main__IMyInterface2(begin, end));
		return ((((((::vl::WString(L"[", false) + ::vl::__vwsn::ToString(::vl::__vwsn::This(xs.Obj())->Get100())) + ::vl::WString(L"][", false)) + ::vl::__vwsn::ToString(::test::CustomInterfaceProcessor::Sum(::vl::__vwsn::Ensure(::vl::Ptr<::vl::reflection::description::IValueEnumerable>(xs))))) + ::vl::WString(L"][", false)) + ::vl::__vwsn::ToString(::test::CustomInterfaceProcessor::Sum(::vl::__vwsn::Ensure(::vl::Ptr<::vl::reflection::description::IValueEnumerable>(xs))))) + ::vl::WString(L"]", false));
	}

	NewCustomInterface2& NewCustomInterface2::Instance()
	{
		return Getvl_workflow_global_NewCustomInterface2().instance;
	}

	__vwsnc1_NewCustomInterface2_main__IMyInterface2::__vwsnc1_NewCustomInterface2_main__IMyInterface2(::vl::vint32_t __vwsnctor_begin, ::vl::vint32_t __vwsnctor_end)
		:begin(__vwsnctor_begin)
		, end(__vwsnctor_end)
	{
	}

	::vl::Ptr<::vl::reflection::description::IValueEnumerator> __vwsnc1_NewCustomInterface2_main__IMyInterface2::CreateEnumerator()
	{
		return ::vl::Ptr<::vl::reflection::description::IValueEnumerator>(new ::vl_workflow_global::__vwsnc2_NewCustomInterface2_main_CreateEnumerator__vl_reflection_description_IValueEnumerator(::vl::__vwsn::This(this)->begin, ::vl::__vwsn::This(this)->end, this));
	}

	::vl::vint32_t __vwsnc1_NewCustomInterface2_main__IMyInterface2::Get50()
	{
		return 50;
	}

	::vl::vint32_t __vwsnc1_NewCustomInterface2_main__IMyInterface2::Get100()
	{
		return (::vl::__vwsn::This(this)->Get50() * 2);
	}

	__vwsnc2_NewCustomInterface2_main_CreateEnumerator__vl_reflection_description_IValueEnumerator::__vwsnc2_NewCustomInterface2_main_CreateEnumerator__vl_reflection_description_IValueEnumerator(::vl::vint32_t __vwsnctor_begin, ::vl::vint32_t __vwsnctor_end, ::IMyInterface2* __vwsnctorthis_0)
		:begin(__vwsnctor_begin)
		, end(__vwsnctor_end)
		, __vwsnthis_0(__vwsnctorthis_0)
	{
		this->index = (- 1);
	}

	::vl::reflection::description::Value __vwsnc2_NewCustomInterface2_main_CreateEnumerator__vl_reflection_description_IValueEnumerator::GetCurrent()
	{
		return ::vl::__vwsn::Box((::vl::__vwsn::This(this)->begin + index));
	}

	::vl::vint32_t __vwsnc2_NewCustomInterface2_main_CreateEnumerator__vl_reflection_description_IValueEnumerator::GetIndex()
	{
		return index;
	}

	bool __vwsnc2_NewCustomInterface2_main_CreateEnumerator__vl_reflection_description_IValueEnumerator::Next()
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

/***********************************************************************
Class (::IMyInterface2)
***********************************************************************/

#undef GLOBAL_SYMBOL
#undef GLOBAL_NAME
#undef GLOBAL_OBJ

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
			IMPL_CPP_TYPE_INFO(::IMyInterface2)

#define _ ,
			BEGIN_INTERFACE_MEMBER(::IMyInterface2)
				CLASS_MEMBER_METHOD(Get100, NO_PARAMETER)
			END_INTERFACE_MEMBER(::IMyInterface2)

#undef _
			class NewCustomInterface2TypeLoader : public Object, public ITypeLoader
			{
			public:
				void Load(ITypeManager* manager)
				{
					ADD_TYPE_INFO(::IMyInterface2)
				}

				void Unload(ITypeManager* manager)
				{
				}
			};
#endif

			bool LoadNewCustomInterface2Types()
			{
#ifndef VCZH_DEBUG_NO_REFLECTION
				if (auto manager = GetGlobalTypeManager())
				{
					return manager->AddTypeLoader(MakePtr<NewCustomInterface2TypeLoader>());
				}
#endif
				return false;
			}
		}
	}
}
