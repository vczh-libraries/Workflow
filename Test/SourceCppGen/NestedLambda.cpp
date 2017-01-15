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
	struct __vwsnf3_NestedLambda_main_H_Calculate_
	{
		::IB* __vwsnthis_0;
		::IA* __vwsnthis_1;

		__vwsnf3_NestedLambda_main_H_Calculate_(::IB* __vwsnctorthis_0, ::IA* __vwsnctorthis_1);

		::vl::vint32_t operator()() const;
	};

	struct __vwsnf4_NestedLambda_main_H_Calculate_
	{
		::IB* __vwsnthis_0;
		::IA* __vwsnthis_1;

		__vwsnf4_NestedLambda_main_H_Calculate_(::IB* __vwsnctorthis_0, ::IA* __vwsnctorthis_1);

		::vl::vint32_t operator()() const;
	};

	struct __vwsnf7_NestedLambda_main_Calculate_
	{
		::IA* __vwsnthis_0;

		__vwsnf7_NestedLambda_main_Calculate_(::IA* __vwsnctorthis_0);

		::vl::vint32_t operator()() const;
	};

	struct __vwsnf8_NestedLambda_main_Calculate_
	{
		::IA* __vwsnthis_0;

		__vwsnf8_NestedLambda_main_Calculate_(::IA* __vwsnctorthis_0);

		::vl::vint32_t operator()() const;
	};

	struct __vwsno1_NestedLambda_main_H_
	{
		::IA* __vwsnthis_0;

		__vwsno1_NestedLambda_main_H_(::IA* __vwsnctorthis_0);

		::vl::WString operator()() const;
	};

	struct __vwsno2_NestedLambda_main_H_Calculate_
	{
		::IB* __vwsnthis_0;
		::IA* __vwsnthis_1;

		__vwsno2_NestedLambda_main_H_Calculate_(::IB* __vwsnctorthis_0, ::IA* __vwsnctorthis_1);

		::vl::vint32_t operator()() const;
	};

	struct __vwsno5_NestedLambda_main_H_Calculate__
	{
		::IB* __vwsnthis_0;
		::IA* __vwsnthis_1;

		__vwsno5_NestedLambda_main_H_Calculate__(::IB* __vwsnctorthis_0, ::IA* __vwsnctorthis_1);

		::vl::vint32_t operator()() const;
	};

	struct __vwsno6_NestedLambda_main_Calculate_
	{
		::IA* __vwsnthis_0;

		__vwsno6_NestedLambda_main_Calculate_(::IA* __vwsnctorthis_0);

		::vl::vint32_t operator()() const;
	};

	struct __vwsno9_NestedLambda_main_Calculate__
	{
		::IA* __vwsnthis_0;

		__vwsno9_NestedLambda_main_Calculate__(::IA* __vwsnctorthis_0);

		::vl::vint32_t operator()() const;
	};

	class __vwsnc1_NestedLambda_main__IA : public ::vl::Object, public virtual ::IA
	{
	public:

		__vwsnc1_NestedLambda_main__IA();

		::vl::vint32_t a = 0;
		::vl::vint32_t GetA() override;
		void SetA(::vl::vint32_t value) override;
		::vl::WString H();
		::vl::WString Calculate() override;
	};

	class __vwsnc2_NestedLambda_main_H__IB : public ::vl::Object, public virtual ::IB
	{
	public:
		::IA* __vwsnthis_0;

		__vwsnc2_NestedLambda_main_H__IB(::IA* __vwsnctorthis_0);

		::vl::vint32_t b = 0;
		::vl::vint32_t GetB() override;
		void SetB(::vl::vint32_t value) override;
		::vl::WString H();
		::vl::WString Calculate() override;
	};

	::vl::WString NestedLambda::main()
	{
		return ::vl::__vwsn::This(::vl::Ptr<::IA>(new ::vl_workflow_global::__vwsnc1_NestedLambda_main__IA()).Obj())->Calculate();
	}

	NestedLambda& NestedLambda::Instance()
	{
		return Getvl_workflow_global_NestedLambda().instance;
	}

	__vwsnf3_NestedLambda_main_H_Calculate_::__vwsnf3_NestedLambda_main_H_Calculate_(::IB* __vwsnctorthis_0, ::IA* __vwsnctorthis_1)
		:__vwsnthis_0(__vwsnctorthis_0)
		, __vwsnthis_1(__vwsnctorthis_1)
	{
	}

	::vl::vint32_t __vwsnf3_NestedLambda_main_H_Calculate_::operator()() const
	{
		return ::vl::__vwsn::This(__vwsnthis_1)->GetA();
	}

	__vwsnf4_NestedLambda_main_H_Calculate_::__vwsnf4_NestedLambda_main_H_Calculate_(::IB* __vwsnctorthis_0, ::IA* __vwsnctorthis_1)
		:__vwsnthis_0(__vwsnctorthis_0)
		, __vwsnthis_1(__vwsnctorthis_1)
	{
	}

	::vl::vint32_t __vwsnf4_NestedLambda_main_H_Calculate_::operator()() const
	{
		return (LAMBDA(::vl_workflow_global::__vwsno5_NestedLambda_main_H_Calculate__(__vwsnthis_0, __vwsnthis_1))() + ::vl::__vwsn::This(__vwsnthis_0)->GetB());
	}

	__vwsnf7_NestedLambda_main_Calculate_::__vwsnf7_NestedLambda_main_Calculate_(::IA* __vwsnctorthis_0)
		:__vwsnthis_0(__vwsnctorthis_0)
	{
	}

	::vl::vint32_t __vwsnf7_NestedLambda_main_Calculate_::operator()() const
	{
		return ::vl::__vwsn::This(__vwsnthis_0)->GetA();
	}

	__vwsnf8_NestedLambda_main_Calculate_::__vwsnf8_NestedLambda_main_Calculate_(::IA* __vwsnctorthis_0)
		:__vwsnthis_0(__vwsnctorthis_0)
	{
	}

	::vl::vint32_t __vwsnf8_NestedLambda_main_Calculate_::operator()() const
	{
		return LAMBDA(::vl_workflow_global::__vwsno9_NestedLambda_main_Calculate__(__vwsnthis_0))();
	}

	__vwsno1_NestedLambda_main_H_::__vwsno1_NestedLambda_main_H_(::IA* __vwsnctorthis_0)
		:__vwsnthis_0(__vwsnctorthis_0)
	{
	}

	::vl::WString __vwsno1_NestedLambda_main_H_::operator()() const
	{
		return ::vl::__vwsn::This(::vl::Ptr<::IB>(new ::vl_workflow_global::__vwsnc2_NestedLambda_main_H__IB(__vwsnthis_0)).Obj())->Calculate();
	}

	__vwsno2_NestedLambda_main_H_Calculate_::__vwsno2_NestedLambda_main_H_Calculate_(::IB* __vwsnctorthis_0, ::IA* __vwsnctorthis_1)
		:__vwsnthis_0(__vwsnctorthis_0)
		, __vwsnthis_1(__vwsnctorthis_1)
	{
	}

	::vl::vint32_t __vwsno2_NestedLambda_main_H_Calculate_::operator()() const
	{
		return (LAMBDA(::vl_workflow_global::__vwsnf3_NestedLambda_main_H_Calculate_(__vwsnthis_0, __vwsnthis_1))() + ::vl::__vwsn::This(__vwsnthis_0)->GetB());
	}

	__vwsno5_NestedLambda_main_H_Calculate__::__vwsno5_NestedLambda_main_H_Calculate__(::IB* __vwsnctorthis_0, ::IA* __vwsnctorthis_1)
		:__vwsnthis_0(__vwsnctorthis_0)
		, __vwsnthis_1(__vwsnctorthis_1)
	{
	}

	::vl::vint32_t __vwsno5_NestedLambda_main_H_Calculate__::operator()() const
	{
		return ::vl::__vwsn::This(__vwsnthis_1)->GetA();
	}

	__vwsno6_NestedLambda_main_Calculate_::__vwsno6_NestedLambda_main_Calculate_(::IA* __vwsnctorthis_0)
		:__vwsnthis_0(__vwsnctorthis_0)
	{
	}

	::vl::vint32_t __vwsno6_NestedLambda_main_Calculate_::operator()() const
	{
		return LAMBDA(::vl_workflow_global::__vwsnf7_NestedLambda_main_Calculate_(__vwsnthis_0))();
	}

	__vwsno9_NestedLambda_main_Calculate__::__vwsno9_NestedLambda_main_Calculate__(::IA* __vwsnctorthis_0)
		:__vwsnthis_0(__vwsnctorthis_0)
	{
	}

	::vl::vint32_t __vwsno9_NestedLambda_main_Calculate__::operator()() const
	{
		return ::vl::__vwsn::This(__vwsnthis_0)->GetA();
	}

	__vwsnc1_NestedLambda_main__IA::__vwsnc1_NestedLambda_main__IA()
	{
		this->a = 0;
	}

	::vl::vint32_t __vwsnc1_NestedLambda_main__IA::GetA()
	{
		return a;
	}

	void __vwsnc1_NestedLambda_main__IA::SetA(::vl::vint32_t value)
	{
		(a = value);
		::vl::__vwsn::EventInvoke(::vl::__vwsn::This(this)->AChanged)();
	}

	::vl::WString __vwsnc1_NestedLambda_main__IA::H()
	{
		return LAMBDA(::vl_workflow_global::__vwsno1_NestedLambda_main_H_(this))();
	}

	::vl::WString __vwsnc1_NestedLambda_main__IA::Calculate()
	{
		auto f = LAMBDA(::vl_workflow_global::__vwsno6_NestedLambda_main_Calculate_(this));
		auto g = LAMBDA(::vl_workflow_global::__vwsnf8_NestedLambda_main_Calculate_(this));
		auto h = ::vl::Func<::vl::WString()>(::vl::__vwsn::This(this), &__vwsnc1_NestedLambda_main__IA::H);
		auto s = ::vl::WString(L"", false);
		(a = 1);
		(s = (s + ((::vl::WString(L"[", false) + ::vl::__vwsn::ToString(f())) + ::vl::WString(L"]", false))));
		::vl::__vwsn::This(this)->SetA(2);
		(s = (s + ((::vl::WString(L"[", false) + ::vl::__vwsn::ToString(g())) + ::vl::WString(L"]", false))));
		::vl::__vwsn::This(this)->SetA((::vl::__vwsn::This(this)->GetA() + 1));
		(s = (s + ((::vl::WString(L"[", false) + h()) + ::vl::WString(L"]", false))));
		return s;
	}

	__vwsnc2_NestedLambda_main_H__IB::__vwsnc2_NestedLambda_main_H__IB(::IA* __vwsnctorthis_0)
		:__vwsnthis_0(__vwsnctorthis_0)
	{
		this->b = 0;
	}

	::vl::vint32_t __vwsnc2_NestedLambda_main_H__IB::GetB()
	{
		return b;
	}

	void __vwsnc2_NestedLambda_main_H__IB::SetB(::vl::vint32_t value)
	{
		(b = value);
		::vl::__vwsn::EventInvoke(::vl::__vwsn::This(this)->BChanged)();
	}

	::vl::WString __vwsnc2_NestedLambda_main_H__IB::H()
	{
		return ((((::vl::WString(L"[", false) + ::vl::__vwsn::ToString(::vl::__vwsn::This(__vwsnthis_0)->GetA())) + ::vl::WString(L", ", false)) + ::vl::__vwsn::ToString(::vl::__vwsn::This(this)->GetB())) + ::vl::WString(L"]", false));
	}

	::vl::WString __vwsnc2_NestedLambda_main_H__IB::Calculate()
	{
		auto f = LAMBDA(::vl_workflow_global::__vwsno2_NestedLambda_main_H_Calculate_(this, __vwsnthis_0));
		auto g = LAMBDA(::vl_workflow_global::__vwsnf4_NestedLambda_main_H_Calculate_(this, __vwsnthis_0));
		auto h = ::vl::Func<::vl::WString()>(::vl::__vwsn::This(this), &__vwsnc2_NestedLambda_main_H__IB::H);
		auto s = ::vl::WString(L"", false);
		(b = 1);
		(s = (s + ((::vl::WString(L"[", false) + ::vl::__vwsn::ToString(f())) + ::vl::WString(L"]", false))));
		::vl::__vwsn::This(this)->SetB(2);
		(s = (s + ((::vl::WString(L"[", false) + ::vl::__vwsn::ToString(g())) + ::vl::WString(L"]", false))));
		::vl::__vwsn::This(this)->SetB((::vl::__vwsn::This(this)->GetB() + 10));
		(s = (s + ((::vl::WString(L"[", false) + h()) + ::vl::WString(L"]", false))));
		return s;
	}

}

/***********************************************************************
Class (::IA)
***********************************************************************/

/***********************************************************************
Class (::IB)
***********************************************************************/

