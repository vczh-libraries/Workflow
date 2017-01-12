/***********************************************************************
Generated from ../Resources/Codegen/ClassMethod.txt
***********************************************************************/

#include "ClassMethod.h"

/***********************************************************************
Global Variables and Functions
***********************************************************************/

BEGIN_GLOBAL_STORAGE_CLASS(vl_workflow_global_ClassMethod)
	vl_workflow_global::ClassMethod instance;
	INITIALIZE_GLOBAL_STORAGE_CLASS

		instance.s = ::vl::WString(L"", false);
	FINALIZE_GLOBAL_STORAGE_CLASS
END_GLOBAL_STORAGE_CLASS(vl_workflow_global_ClassMethod)

namespace vl_workflow_global
{
	::vl::WString ClassMethod::main()
	{
		throw 0;
	}

	ClassMethod& ClassMethod::Instance()
	{
		return Getvl_workflow_global_ClassMethod().instance;
	}

	struct __vwsnf1_ClassMethod_main_
	{
		::vl::Ptr<::Methods> a;

		__vwsnf1_ClassMethod_main_(::vl::Ptr<::Methods> __vwsnctor_a);

		void operator()() const;
	};

	__vwsnf1_ClassMethod_main_::__vwsnf1_ClassMethod_main_(::vl::Ptr<::Methods> __vwsnctor_a)
		:a(__vwsnctor_a)
	{
	}

	void __vwsnf1_ClassMethod_main_::operator()() const
	{
		throw 0;
	}
}

/***********************************************************************
Class (::Methods)
***********************************************************************/

Methods::Methods()
{
	throw 0;
}

Methods::Methods(::vl::vint32_t _x)
{
	throw 0;
}

::vl::vint32_t Methods::GetX()
{
	throw 0;
}

void Methods::SetX(::vl::vint32_t _x)
{
	throw 0;
}

