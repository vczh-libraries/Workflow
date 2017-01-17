/***********************************************************************
Generated from ../Resources/Codegen/ClassDtor.txt
***********************************************************************/

#ifndef VCZH_WORKFLOW_CPP_GENERATED_CLASSDTOR
#define VCZH_WORKFLOW_CPP_GENERATED_CLASSDTOR

#include "../Source/CppTypes.h"

#if defined(__GNUC__)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wparentheses-equality"
#elif defined(__clang__)
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wparentheses-equality"
#endif

#pragma warning(push)
#pragma warning(disable:4250)
class Dtor;

class Dtor : public ::vl::Object, public ::vl::reflection::Description<Dtor>
{
public:

	::vl::WString name = ::vl::WString(L"", false);
	::Dtor* next = static_cast<::Dtor*>(nullptr);
	Dtor();
	~Dtor();
};
#pragma warning(pop)

/***********************************************************************
Global Variables and Functions
***********************************************************************/

namespace vl_workflow_global
{
	class ClassDtor
	{
	public:

		::vl::WString s;

		::vl::WString main();

		static ClassDtor& Instance();
	};
}

#if defined(__GNUC__)
#pragma GCC diagnostic pop
#elif defined(__clang__)
#pragma clang diagnostic pop
#endif

#endif
