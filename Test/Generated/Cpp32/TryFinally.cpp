/***********************************************************************
!!!!!! DO NOT MODIFY !!!!!!

Source: ../Resources/Codegen/TryFinally.txt

This file is generated by Workflow compiler
https://github.com/vczh-libraries
***********************************************************************/

#include "TryFinally.h"

#if defined( _MSC_VER)
#pragma warning(push)
#pragma warning(disable:4250)
#elif defined(__clang__)
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wparentheses-equality"
#elif defined(__GNUC__)
#pragma GCC diagnostic push
#endif

#define GLOBAL_SYMBOL ::vl_workflow_global::TryFinally::
#define GLOBAL_NAME ::vl_workflow_global::TryFinally::Instance().
#define GLOBAL_OBJ &::vl_workflow_global::TryFinally::Instance()

/***********************************************************************
Global Variables
***********************************************************************/

BEGIN_GLOBAL_STORAGE_CLASS(vl_workflow_global_TryFinally)
	vl_workflow_global::TryFinally instance;
	INITIALIZE_GLOBAL_STORAGE_CLASS

		instance.sum = static_cast<::vl::vint32_t>(0);
	FINALIZE_GLOBAL_STORAGE_CLASS

END_GLOBAL_STORAGE_CLASS(vl_workflow_global_TryFinally)

namespace vl_workflow_global
{
/***********************************************************************
Global Functions
***********************************************************************/

	void TryFinally::test1()
	{
		{
			auto __vwsn_for_begin_x = static_cast<::vl::vint32_t>(1);
			auto __vwsn_for_end_x = static_cast<::vl::vint32_t>(5);
			auto x = __vwsn_for_begin_x;
			while ((x <= __vwsn_for_end_x))
			{
				{
					{
						auto __vwsnb_0 = [&]()
						{
							(GLOBAL_NAME sum = (GLOBAL_NAME sum + static_cast<::vl::vint32_t>(1)));
						}
						;
						::vl::__vwsn::RunOnExit<::std::remove_cvref_t<decltype(__vwsnb_0)>> __vwsnb_0_dtor(&__vwsnb_0);
						try
						{
							{
								{
									(x = (x + static_cast<::vl::vint32_t>(1)));
									continue;
								}
							}
						}
						catch(const ::vl::Exception&)
						{
						}
					}
				}
				(x = (x + static_cast<::vl::vint32_t>(1)));
			}
		}
	}

	void TryFinally::test2()
	{
		{
			auto __vwsn_for_begin_x = static_cast<::vl::vint32_t>(1);
			auto __vwsn_for_end_x = static_cast<::vl::vint32_t>(5);
			auto x = __vwsn_for_begin_x;
			while ((x <= __vwsn_for_end_x))
			{
				{
					{
						auto __vwsnb_0 = [&]()
						{
							(GLOBAL_NAME sum = (GLOBAL_NAME sum + static_cast<::vl::vint32_t>(10)));
						}
						;
						::vl::__vwsn::RunOnExit<::std::remove_cvref_t<decltype(__vwsnb_0)>> __vwsnb_0_dtor(&__vwsnb_0);
						try
						{
							{
								break;
							}
						}
						catch(const ::vl::Exception&)
						{
						}
					}
				}
				(x = (x + static_cast<::vl::vint32_t>(1)));
			}
		}
	}

	void TryFinally::test3()
	{
		{
			auto __vwsnb_0 = [&]()
			{
				(GLOBAL_NAME sum = (GLOBAL_NAME sum + static_cast<::vl::vint32_t>(100)));
			}
			;
			::vl::__vwsn::RunOnExit<::std::remove_cvref_t<decltype(__vwsnb_0)>> __vwsnb_0_dtor(&__vwsnb_0);
			try
			{
				{
					return;
				}
			}
			catch(const ::vl::Exception&)
			{
			}
		}
	}

	void TryFinally::test4()
	{
		{
			auto __vwsnb_0 = [&]()
			{
				(GLOBAL_NAME sum = (GLOBAL_NAME sum + static_cast<::vl::vint32_t>(1000)));
			}
			;
			::vl::__vwsn::RunOnExit<::std::remove_cvref_t<decltype(__vwsnb_0)>> __vwsnb_0_dtor(&__vwsnb_0);
			try
			{
				{
					{
						auto __vwsnb_1 = [&]()
						{
							(GLOBAL_NAME sum = (GLOBAL_NAME sum + static_cast<::vl::vint32_t>(1000)));
						}
						;
						::vl::__vwsn::RunOnExit<::std::remove_cvref_t<decltype(__vwsnb_1)>> __vwsnb_1_dtor(&__vwsnb_1);
						try
						{
							{
								{
									auto __vwsnb_2 = [&]()
									{
										(GLOBAL_NAME sum = (GLOBAL_NAME sum + static_cast<::vl::vint32_t>(1000)));
									}
									;
									::vl::__vwsn::RunOnExit<::std::remove_cvref_t<decltype(__vwsnb_2)>> __vwsnb_2_dtor(&__vwsnb_2);
									try
									{
										{
											return;
										}
									}
									catch(const ::vl::Exception&)
									{
									}
								}
							}
						}
						catch(const ::vl::Exception&)
						{
						}
					}
				}
			}
			catch(const ::vl::Exception&)
			{
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

#if defined( _MSC_VER)
#pragma warning(pop)
#elif defined(__clang__)
#pragma clang diagnostic pop
#elif defined(__GNUC__)
#pragma GCC diagnostic pop
#endif
