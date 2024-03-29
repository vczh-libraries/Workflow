/***********************************************************************
!!!!!! DO NOT MODIFY !!!!!!

Source: ../Resources/Codegen/WorkflowMultiFiles.txt

This file is generated by Workflow compiler
https://github.com/vczh-libraries
***********************************************************************/

#ifndef VCZH_WORKFLOW_COMPILER_GENERATED_WORKFLOWMULTIFILESREFLECTION
#define VCZH_WORKFLOW_COMPILER_GENERATED_WORKFLOWMULTIFILESREFLECTION

#include "WorkflowMultiFilesIncludes.h"

#if defined( _MSC_VER)
#pragma warning(push)
#pragma warning(disable:4250)
#elif defined(__clang__)
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wparentheses-equality"
#elif defined(__GNUC__)
#pragma GCC diagnostic push
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
			DECL_TYPE_INFO(::multifiles::Animal)
			DECL_TYPE_INFO(::multifiles::BadDog)
			DECL_TYPE_INFO(::multifiles::Bird)
			DECL_TYPE_INFO(::multifiles::C1)
			DECL_TYPE_INFO(::multifiles::C2)
			DECL_TYPE_INFO(::multifiles::C3)
			DECL_TYPE_INFO(::multifiles::C4)
			DECL_TYPE_INFO(::multifiles::Cat)
			DECL_TYPE_INFO(::multifiles::Dog)
			DECL_TYPE_INFO(::multifiles::FlyingObject)
			DECL_TYPE_INFO(::multifiles::GoodDog)
			DECL_TYPE_INFO(::multifiles::HuntingObject)
			DECL_TYPE_INFO(::multifiles::HuntingObject::Inside)
			DECL_TYPE_INFO(::multifiles::Leopard)
			DECL_TYPE_INFO(::multifiles::Lion)
			DECL_TYPE_INFO(::multifiles::Tiger)
#endif

			extern bool LoadWorkflowMultiFilesTypes();
		}
	}
}

#if defined( _MSC_VER)
#pragma warning(pop)
#elif defined(__clang__)
#pragma clang diagnostic pop
#elif defined(__GNUC__)
#pragma GCC diagnostic pop
#endif

#endif
