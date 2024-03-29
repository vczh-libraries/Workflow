/***********************************************************************
!!!!!! DO NOT MODIFY !!!!!!

Source: ../Resources/Codegen/WorkflowMultiFiles.txt

This file is generated by Workflow compiler
https://github.com/vczh-libraries
***********************************************************************/

#ifndef VCZH_WORKFLOW_COMPILER_GENERATED_WORKFLOWMULTIFILES
#define VCZH_WORKFLOW_COMPILER_GENERATED_WORKFLOWMULTIFILES

#include "../Source/CppTypes.h"

#if defined( _MSC_VER)
#pragma warning(push)
#pragma warning(disable:4250)
#elif defined(__clang__)
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wparentheses-equality"
#elif defined(__GNUC__)
#pragma GCC diagnostic push
#endif

namespace multifiles
{
	class Animal;
	class C1;
	class C2;
	class C3;
	class C4;
	class Cat;
	class Dog;
	class BadDog;
	class FlyingObject;
	class Bird;
	class GoodDog;
	class HuntingObject;
	class Leopard;
	class Lion;
	class Tiger;

	class C1 : public ::vl::Object, public ::vl::reflection::Description<C1>
	{
#ifdef VCZH_DESCRIPTABLEOBJECT_WITH_METADATA
		friend struct ::vl::reflection::description::CustomTypeDescriptorSelector<C1>;
#endif
	public:
		C1();
	};

	class C2 : public ::vl::Object, public ::vl::reflection::Description<C2>
	{
#ifdef VCZH_DESCRIPTABLEOBJECT_WITH_METADATA
		friend struct ::vl::reflection::description::CustomTypeDescriptorSelector<C2>;
#endif
	public:
		C2();
	};

	class C3 : public ::multifiles::C1, public ::multifiles::C2, public ::vl::reflection::Description<C3>
	{
#ifdef VCZH_DESCRIPTABLEOBJECT_WITH_METADATA
		friend struct ::vl::reflection::description::CustomTypeDescriptorSelector<C3>;
#endif
	public:
		C3();
	};

	class C4 : public ::vl::Object, public ::vl::reflection::Description<C4>
	{
#ifdef VCZH_DESCRIPTABLEOBJECT_WITH_METADATA
		friend struct ::vl::reflection::description::CustomTypeDescriptorSelector<C4>;
#endif
	public:
		C4();
	};

	class FlyingObject : public ::vl::Object, public ::vl::reflection::Description<FlyingObject>
	{
#ifdef VCZH_DESCRIPTABLEOBJECT_WITH_METADATA
		friend struct ::vl::reflection::description::CustomTypeDescriptorSelector<FlyingObject>;
#endif
	public:
		FlyingObject();
	};

}
/***********************************************************************
Global Variables and Functions
***********************************************************************/

namespace vl_workflow_global
{
	class WorkflowMultiFiles
	{
	public:

		::vl::WString main();

		static WorkflowMultiFiles& Instance();
	};
}

#if defined( _MSC_VER)
#pragma warning(pop)
#elif defined(__clang__)
#pragma clang diagnostic pop
#elif defined(__GNUC__)
#pragma GCC diagnostic pop
#endif

#endif
