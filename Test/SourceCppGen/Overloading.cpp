/***********************************************************************
!!!!!! DO NOT MODIFY !!!!!!

Source: ../Resources/Codegen/Overloading.txt

This file is generated by Workflow compiler
https://github.com/vczh-libraries
***********************************************************************/

#include "Overloading.h"

#if defined( _MSC_VER)
#pragma warning(push)
#pragma warning(disable:4250)
#elif defined(__GNUC__)
#pragma GCC diagnostic push
#elif defined(__clang__)
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wparentheses-equality"
#endif

#define GLOBAL_SYMBOL ::vl_workflow_global::Overloading::
#define GLOBAL_NAME ::vl_workflow_global::Overloading::Instance().
#define GLOBAL_OBJ &::vl_workflow_global::Overloading::Instance()

/***********************************************************************
Global Variables
***********************************************************************/

BEGIN_GLOBAL_STORAGE_CLASS(vl_workflow_global_Overloading)
	vl_workflow_global::Overloading instance;
	INITIALIZE_GLOBAL_STORAGE_CLASS

		instance.s = ::vl::WString::Unmanaged(L"");
	FINALIZE_GLOBAL_STORAGE_CLASS

		instance.s = ::vl::WString::Empty;
END_GLOBAL_STORAGE_CLASS(vl_workflow_global_Overloading)

namespace vl_workflow_global
{
/***********************************************************************
Global Functions
***********************************************************************/

	::vl::WString Overloading::main()
	{
		(GLOBAL_NAME s = ((((::vl::WString::Unmanaged(L"") + GLOBAL_NAME s) + ::vl::WString::Unmanaged(L"[")) + ::vl::__vwsn::ToString(::vl::reflection::description::Math::Max(static_cast<::vl::vint8_t>(static_cast<::vl::vint>(1)), static_cast<::vl::vint8_t>(static_cast<::vl::vint>(2))))) + ::vl::WString::Unmanaged(L"]")));
		(GLOBAL_NAME s = ((((::vl::WString::Unmanaged(L"") + GLOBAL_NAME s) + ::vl::WString::Unmanaged(L"[")) + ::vl::__vwsn::ToString(::vl::reflection::description::Math::Max(static_cast<::vl::vint16_t>(static_cast<::vl::vint>(1)), static_cast<::vl::vint16_t>(static_cast<::vl::vint>(2))))) + ::vl::WString::Unmanaged(L"]")));
		(GLOBAL_NAME s = ((((::vl::WString::Unmanaged(L"") + GLOBAL_NAME s) + ::vl::WString::Unmanaged(L"[")) + ::vl::__vwsn::ToString(::vl::reflection::description::Math::Max(static_cast<::vl::vint32_t>(static_cast<::vl::vint>(1)), static_cast<::vl::vint32_t>(static_cast<::vl::vint>(2))))) + ::vl::WString::Unmanaged(L"]")));
		(GLOBAL_NAME s = ((((::vl::WString::Unmanaged(L"") + GLOBAL_NAME s) + ::vl::WString::Unmanaged(L"[")) + ::vl::__vwsn::ToString(::vl::reflection::description::Math::Max(static_cast<::vl::vint64_t>(static_cast<::vl::vint>(1)), static_cast<::vl::vint64_t>(static_cast<::vl::vint>(2))))) + ::vl::WString::Unmanaged(L"]")));
		(GLOBAL_NAME s = ((((::vl::WString::Unmanaged(L"") + GLOBAL_NAME s) + ::vl::WString::Unmanaged(L"[")) + ::vl::__vwsn::ToString(::vl::reflection::description::Math::Max(static_cast<::vl::vuint8_t>(static_cast<::vl::vint>(1)), static_cast<::vl::vuint8_t>(static_cast<::vl::vint>(2))))) + ::vl::WString::Unmanaged(L"]")));
		(GLOBAL_NAME s = ((((::vl::WString::Unmanaged(L"") + GLOBAL_NAME s) + ::vl::WString::Unmanaged(L"[")) + ::vl::__vwsn::ToString(::vl::reflection::description::Math::Max(static_cast<::vl::vuint16_t>(static_cast<::vl::vint>(1)), static_cast<::vl::vuint16_t>(static_cast<::vl::vint>(2))))) + ::vl::WString::Unmanaged(L"]")));
		(GLOBAL_NAME s = ((((::vl::WString::Unmanaged(L"") + GLOBAL_NAME s) + ::vl::WString::Unmanaged(L"[")) + ::vl::__vwsn::ToString(::vl::reflection::description::Math::Max(static_cast<::vl::vuint32_t>(static_cast<::vl::vint>(1)), static_cast<::vl::vuint32_t>(static_cast<::vl::vint>(2))))) + ::vl::WString::Unmanaged(L"]")));
		(GLOBAL_NAME s = ((((::vl::WString::Unmanaged(L"") + GLOBAL_NAME s) + ::vl::WString::Unmanaged(L"[")) + ::vl::__vwsn::ToString(::vl::reflection::description::Math::Max(static_cast<::vl::vuint64_t>(static_cast<::vl::vint>(1)), static_cast<::vl::vuint64_t>(static_cast<::vl::vint>(2))))) + ::vl::WString::Unmanaged(L"]")));
		(GLOBAL_NAME s = ((((::vl::WString::Unmanaged(L"") + GLOBAL_NAME s) + ::vl::WString::Unmanaged(L"[")) + ::vl::__vwsn::ToString(::vl::reflection::description::Math::Max(static_cast<float>(static_cast<::vl::vint>(1)), static_cast<float>(static_cast<::vl::vint>(2))))) + ::vl::WString::Unmanaged(L"]")));
		(GLOBAL_NAME s = ((((::vl::WString::Unmanaged(L"") + GLOBAL_NAME s) + ::vl::WString::Unmanaged(L"[")) + ::vl::__vwsn::ToString(::vl::reflection::description::Math::Max(static_cast<double>(static_cast<::vl::vint>(1)), static_cast<double>(static_cast<::vl::vint>(2))))) + ::vl::WString::Unmanaged(L"]")));
		return GLOBAL_NAME s;
	}

	Overloading& Overloading::Instance()
	{
		return Getvl_workflow_global_Overloading().instance;
	}
}

#undef GLOBAL_SYMBOL
#undef GLOBAL_NAME
#undef GLOBAL_OBJ

#if defined( _MSC_VER)
#pragma warning(pop)
#elif defined(__GNUC__)
#pragma GCC diagnostic pop
#elif defined(__clang__)
#pragma clang diagnostic pop
#endif
