/***********************************************************************
Generated from ../Resources/Codegen/EnumCtor2.txt
***********************************************************************/

#ifndef VCZH_WORKFLOW_CPP_GENERATED_ENUMCTOR2
#define VCZH_WORKFLOW_CPP_GENERATED_ENUMCTOR2

#include "../Source/CppTypes.h"

#if defined(__GNUC__)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wparentheses-equality"
#elif defined(__clang__)
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wparentheses-equality"
#endif

enum class Seasons : vl::vuint64_t
{
	None = 0UL,
	Spring = 1UL,
	Summer = 2UL,
	Autumn = 4UL,
	Winter = 8UL,
	Good = Spring | Autumn,
	Bad = Summer | Winter,
};
inline Seasons operator& (Seasons a, Seasons b) { return static_cast<Seasons>(static_cast<::vl::vuint64_t>(a) & static_cast<::vl::vuint64_t>(b)); }
inline Seasons operator| (Seasons a, Seasons b) { return static_cast<Seasons>(static_cast<::vl::vuint64_t>(a) | static_cast<::vl::vuint64_t>(b)); }


/***********************************************************************
Global Variables and Functions
***********************************************************************/

namespace vl_workflow_global
{
	class EnumCtor2
	{
	public:

		::Seasons Id(::Seasons s);
		::vl::WString main();

		static EnumCtor2& Instance();
	};
}

#if defined(__GNUC__)
#pragma GCC diagnostic pop
#elif defined(__clang__)
#pragma clang diagnostic pop
#endif

#endif
