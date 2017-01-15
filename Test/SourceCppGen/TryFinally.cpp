/***********************************************************************
Generated from ../Resources/Codegen/TryFinally.txt
***********************************************************************/

#include "TryFinally.h"

#define GLOBAL_SYMBOL ::vl_workflow_global::TryFinally::
#define GLOBAL_NAME ::vl_workflow_global::TryFinally::Instance().
#define GLOBAL_OBJ &::vl_workflow_global::TryFinally::Instance()

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
		FOREACH(__vwsnt_0, x, ::vl::__vwsn::Range(1, 5 + 1))
		{
			{
				auto __vwsnb_0 = [&]()
				{
					(GLOBAL_NAME sum = (GLOBAL_NAME sum + 1));
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
		FOREACH(__vwsnt_0, x, ::vl::__vwsn::Range(1, 5 + 1))
		{
			{
				auto __vwsnb_0 = [&]()
				{
					(GLOBAL_NAME sum = (GLOBAL_NAME sum + 10));
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
				(GLOBAL_NAME sum = (GLOBAL_NAME sum + 100));
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
				(GLOBAL_NAME sum = (GLOBAL_NAME sum + 1000));
			}
			;
			try
			{
				{
					{
						auto __vwsnb_1 = [&]()
						{
							(GLOBAL_NAME sum = (GLOBAL_NAME sum + 1000));
						}
						;
						try
						{
							{
								{
									auto __vwsnb_2 = [&]()
									{
										(GLOBAL_NAME sum = (GLOBAL_NAME sum + 1000));
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
		GLOBAL_NAME test1();
		GLOBAL_NAME test2();
		GLOBAL_NAME test3();
		GLOBAL_NAME test4();
		return ::vl::__vwsn::ToString(GLOBAL_NAME sum);
	}

	TryFinally& TryFinally::Instance()
	{
		return Getvl_workflow_global_TryFinally().instance;
	}
}


#undef GLOBAL_SYMBOL
#undef GLOBAL_NAME
#undef GLOBAL_OBJ
