/***********************************************************************
Generated from ../Resources/Codegen/NewCustomInterface.txt
***********************************************************************/

#include "NewCustomInterface.h"

#if defined(__GNUC__)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wparentheses-equality"
#elif defined(__clang__)
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wparentheses-equality"
#endif
#define GLOBAL_SYMBOL ::vl_workflow_global::NewCustomInterface::
#define GLOBAL_NAME ::vl_workflow_global::NewCustomInterface::Instance().
#define GLOBAL_OBJ &::vl_workflow_global::NewCustomInterface::Instance()

/***********************************************************************
Global Variables and Functions
***********************************************************************/

BEGIN_GLOBAL_STORAGE_CLASS(vl_workflow_global_NewCustomInterface)
	vl_workflow_global::NewCustomInterface instance;
	INITIALIZE_GLOBAL_STORAGE_CLASS
	FINALIZE_GLOBAL_STORAGE_CLASS
END_GLOBAL_STORAGE_CLASS(vl_workflow_global_NewCustomInterface)

namespace vl_workflow_global
{
	class __vwsnc1_NewCustomInterface_main__IMyInterface : public ::vl::Object, public virtual ::IMyInterface
	{
	public:
		::vl::vint32_t begin;
		::vl::vint32_t end;

		__vwsnc1_NewCustomInterface_main__IMyInterface(::vl::vint32_t __vwsnctor_begin, ::vl::vint32_t __vwsnctor_end);

		::vl::Ptr<::vl::reflection::description::IValueEnumerator> CreateEnumerator() override;
		::vl::vint32_t Get100() override;
	};

	class __vwsnc2_NewCustomInterface_main_CreateEnumerator__vl_reflection_description_IValueEnumerator : public ::vl::Object, public virtual ::vl::reflection::description::IValueEnumerator
	{
	public:
		::vl::vint32_t begin;
		::vl::vint32_t end;
		::IMyInterface* __vwsnthis_0;

		__vwsnc2_NewCustomInterface_main_CreateEnumerator__vl_reflection_description_IValueEnumerator(::vl::vint32_t __vwsnctor_begin, ::vl::vint32_t __vwsnctor_end, ::IMyInterface* __vwsnctorthis_0);

		::vl::vint32_t index = 0;
		::vl::reflection::description::Value GetCurrent() override;
		::vl::vint32_t GetIndex() override;
		bool Next() override;
	};

	::vl::WString NewCustomInterface::main()
	{
		auto begin = 1;
		auto end = 5;
		auto xs = ::vl::Ptr<::IMyInterface>(new ::vl_workflow_global::__vwsnc1_NewCustomInterface_main__IMyInterface(begin, end));
		return ((((((::vl::WString(L"[", false) + ::vl::__vwsn::ToString(::vl::__vwsn::This(xs.Obj())->Get100())) + ::vl::WString(L"][", false)) + ::vl::__vwsn::ToString(::test::CustomInterfaceProcessor::Sum(::vl::__vwsn::Ensure(::vl::Ptr<::vl::reflection::description::IValueEnumerable>(xs))))) + ::vl::WString(L"][", false)) + ::vl::__vwsn::ToString(::test::CustomInterfaceProcessor::Sum(::vl::__vwsn::Ensure(::vl::Ptr<::vl::reflection::description::IValueEnumerable>(xs))))) + ::vl::WString(L"]", false));
	}

	NewCustomInterface& NewCustomInterface::Instance()
	{
		return Getvl_workflow_global_NewCustomInterface().instance;
	}

	__vwsnc1_NewCustomInterface_main__IMyInterface::__vwsnc1_NewCustomInterface_main__IMyInterface(::vl::vint32_t __vwsnctor_begin, ::vl::vint32_t __vwsnctor_end)
		:begin(__vwsnctor_begin)
		, end(__vwsnctor_end)
	{
	}

	::vl::Ptr<::vl::reflection::description::IValueEnumerator> __vwsnc1_NewCustomInterface_main__IMyInterface::CreateEnumerator()
	{
		return ::vl::Ptr<::vl::reflection::description::IValueEnumerator>(new ::vl_workflow_global::__vwsnc2_NewCustomInterface_main_CreateEnumerator__vl_reflection_description_IValueEnumerator(::vl::__vwsn::This(this)->begin, ::vl::__vwsn::This(this)->end, this));
	}

	::vl::vint32_t __vwsnc1_NewCustomInterface_main__IMyInterface::Get100()
	{
		return (::IMyInterface::Get50() * 2);
	}

	__vwsnc2_NewCustomInterface_main_CreateEnumerator__vl_reflection_description_IValueEnumerator::__vwsnc2_NewCustomInterface_main_CreateEnumerator__vl_reflection_description_IValueEnumerator(::vl::vint32_t __vwsnctor_begin, ::vl::vint32_t __vwsnctor_end, ::IMyInterface* __vwsnctorthis_0)
		:begin(__vwsnctor_begin)
		, end(__vwsnctor_end)
		, __vwsnthis_0(__vwsnctorthis_0)
	{
		this->index = (- 1);
	}

	::vl::reflection::description::Value __vwsnc2_NewCustomInterface_main_CreateEnumerator__vl_reflection_description_IValueEnumerator::GetCurrent()
	{
		return ::vl::__vwsn::Box((::vl::__vwsn::This(this)->begin + index));
	}

	::vl::vint32_t __vwsnc2_NewCustomInterface_main_CreateEnumerator__vl_reflection_description_IValueEnumerator::GetIndex()
	{
		return index;
	}

	bool __vwsnc2_NewCustomInterface_main_CreateEnumerator__vl_reflection_description_IValueEnumerator::Next()
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
Class (::IMyInterface)
***********************************************************************/

::vl::vint32_t IMyInterface::Get50()
{
	return 50;
}

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
			IMPL_CPP_TYPE_INFO(::IMyInterface)

#define _ ,
			BEGIN_INTERFACE_MEMBER(::IMyInterface)
				CLASS_MEMBER_METHOD_OVERLOAD(Get100, NO_PARAMETER, ::vl::vint32_t(::IMyInterface::*)())
				CLASS_MEMBER_STATIC_METHOD_OVERLOAD(Get50, NO_PARAMETER, ::vl::vint32_t(::IMyInterface::*)())
			END_INTERFACE_MEMBER(::IMyInterface)

#undef _
			class NewCustomInterfaceTypeLoader : public Object, public ITypeLoader
			{
			public:
				void Load(ITypeManager* manager)
				{
					ADD_TYPE_INFO(::IMyInterface)
				}

				void Unload(ITypeManager* manager)
				{
				}
			};
#endif

			bool LoadNewCustomInterfaceTypes()
			{
#ifndef VCZH_DEBUG_NO_REFLECTION
				if (auto manager = GetGlobalTypeManager())
				{
					return manager->AddTypeLoader(MakePtr<NewCustomInterfaceTypeLoader>());
				}
#endif
				return false;
			}
		}
	}
}
