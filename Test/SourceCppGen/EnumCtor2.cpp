/***********************************************************************
Generated from ../Resources/Codegen/EnumCtor2.txt
***********************************************************************/

#include "EnumCtor2.h"

#if defined(__GNUC__)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wparentheses-equality"
#elif defined(__clang__)
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wparentheses-equality"
#endif
#define GLOBAL_SYMBOL ::vl_workflow_global::EnumCtor2::
#define GLOBAL_NAME ::vl_workflow_global::EnumCtor2::Instance().
#define GLOBAL_OBJ &::vl_workflow_global::EnumCtor2::Instance()

/***********************************************************************
Global Variables and Functions
***********************************************************************/

BEGIN_GLOBAL_STORAGE_CLASS(vl_workflow_global_EnumCtor2)
	vl_workflow_global::EnumCtor2 instance;
	INITIALIZE_GLOBAL_STORAGE_CLASS
	FINALIZE_GLOBAL_STORAGE_CLASS
END_GLOBAL_STORAGE_CLASS(vl_workflow_global_EnumCtor2)

namespace vl_workflow_global
{
	::Seasons EnumCtor2::Id(::Seasons s)
	{
		return s;
	}

	::vl::WString EnumCtor2::main()
	{
		auto a = ::Seasons::Spring;
		auto b = ::Seasons::Summer;
		auto c = GLOBAL_NAME Id(::Seasons::Autumn);
		auto d = ::Seasons::Winter;
		return ((((((((((((::vl::__vwsn::ToString(static_cast<::vl::vuint64_t>(a)) + ::vl::WString(L", ", false)) + ::vl::__vwsn::ToString(static_cast<::vl::vuint64_t>(b))) + ::vl::WString(L", ", false)) + ::vl::__vwsn::ToString(static_cast<::vl::vuint64_t>(c))) + ::vl::WString(L", ", false)) + ::vl::__vwsn::ToString(static_cast<::vl::vuint64_t>(d))) + ::vl::WString(L", ", false)) + ::vl::__vwsn::ToString(static_cast<::vl::vuint64_t>((a | b)))) + ::vl::WString(L", ", false)) + ::vl::__vwsn::ToString(static_cast<::vl::vuint64_t>((a & ::Seasons::Good)))) + ::vl::WString(L", ", false)) + ::vl::__vwsn::ToString(static_cast<::vl::vuint64_t>((a & ::Seasons::Bad))));
	}

	EnumCtor2& EnumCtor2::Instance()
	{
		return Getvl_workflow_global_EnumCtor2().instance;
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

/***********************************************************************
Reflection
***********************************************************************/

namespace vl
{
	namespace reflection
	{
		namespace description
		{
#ifndef VCZH_DEBUG_NO_REFLECTION
			IMPL_CPP_TYPE_INFO(::Seasons)


			class EnumCtor2TypeLoader : public Object, public ITypeLoader
			{
			public:
				void Load(ITypeManager* manager)
				{
					ADD_TYPE_INFO(::Seasons)
				}

				void Unload(ITypeManager* manager)
				{
				}
			};
#endif

			bool LoadEnumCtor2Types()
			{
#ifndef VCZH_DEBUG_NO_REFLECTION
				if (auto manager = GetGlobalTypeManager())
				{
					return manager->AddTypeLoader(MakePtr<EnumCtor2TypeLoader>());
				}
#endif
				return false;
			}
		}
	}
}
