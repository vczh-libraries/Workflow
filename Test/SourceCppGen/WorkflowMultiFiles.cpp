/***********************************************************************
!!!!!! DO NOT MODIFY !!!!!!

Source: ../Resources/Codegen/WorkflowMultiFiles.txt

This file is generated by Workflow compiler
https://github.com/vczh-libraries
***********************************************************************/

#include "WorkflowMultiFilesIncludes.h"
/* CodePack:BeginIgnore() */
#ifndef VCZH_DEBUG_NO_REFLECTION
/* CodePack:ConditionOff(VCZH_DEBUG_NO_REFLECTION, WorkflowMultiFilesReflection.h) */
#include "WorkflowMultiFilesReflection.h"
#endif
/* CodePack:EndIgnore() */

#if defined( _MSC_VER)
#pragma warning(push)
#pragma warning(disable:4250)
#elif defined(__GNUC__)
#pragma GCC diagnostic push
#elif defined(__clang__)
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wparentheses-equality"
#endif

#define GLOBAL_SYMBOL ::vl_workflow_global::WorkflowMultiFiles::
#define GLOBAL_NAME ::vl_workflow_global::WorkflowMultiFiles::Instance().
#define GLOBAL_OBJ &::vl_workflow_global::WorkflowMultiFiles::Instance()

/***********************************************************************
Global Variables
***********************************************************************/

BEGIN_GLOBAL_STORAGE_CLASS(vl_workflow_global_WorkflowMultiFiles)
	vl_workflow_global::WorkflowMultiFiles instance;
	INITIALIZE_GLOBAL_STORAGE_CLASS
	FINALIZE_GLOBAL_STORAGE_CLASS
END_GLOBAL_STORAGE_CLASS(vl_workflow_global_WorkflowMultiFiles)

namespace vl_workflow_global
{
/***********************************************************************
Global Functions
***********************************************************************/

	::vl::WString WorkflowMultiFiles::main()
	{
		return ::vl::WString(L"MultiFiles", false);
	}

	WorkflowMultiFiles& WorkflowMultiFiles::Instance()
	{
		return Getvl_workflow_global_WorkflowMultiFiles().instance;
	}
}

/***********************************************************************
Class (::multifiles::C1)
***********************************************************************/

namespace multifiles
{
	C1::C1()
	{
	}

/***********************************************************************
Class (::multifiles::C2)
***********************************************************************/

	C2::C2()
	{
	}

/***********************************************************************
Class (::multifiles::C3)
***********************************************************************/

	C3::C3()
	{
	}

/***********************************************************************
Class (::multifiles::C4)
***********************************************************************/

	C4::C4()
	{
	}

/***********************************************************************
Class (::multifiles::Cat)
***********************************************************************/

	Cat::Cat()
	{
	}

/***********************************************************************
Class (::multifiles::Dog)
***********************************************************************/

	Dog::Dog()
	{
	}

/***********************************************************************
Class (::multifiles::BadDog)
***********************************************************************/

	BadDog::BadDog()
	{
	}

/***********************************************************************
Class (::multifiles::FlyingObject)
***********************************************************************/

	FlyingObject::FlyingObject()
	{
	}

/***********************************************************************
Class (::multifiles::GoodDog)
***********************************************************************/

	GoodDog::GoodDog()
	{
	}

/***********************************************************************
Class (::multifiles::HuntingObject)
***********************************************************************/

	HuntingObject::HuntingObject()
	{
	}

/***********************************************************************
Class (::multifiles::HuntingObject::Inside)
***********************************************************************/

	HuntingObject::Inside::Inside()
	{
	}

/***********************************************************************
Class (::multifiles::Leopard)
***********************************************************************/

	Leopard::Leopard()
	{
	}

/***********************************************************************
Class (::multifiles::Tiger)
***********************************************************************/

	Tiger::Tiger()
	{
	}

}
#undef GLOBAL_SYMBOL
#undef GLOBAL_NAME
#undef GLOBAL_OBJ

#if defined( _MSC_VER)
#pragma warning(pop)
#elif defined(__GNUC__)
#pragma GCC diagnostic pop
#elif defined(__clang__)
#pragma clang diagnostic pop
#endif
