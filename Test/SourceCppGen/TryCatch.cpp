/***********************************************************************
Generated from ../Resources/Codegen/TryCatch.txt
***********************************************************************/

#include "TryCatch.h"

#if defined(__GNUC__)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wparentheses-equality"
#elif defined(__clang__)
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wparentheses-equality"
#endif
#define GLOBAL_SYMBOL ::vl_workflow_global::TryCatch::
#define GLOBAL_NAME ::vl_workflow_global::TryCatch::Instance().
#define GLOBAL_OBJ &::vl_workflow_global::TryCatch::Instance()

/***********************************************************************
Global Variables and Functions
***********************************************************************/

BEGIN_GLOBAL_STORAGE_CLASS(vl_workflow_global_TryCatch)
	vl_workflow_global::TryCatch instance;
	INITIALIZE_GLOBAL_STORAGE_CLASS

		instance.log = ::vl::WString(L"", false);
	FINALIZE_GLOBAL_STORAGE_CLASS

		instance.log = ::vl::WString::Empty;
END_GLOBAL_STORAGE_CLASS(vl_workflow_global_TryCatch)

namespace vl_workflow_global
{
	void TryCatch::Test1()
	{
		try
		{
			{
				throw ::vl::Exception(::vl::WString(L"Test1::catch", false));
			}
		}
		catch(const ::vl::Exception& __vwsne_0)
		{
			auto ex = ::vl::reflection::description::IValueException::Create(__vwsne_0.Message());
			{
				(GLOBAL_NAME log = (((GLOBAL_NAME log + ::vl::WString(L"[", false)) + ::vl::__vwsn::This(ex.Obj())->GetMessage()) + ::vl::WString(L"]", false)));
			}
		}
		catch(const ::vl::Error& __vwsne_0)
		{
			auto ex = ::vl::reflection::description::IValueException::Create(__vwsne_0.Description());
			{
				(GLOBAL_NAME log = (((GLOBAL_NAME log + ::vl::WString(L"[", false)) + ::vl::__vwsn::This(ex.Obj())->GetMessage()) + ::vl::WString(L"]", false)));
			}
		}
	}

	void TryCatch::Test2()
	{
		{
			auto __vwsnb_0 = [&]()
			{
				(GLOBAL_NAME log = (GLOBAL_NAME log + ::vl::WString(L"[Test2::finally]", false)));
			}
			;
			::vl::__vwsn::RunOnExit<::vl::RemoveCVR<decltype(__vwsnb_0)>::Type> __vwsnb_0_dtor(&__vwsnb_0);
			try
			{
				{
					throw ::vl::Exception(::vl::WString(L"Test2::catch", false));
				}
			}
			catch(const ::vl::Exception& __vwsne_0)
			{
				auto ex = ::vl::reflection::description::IValueException::Create(__vwsne_0.Message());
				{
					(GLOBAL_NAME log = (((GLOBAL_NAME log + ::vl::WString(L"[", false)) + ::vl::__vwsn::This(ex.Obj())->GetMessage()) + ::vl::WString(L"]", false)));
				}
			}
			catch(const ::vl::Error& __vwsne_0)
			{
				auto ex = ::vl::reflection::description::IValueException::Create(__vwsne_0.Description());
				{
					(GLOBAL_NAME log = (((GLOBAL_NAME log + ::vl::WString(L"[", false)) + ::vl::__vwsn::This(ex.Obj())->GetMessage()) + ::vl::WString(L"]", false)));
				}
			}
		}
	}

	void TryCatch::Throw(const ::vl::WString& message)
	{
		throw ::vl::Exception(message);
	}

	void TryCatch::Test3()
	{
		{
			auto __vwsnb_0 = [&]()
			{
				(GLOBAL_NAME log = (GLOBAL_NAME log + ::vl::WString(L"[Test3::finally2]", false)));
			}
			;
			::vl::__vwsn::RunOnExit<::vl::RemoveCVR<decltype(__vwsnb_0)>::Type> __vwsnb_0_dtor(&__vwsnb_0);
			try
			{
				{
					{
						auto __vwsnb_1 = [&]()
						{
							(GLOBAL_NAME log = (GLOBAL_NAME log + ::vl::WString(L"[Test3::finally1]", false)));
						}
						;
						::vl::__vwsn::RunOnExit<::vl::RemoveCVR<decltype(__vwsnb_1)>::Type> __vwsnb_1_dtor(&__vwsnb_1);
						try
						{
							{
								GLOBAL_NAME Throw(::vl::WString(L"Test3::catch", false));
							}
						}
						catch(const ::vl::Exception& __vwsne_1)
						{
							auto ex = ::vl::reflection::description::IValueException::Create(__vwsne_1.Message());
							{
								(GLOBAL_NAME log = (((GLOBAL_NAME log + ::vl::WString(L"[", false)) + ::vl::__vwsn::This(ex.Obj())->GetMessage()) + ::vl::WString(L"1]", false)));
								throw;
							}
						}
						catch(const ::vl::Error& __vwsne_1)
						{
							auto ex = ::vl::reflection::description::IValueException::Create(__vwsne_1.Description());
							{
								(GLOBAL_NAME log = (((GLOBAL_NAME log + ::vl::WString(L"[", false)) + ::vl::__vwsn::This(ex.Obj())->GetMessage()) + ::vl::WString(L"1]", false)));
								throw;
							}
						}
					}
				}
			}
			catch(const ::vl::Exception& __vwsne_0)
			{
				auto ex = ::vl::reflection::description::IValueException::Create(__vwsne_0.Message());
				{
					(GLOBAL_NAME log = (((GLOBAL_NAME log + ::vl::WString(L"[", false)) + ::vl::__vwsn::This(ex.Obj())->GetMessage()) + ::vl::WString(L"2]", false)));
				}
			}
			catch(const ::vl::Error& __vwsne_0)
			{
				auto ex = ::vl::reflection::description::IValueException::Create(__vwsne_0.Description());
				{
					(GLOBAL_NAME log = (((GLOBAL_NAME log + ::vl::WString(L"[", false)) + ::vl::__vwsn::This(ex.Obj())->GetMessage()) + ::vl::WString(L"2]", false)));
				}
			}
		}
	}

	void TryCatch::Test4()
	{
		{
			auto __vwsnb_0 = [&]()
			{
				(GLOBAL_NAME log = (GLOBAL_NAME log + ::vl::WString(L"[Test4::finally2]", false)));
			}
			;
			::vl::__vwsn::RunOnExit<::vl::RemoveCVR<decltype(__vwsnb_0)>::Type> __vwsnb_0_dtor(&__vwsnb_0);
			try
			{
				{
					{
						auto __vwsnb_1 = [&]()
						{
							(GLOBAL_NAME log = (GLOBAL_NAME log + ::vl::WString(L"[Test4::finally1]", false)));
						}
						;
						::vl::__vwsn::RunOnExit<::vl::RemoveCVR<decltype(__vwsnb_1)>::Type> __vwsnb_1_dtor(&__vwsnb_1);
						try
						{
							{
							}
						}
						catch(const ::vl::Exception& __vwsne_1)
						{
							auto ex = ::vl::reflection::description::IValueException::Create(__vwsne_1.Message());
							{
								(GLOBAL_NAME log = (((GLOBAL_NAME log + ::vl::WString(L"[", false)) + ::vl::__vwsn::This(ex.Obj())->GetMessage()) + ::vl::WString(L"1]", false)));
								throw;
							}
						}
						catch(const ::vl::Error& __vwsne_1)
						{
							auto ex = ::vl::reflection::description::IValueException::Create(__vwsne_1.Description());
							{
								(GLOBAL_NAME log = (((GLOBAL_NAME log + ::vl::WString(L"[", false)) + ::vl::__vwsn::This(ex.Obj())->GetMessage()) + ::vl::WString(L"1]", false)));
								throw;
							}
						}
					}
				}
			}
			catch(const ::vl::Exception& __vwsne_0)
			{
				auto ex = ::vl::reflection::description::IValueException::Create(__vwsne_0.Message());
				{
					(GLOBAL_NAME log = (((GLOBAL_NAME log + ::vl::WString(L"[", false)) + ::vl::__vwsn::This(ex.Obj())->GetMessage()) + ::vl::WString(L"2]", false)));
				}
			}
			catch(const ::vl::Error& __vwsne_0)
			{
				auto ex = ::vl::reflection::description::IValueException::Create(__vwsne_0.Description());
				{
					(GLOBAL_NAME log = (((GLOBAL_NAME log + ::vl::WString(L"[", false)) + ::vl::__vwsn::This(ex.Obj())->GetMessage()) + ::vl::WString(L"2]", false)));
				}
			}
		}
	}

	::vl::WString TryCatch::main()
	{
		GLOBAL_NAME Test1();
		GLOBAL_NAME Test2();
		GLOBAL_NAME Test3();
		GLOBAL_NAME Test4();
		return GLOBAL_NAME log;
	}

	TryCatch& TryCatch::Instance()
	{
		return Getvl_workflow_global_TryCatch().instance;
	}
}

#undef GLOBAL_SYMBOL
#undef GLOBAL_NAME
#undef GLOBAL_OBJ

#if defined(__GNUC__)
#pragma GCC diagnostic pop
#elif defined(__clang__)
#pragma clang diagnostic pop
#endif
