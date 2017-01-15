/***********************************************************************
Generated from ../Resources/Codegen/OpNot.txt
***********************************************************************/

#include "OpNot.h"

/***********************************************************************
Global Variables and Functions
***********************************************************************/

BEGIN_GLOBAL_STORAGE_CLASS(vl_workflow_global_OpNot)
	vl_workflow_global::OpNot instance;
	INITIALIZE_GLOBAL_STORAGE_CLASS
	FINALIZE_GLOBAL_STORAGE_CLASS
END_GLOBAL_STORAGE_CLASS(vl_workflow_global_OpNot)

namespace vl_workflow_global
{
	::vl::WString OpNot::main()
	{
				bool b = false;
				::vl::vint8_t i8 = static_cast<::vl::vint32_t>(1);
				::vl::vint16_t i16 = static_cast<::vl::vint32_t>(2);
				::vl::vint32_t i32 = 4;
				::vl::vint64_t i64 = static_cast<::vl::vint32_t>(8);
				::vl::vuint8_t u8 = static_cast<::vl::vint32_t>(1);
				::vl::vuint16_t u16 = static_cast<::vl::vint32_t>(2);
				::vl::vuint32_t u32 = static_cast<::vl::vint32_t>(4);
				::vl::vuint64_t u64 = static_cast<::vl::vint32_t>(8);
				float f32 = static_cast<double>(1.5);
				double f64 = static_cast<::vl::vint32_t>(3);
				::vl::WString s = ::vl::WString(L"text", false);
				return ((((((((((((((((static_cast<bool>((! b)) + ::vl::WString(L", ", false)) + static_cast<::vl::vint8_t>((! i8))) + ::vl::WString(L", ", false)) + static_cast<::vl::vint16_t>((! i16))) + ::vl::WString(L", ", false)) + static_cast<::vl::vint32_t>((! i32))) + ::vl::WString(L", ", false)) + static_cast<::vl::vint64_t>((! i64))) + ::vl::WString(L", ", false)) + static_cast<::vl::vuint8_t>((! u8))) + ::vl::WString(L", ", false)) + static_cast<::vl::vuint16_t>((! u16))) + ::vl::WString(L", ", false)) + static_cast<::vl::vuint32_t>((! u32))) + ::vl::WString(L", ", false)) + static_cast<::vl::vuint64_t>((! u64)));
	}

	OpNot& OpNot::Instance()
	{
		return Getvl_workflow_global_OpNot().instance;
	}
}

