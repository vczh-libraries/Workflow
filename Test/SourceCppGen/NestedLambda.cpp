/***********************************************************************
Generated from ../Resources/Codegen/NestedLambda.txt
***********************************************************************/

#include "NestedLambda.h"

/***********************************************************************
Global Variables and Functions
***********************************************************************/

BEGIN_GLOBAL_STORAGE_CLASS(vl_workflow_global_NestedLambda)
	vl_workflow_global::NestedLambda instance;
	INITIALIZE_GLOBAL_STORAGE_CLASS
	FINALIZE_GLOBAL_STORAGE_CLASS
END_GLOBAL_STORAGE_CLASS(vl_workflow_global_NestedLambda)

namespace vl_workflow_global
{
	::vl::WString NestedLambda::main()
	{
		throw 0;
	}

	NestedLambda& NestedLambda::Instance()
	{
		return Getvl_workflow_global_NestedLambda().instance;
	}










	class __vwsnc1_NestedLambda_main__IA : public ::vl::Object, public virtual ::IA
	{
	public:
		::vl::vint32_t a = 0;
		::vl::vint32_t GetA() override;
		void SetA(::vl::vint32_t value) override;
		::vl::WString H();
		::vl::WString Calculate() override;
	};

	class __vwsnc2_NestedLambda_main_H__IB : public ::vl::Object, public virtual ::IB
	{
	public:
		::vl::vint32_t b = 0;
		::vl::vint32_t GetB() override;
		void SetB(::vl::vint32_t value) override;
		::vl::WString H();
		::vl::WString Calculate() override;
	};












	::vl::vint32_t __vwsnc1_NestedLambda_main__IA::GetA()
	{
		throw 0;
	}

	void __vwsnc1_NestedLambda_main__IA::SetA(::vl::vint32_t value)
	{
		throw 0;
	}

	::vl::WString __vwsnc1_NestedLambda_main__IA::H()
	{
		throw 0;
	}

	::vl::WString __vwsnc1_NestedLambda_main__IA::Calculate()
	{
		throw 0;
	}



	::vl::vint32_t __vwsnc2_NestedLambda_main_H__IB::GetB()
	{
		throw 0;
	}

	void __vwsnc2_NestedLambda_main_H__IB::SetB(::vl::vint32_t value)
	{
		throw 0;
	}

	::vl::WString __vwsnc2_NestedLambda_main_H__IB::H()
	{
		throw 0;
	}

	::vl::WString __vwsnc2_NestedLambda_main_H__IB::Calculate()
	{
		throw 0;
	}
}

/***********************************************************************
Class (::IA)
***********************************************************************/

/***********************************************************************
Class (::IB)
***********************************************************************/

