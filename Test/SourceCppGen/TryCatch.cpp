/***********************************************************************
Generated from ../Resources/Codegen/TryCatch.txt
***********************************************************************/

#include "TryCatch.h"

/***********************************************************************
Global Variables and Functions
***********************************************************************/

BEGIN_GLOBAL_STORAGE_CLASS(vl_workflow_global_TryCatch)
	vl_workflow_global::TryCatch instance;
	INITIALIZE_GLOBAL_STORAGE_CLASS

		instance.log = ::vl::WString(L"", false);
	FINALIZE_GLOBAL_STORAGE_CLASS
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
				(::vl_workflow_global::TryCatch::Instance().log = (((::vl_workflow_global::TryCatch::Instance().log + ::vl::WString(L"[", false)) + ::vl::__vwsn::This(ex)->GetMessage()) + ::vl::WString(L"]", false)));
			}
		}
		catch(const ::vl::Error& __vwsne_0)
		{
			auto ex = ::vl::reflection::description::IValueException::Create(__vwsne_0.Description());
			{
				(::vl_workflow_global::TryCatch::Instance().log = (((::vl_workflow_global::TryCatch::Instance().log + ::vl::WString(L"[", false)) + ::vl::__vwsn::This(ex)->GetMessage()) + ::vl::WString(L"]", false)));
			}
		}
	}

	void TryCatch::Test2()
	{
		{
			auto __vwsnb_0 = [&]()
			{
				(::vl_workflow_global::TryCatch::Instance().log = (::vl_workflow_global::TryCatch::Instance().log + ::vl::WString(L"[Test2::finally]", false)));
			}
			;
			try
			{
				{
					throw ::vl::Exception(::vl::WString(L"Test2::catch", false));
				}
				__vwsnb_0();
			}
			catch(const ::vl::Exception& __vwsne_0)
			{
				auto ex = ::vl::reflection::description::IValueException::Create(__vwsne_0.Message());
				{
					(::vl_workflow_global::TryCatch::Instance().log = (((::vl_workflow_global::TryCatch::Instance().log + ::vl::WString(L"[", false)) + ::vl::__vwsn::This(ex)->GetMessage()) + ::vl::WString(L"]", false)));
				}
				__vwsnb_0();
			}
			catch(const ::vl::Error& __vwsne_0)
			{
				auto ex = ::vl::reflection::description::IValueException::Create(__vwsne_0.Description());
				{
					(::vl_workflow_global::TryCatch::Instance().log = (((::vl_workflow_global::TryCatch::Instance().log + ::vl::WString(L"[", false)) + ::vl::__vwsn::This(ex)->GetMessage()) + ::vl::WString(L"]", false)));
				}
				__vwsnb_0();
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
				(::vl_workflow_global::TryCatch::Instance().log = (::vl_workflow_global::TryCatch::Instance().log + ::vl::WString(L"[Test3::finally2]", false)));
			}
			;
			try
			{
				{
					{
						auto __vwsnb_1 = [&]()
						{
							(::vl_workflow_global::TryCatch::Instance().log = (::vl_workflow_global::TryCatch::Instance().log + ::vl::WString(L"[Test3::finally1]", false)));
						}
						;
						try
						{
							{
								::vl_workflow_global::TryCatch::Instance().Throw(::vl::WString(L"Test3::catch", false));
							}
							__vwsnb_1();
						}
						catch(const ::vl::Exception& __vwsne_1)
						{
							auto ex = ::vl::reflection::description::IValueException::Create(__vwsne_1.Message());
							{
								(::vl_workflow_global::TryCatch::Instance().log = (((::vl_workflow_global::TryCatch::Instance().log + ::vl::WString(L"[", false)) + ::vl::__vwsn::This(ex)->GetMessage()) + ::vl::WString(L"1]", false)));
								throw;
							}
							__vwsnb_1();
						}
						catch(const ::vl::Error& __vwsne_1)
						{
							auto ex = ::vl::reflection::description::IValueException::Create(__vwsne_1.Description());
							{
								(::vl_workflow_global::TryCatch::Instance().log = (((::vl_workflow_global::TryCatch::Instance().log + ::vl::WString(L"[", false)) + ::vl::__vwsn::This(ex)->GetMessage()) + ::vl::WString(L"1]", false)));
								throw;
							}
							__vwsnb_1();
						}
					}
				}
				__vwsnb_0();
			}
			catch(const ::vl::Exception& __vwsne_0)
			{
				auto ex = ::vl::reflection::description::IValueException::Create(__vwsne_0.Message());
				{
					(::vl_workflow_global::TryCatch::Instance().log = (((::vl_workflow_global::TryCatch::Instance().log + ::vl::WString(L"[", false)) + ::vl::__vwsn::This(ex)->GetMessage()) + ::vl::WString(L"2]", false)));
				}
				__vwsnb_0();
			}
			catch(const ::vl::Error& __vwsne_0)
			{
				auto ex = ::vl::reflection::description::IValueException::Create(__vwsne_0.Description());
				{
					(::vl_workflow_global::TryCatch::Instance().log = (((::vl_workflow_global::TryCatch::Instance().log + ::vl::WString(L"[", false)) + ::vl::__vwsn::This(ex)->GetMessage()) + ::vl::WString(L"2]", false)));
				}
				__vwsnb_0();
			}
		}
	}

	void TryCatch::Test4()
	{
		{
			auto __vwsnb_0 = [&]()
			{
				(::vl_workflow_global::TryCatch::Instance().log = (::vl_workflow_global::TryCatch::Instance().log + ::vl::WString(L"[Test4::finally2]", false)));
			}
			;
			try
			{
				{
					{
						auto __vwsnb_1 = [&]()
						{
							(::vl_workflow_global::TryCatch::Instance().log = (::vl_workflow_global::TryCatch::Instance().log + ::vl::WString(L"[Test4::finally1]", false)));
						}
						;
						try
						{
							{
							}
							__vwsnb_1();
						}
						catch(const ::vl::Exception& __vwsne_1)
						{
							auto ex = ::vl::reflection::description::IValueException::Create(__vwsne_1.Message());
							{
								(::vl_workflow_global::TryCatch::Instance().log = (((::vl_workflow_global::TryCatch::Instance().log + ::vl::WString(L"[", false)) + ::vl::__vwsn::This(ex)->GetMessage()) + ::vl::WString(L"1]", false)));
								throw;
							}
							__vwsnb_1();
						}
						catch(const ::vl::Error& __vwsne_1)
						{
							auto ex = ::vl::reflection::description::IValueException::Create(__vwsne_1.Description());
							{
								(::vl_workflow_global::TryCatch::Instance().log = (((::vl_workflow_global::TryCatch::Instance().log + ::vl::WString(L"[", false)) + ::vl::__vwsn::This(ex)->GetMessage()) + ::vl::WString(L"1]", false)));
								throw;
							}
							__vwsnb_1();
						}
					}
				}
				__vwsnb_0();
			}
			catch(const ::vl::Exception& __vwsne_0)
			{
				auto ex = ::vl::reflection::description::IValueException::Create(__vwsne_0.Message());
				{
					(::vl_workflow_global::TryCatch::Instance().log = (((::vl_workflow_global::TryCatch::Instance().log + ::vl::WString(L"[", false)) + ::vl::__vwsn::This(ex)->GetMessage()) + ::vl::WString(L"2]", false)));
				}
				__vwsnb_0();
			}
			catch(const ::vl::Error& __vwsne_0)
			{
				auto ex = ::vl::reflection::description::IValueException::Create(__vwsne_0.Description());
				{
					(::vl_workflow_global::TryCatch::Instance().log = (((::vl_workflow_global::TryCatch::Instance().log + ::vl::WString(L"[", false)) + ::vl::__vwsn::This(ex)->GetMessage()) + ::vl::WString(L"2]", false)));
				}
				__vwsnb_0();
			}
		}
	}

	::vl::WString TryCatch::main()
	{
		::vl_workflow_global::TryCatch::Instance().Test1();
		::vl_workflow_global::TryCatch::Instance().Test2();
		::vl_workflow_global::TryCatch::Instance().Test3();
		::vl_workflow_global::TryCatch::Instance().Test4();
		return ::vl_workflow_global::TryCatch::Instance().log;
	}

	TryCatch& TryCatch::Instance()
	{
		return Getvl_workflow_global_TryCatch().instance;
	}
}

