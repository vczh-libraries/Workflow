/***********************************************************************
Generated from ../Resources/Codegen/BindCustomInterface2.txt
***********************************************************************/

#ifndef VCZH_WORKFLOW_CPP_GENERATED_BINDCUSTOMINTERFACE2
#define VCZH_WORKFLOW_CPP_GENERATED_BINDCUSTOMINTERFACE2

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
class ISummer2;
class IAdder2;

class ISummer2 : public virtual ::vl::reflection::IDescriptable, public ::vl::reflection::Description<ISummer2>
{
public:

	virtual ::vl::vint32_t GetSum() = 0;
	::vl::Event<void()> SumChanged;
};

class IAdder2 : public virtual ::ISummer2, public ::vl::reflection::Description<IAdder2>
{
public:

	virtual void Add(::vl::vint32_t value) = 0;
};
#pragma warning(pop)

/***********************************************************************
Global Variables and Functions
***********************************************************************/

namespace vl_workflow_global
{
	class BindCustomInterface2
	{
	public:

		::vl::WString s;

		::vl::Ptr<::vl::reflection::description::IValueSubscription> Bind(::vl::Ptr<::ISummer2> summer);
		void Execute(::vl::Ptr<::IAdder2> adder);
		::vl::WString main();

		static BindCustomInterface2& Instance();
	};
}

#if defined(__GNUC__)
#pragma GCC diagnostic pop
#elif defined(__clang__)
#pragma clang diagnostic pop
#endif

#endif
