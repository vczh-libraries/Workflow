/***********************************************************************
Generated from ../Resources/Codegen/TryFinally.txt
***********************************************************************/

#include "TryFinally.h"

/***********************************************************************
Global Variables and Functions
***********************************************************************/

BEGIN_GLOBAL_STORAGE_CLASS(vl_workflow_global_TryFinally)
	vl_workflow_global::TryFinally instance;
	INITIALIZE_GLOBAL_STORAGE_CLASS

		instance.sum = 0;
	FINALIZE_GLOBAL_STORAGE_CLASS
END_GLOBAL_STORAGE_CLASS(vl_workflow_global_TryFinally)

namespace vl_workflow_global
{
	void TryFinally::test1()
	{
		using __vwsnt_0 = ::vl::vint32_t;
		FOREACH(__vwsnt_0, x, [&](::vl::vint32_t __vwsn_1, ::vl::vint32_t __vwsn_2){ return ::vl::collections::Range<::vl::vint32_t>(__vwsn_1, __vwsn_2 - __vwsn_1); }(1, 5 + 1))
		{
			{
				auto __vwsnb_0 = [&]()
				{
					(::vl_workflow_global::TryFinally::Instance().sum = (::vl_workflow_global::TryFinally::Instance().sum + 1));
				}
				;
				try
				{
					{
						continue;
					}
					__vwsnb_0();
				}
				catch(const ::vl::Exception&)
				{
					__vwsnb_0();
				}
				catch(const ::vl::Error&)
				{
					__vwsnb_0();
				}
			}
		}
	}

	void TryFinally::test2()
	{
		using __vwsnt_0 = ::vl::vint32_t;
		FOREACH(__vwsnt_0, x, [&](::vl::vint32_t __vwsn_1, ::vl::vint32_t __vwsn_2){ return ::vl::collections::Range<::vl::vint32_t>(__vwsn_1, __vwsn_2 - __vwsn_1); }(1, 5 + 1))
		{
			{
				auto __vwsnb_0 = [&]()
				{
					(::vl_workflow_global::TryFinally::Instance().sum = (::vl_workflow_global::TryFinally::Instance().sum + 10));
				}
				;
				try
				{
					{
						break;
					}
					__vwsnb_0();
				}
				catch(const ::vl::Exception&)
				{
					__vwsnb_0();
				}
				catch(const ::vl::Error&)
				{
					__vwsnb_0();
				}
			}
		}
	}

	void TryFinally::test3()
	{
		{
			auto __vwsnb_0 = [&]()
			{
				(::vl_workflow_global::TryFinally::Instance().sum = (::vl_workflow_global::TryFinally::Instance().sum + 100));
			}
			;
			try
			{
				{
					return;
				}
				__vwsnb_0();
			}
			catch(const ::vl::Exception&)
			{
				__vwsnb_0();
			}
			catch(const ::vl::Error&)
			{
				__vwsnb_0();
			}
		}
	}

	void TryFinally::test4()
	{
		{
			auto __vwsnb_0 = [&]()
			{
				(::vl_workflow_global::TryFinally::Instance().sum = (::vl_workflow_global::TryFinally::Instance().sum + 1000));
			}
			;
			try
			{
				{
					{
						auto __vwsnb_1 = [&]()
						{
							(::vl_workflow_global::TryFinally::Instance().sum = (::vl_workflow_global::TryFinally::Instance().sum + 1000));
						}
						;
						try
						{
							{
								{
									auto __vwsnb_2 = [&]()
									{
										(::vl_workflow_global::TryFinally::Instance().sum = (::vl_workflow_global::TryFinally::Instance().sum + 1000));
									}
									;
									try
									{
										{
											return;
										}
										__vwsnb_2();
									}
									catch(const ::vl::Exception&)
									{
										__vwsnb_2();
									}
									catch(const ::vl::Error&)
									{
										__vwsnb_2();
									}
								}
							}
							__vwsnb_1();
						}
						catch(const ::vl::Exception&)
						{
							__vwsnb_1();
						}
						catch(const ::vl::Error&)
						{
							__vwsnb_1();
						}
					}
				}
				__vwsnb_0();
			}
			catch(const ::vl::Exception&)
			{
				__vwsnb_0();
			}
			catch(const ::vl::Error&)
			{
				__vwsnb_0();
			}
		}
	}

	::vl::WString TryFinally::main()
	{
		::vl_workflow_global::TryFinally::Instance().test1();
		::vl_workflow_global::TryFinally::Instance().test2();
		::vl_workflow_global::TryFinally::Instance().test3();
		::vl_workflow_global::TryFinally::Instance().test4();
		return [&](){ ::vl::WString __vwsn_temp__; ::vl::reflection::description::TypedValueSerializerProvider<::vl::vint32_t>::Serialize(::vl_workflow_global::TryFinally::Instance().sum, __vwsn_temp__); return __vwsn_temp__; }();
	}

	TryFinally& TryFinally::Instance()
	{
		return Getvl_workflow_global_TryFinally().instance;
	}
}

