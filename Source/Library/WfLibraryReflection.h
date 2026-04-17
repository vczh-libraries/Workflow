/***********************************************************************
Vczh Library++ 3.0
Developer: Zihan Chen(vczh)
Framework::Reflection

Interfaces:
***********************************************************************/

#ifndef VCZH_WORKFLOW_LIBRARY_REFLECTION
#define VCZH_WORKFLOW_LIBRARY_REFLECTION

#include "WfLibraryPredefined.h"
#include "WfLibraryRpc.h"

namespace vl
{
	namespace reflection
	{
		namespace description
		{

/***********************************************************************
Predefined Types
***********************************************************************/

#ifndef VCZH_DEBUG_NO_REFLECTION

#define WORKFLOW_LIBRARY_ATTRIBUTE_TYPES(F)\
			F(vl::__vwsn::att_cpp_File)\
			F(vl::__vwsn::att_cpp_UserImpl)\
			F(vl::__vwsn::att_cpp_Private)\
			F(vl::__vwsn::att_cpp_Protected)\
			F(vl::__vwsn::att_cpp_Friend)\
			F(vl::__vwsn::att_rpc_Interface)\
			F(vl::__vwsn::att_rpc_Ctor)\
			F(vl::__vwsn::att_rpc_Byval)\
			F(vl::__vwsn::att_rpc_Byref)\
			F(vl::__vwsn::att_rpc_Cached)\
			F(vl::__vwsn::att_rpc_Dynamic)\

#define WORKFLOW_LIBRARY_TYPES(F)\
			F(Sys)							\
			F(Math)							\
			F(Localization)					\
			F(CoroutineStatus)				\
			F(CoroutineResult)				\
			F(ICoroutine)					\
			F(EnumerableCoroutine::IImpl)	\
			F(EnumerableCoroutine)			\
			F(AsyncStatus)					\
			F(AsyncContext)					\
			F(IAsync)						\
			F(IPromise)						\
			F(IFuture)						\
			F(IAsyncScheduler)				\
			F(AsyncCoroutine::IImpl)		\
			F(AsyncCoroutine)				\
			F(StateMachine)					\
			F(Versioning)					\
			F(vl::rpc_controller::RpcObjectReference)\
			F(vl::rpc_controller::IRpcIdSync)\
			F(vl::rpc_controller::IRpcListOps)\
			F(vl::rpc_controller::IRpcListEventOps)\
			F(vl::rpc_controller::IRpcObjectOps)\
			F(vl::rpc_controller::IRpcObjectEventOps)\
			F(vl::rpc_controller::IRpcController)\
			F(vl::rpc_controller::IRpcLifeCycle)\
			WORKFLOW_LIBRARY_ATTRIBUTE_TYPES(F)\

			WORKFLOW_LIBRARY_TYPES(DECL_TYPE_INFO)

#endif

/***********************************************************************
Interface Implementation Proxy (Implement)
***********************************************************************/

#ifdef VCZH_DESCRIPTABLEOBJECT_WITH_METADATA

#pragma warning(push)
#pragma warning(disable:4250)

			BEGIN_INTERFACE_PROXY_NOPARENT_SHAREDPTR(vl::rpc_controller::IRpcIdSync)
				void SyncIds(vl::Ptr<vl::reflection::description::IValueDictionary> ids)override
				{
					INVOKE_INTERFACE_PROXY(SyncIds, ids);
				}
			END_INTERFACE_PROXY(vl::rpc_controller::IRpcIdSync)

			BEGIN_INTERFACE_PROXY_SHAREDPTR(vl::rpc_controller::IRpcObjectOps, vl::rpc_controller::IRpcIdSync)
				vl::reflection::description::Value InvokeMethod(vl::rpc_controller::RpcObjectReference ref, vl::vint methodId, vl::Ptr<vl::reflection::description::IValueArray> arguments)override
				{
					INVOKEGET_INTERFACE_PROXY(InvokeMethod, ref, methodId, arguments);
				}

				vl::Ptr<vl::reflection::description::IAsync> InvokeMethodAsync(vl::rpc_controller::RpcObjectReference ref, vl::vint methodId, vl::Ptr<vl::reflection::description::IValueArray> arguments)override
				{
					INVOKEGET_INTERFACE_PROXY(InvokeMethodAsync, ref, methodId, arguments);
				}

				void ObjectHold(vl::rpc_controller::RpcObjectReference ref, bool hold)override
				{
					INVOKE_INTERFACE_PROXY(ObjectHold, ref, hold);
				}

				vl::rpc_controller::RpcObjectReference RequestService(vl::vint typeId)override
				{
					INVOKEGET_INTERFACE_PROXY(RequestService, typeId);
				}
			END_INTERFACE_PROXY(vl::rpc_controller::IRpcObjectOps)

			BEGIN_INTERFACE_PROXY_SHAREDPTR(vl::rpc_controller::IRpcObjectEventOps, vl::rpc_controller::IRpcIdSync)
				void InvokeEvent(vl::rpc_controller::RpcObjectReference ref, vl::vint eventId, vl::Ptr<vl::reflection::description::IValueArray> arguments)override
				{
					INVOKE_INTERFACE_PROXY(InvokeEvent, ref, eventId, arguments);
				}
			END_INTERFACE_PROXY(vl::rpc_controller::IRpcObjectEventOps)

			BEGIN_INTERFACE_PROXY_NOPARENT_SHAREDPTR(ICoroutine)

				void Resume(bool raiseException, Ptr<CoroutineResult> output)override
				{
					INVOKE_INTERFACE_PROXY(Resume, raiseException, output);
				}

				Ptr<IValueException> GetFailure()override
				{
					INVOKEGET_INTERFACE_PROXY_NOPARAMS(GetFailure);
				}

				CoroutineStatus GetStatus()override
				{
					INVOKEGET_INTERFACE_PROXY_NOPARAMS(GetStatus);
				}
			END_INTERFACE_PROXY(ICoroutine)

			BEGIN_INTERFACE_PROXY_NOPARENT_SHAREDPTR(IAsync)

				AsyncStatus GetStatus()override
				{
					INVOKEGET_INTERFACE_PROXY_NOPARAMS(GetStatus);
				}

				bool Execute(const Func<void(Ptr<CoroutineResult>)>& callback, Ptr<AsyncContext> context)override
				{
					INVOKEGET_INTERFACE_PROXY(Execute, callback, context);
				}
			END_INTERFACE_PROXY(IAsync)

#pragma warning(pop)

#endif

/***********************************************************************
LoadPredefinedTypes
***********************************************************************/

			extern bool										WfLoadLibraryTypes();
		}
	}
}

#endif
