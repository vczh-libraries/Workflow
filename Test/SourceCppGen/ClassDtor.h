/***********************************************************************
Generated from ../Resources/Codegen/ClassDtor.txt
***********************************************************************/

#ifndef VCZH_WORKFLOW_CPP_GENERATED_CLASSDTOR
#define VCZH_WORKFLOW_CPP_GENERATED_CLASSDTOR

#include "../Source/CppTypes.h"

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

#endif
