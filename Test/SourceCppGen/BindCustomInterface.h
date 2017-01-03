/***********************************************************************
Generated from ../Resources/Codegen/BindCustomInterface.txt
***********************************************************************/

#ifndef VCZH_WORKFLOW_CPP_GENERATED_BINDCUSTOMINTERFACE
#define VCZH_WORKFLOW_CPP_GENERATED_BINDCUSTOMINTERFACE

#include "../Source/CppTypes.h"

#pragma warning(push)
#pragma warning(disable:4250)
class ISummer;
class IAdder;

class ISummer : public virtual ::vl::reflection::IDescriptable, public ::vl::reflection::Description<ISummer>
{
public:

};

class IAdder : public virtual ::ISummer, public ::vl::reflection::Description<IAdder>
{
public:

};
#pragma warning(pop)

/***********************************************************************
Global Variables and Functions
***********************************************************************/

namespace vl_workflow_global
{
	class BindCustomInterface
	{
	public:

		::vl::WString s;

		::vl::Ptr<::vl::reflection::description::IValueSubscription> Bind(::vl::Ptr<::ISummer> summer);
		void Execute(::vl::Ptr<::IAdder> adder);
		::vl::WString main();

		static BindCustomInterface& Instance();
	};
}

#endif
