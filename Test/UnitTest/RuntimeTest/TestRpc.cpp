#include "../../Source/Helper.h"
#include "../../Source/RpcDualLifecycleMock.h"
#include "../../../Source/Runtime/WfRuntime.h"
#include "../../../Source/Library/WfLibraryReflection.h"

using namespace vl::rpc_controller;
using namespace vl::rpc_controller_test;
using namespace vl::workflow::runtime;

bool DecodeRpcName(const WString& rpcLine, WString& itemName, WString& itemResult)
{
	const wchar_t* reading = rpcLine.Buffer();
	auto eq = wcschr(reading, L'=');
	if (!eq) return false;
	vint index = eq - reading;
	itemName = rpcLine.Sub(0, index);
	itemResult = rpcLine.Sub(index + 1, rpcLine.Length() - index - 1);
	return true;
}

namespace
{
	using namespace vl;
	using namespace vl::collections;
	using namespace vl::reflection;
	using namespace vl::reflection::description;

	class RpcWorkflowLifecycleMock : public RpcDualLifecycleMock
	{
	private:
		Ptr<WfRuntimeGlobalContext>								globalContext;
		Ptr<IValueFunctionProxy>								wrapperCreateFunc;
		Func<void(vint, IRpcLifeCycle*, RpcObjectReference, IDescriptable*)>		listenerAttachFunc;
	public:
		RpcWorkflowLifecycleMock(vint _clientId)
			: RpcDualLifecycleMock(_clientId)
		{
		}

		~RpcWorkflowLifecycleMock()
		{
			// Disconnect wrappers while globalContext is still alive,
			// because DisconnectFromLifecycle executes Workflow bytecode.
			DisconnectTrackedWrappers();
			GetController()->UnregisterAllLocalObjects(true);
			listenerAttachFunc = {};
			wrapperCreateFunc = nullptr;
			globalContext = nullptr;
		}

		void DisconnectTrackedWrappersBeforeDispose()
		{
			DisconnectTrackedWrappers();
		}

		void SetGlobalContext(Ptr<WfRuntimeGlobalContext> _globalContext)
		{
			globalContext = _globalContext;
		}

		void SetIdMapWithReflection(const Dictionary<WString, vint>& _idMap)
		{
			SetIdMap(_idMap);
			wrapperCreateFunc = LoadFunction(globalContext, L"rpcwrapper_Create");
			auto listenerAttachIndex = globalContext->assembly->functionByName.Keys().IndexOf(L"rpclistener_Attach");
			if (listenerAttachIndex == -1)
			{
				listenerAttachFunc = {};
			}
			else
			{
				const auto& listenerAttachFunctions = globalContext->assembly->functionByName.GetByIndex(listenerAttachIndex);
				CHECK_ERROR(listenerAttachFunctions.Count() <= 1, L"Multiple rpclistener_Attach functions are found.");
				listenerAttachFunc = listenerAttachFunctions.Count() == 1
					? LoadFunction<void(vint, IRpcLifeCycle*, RpcObjectReference, IDescriptable*)>(globalContext, L"rpclistener_Attach")
					: Func<void(vint, IRpcLifeCycle*, RpcObjectReference, IDescriptable*)>();
			}

			RegisterWrapperFactory([this](RpcObjectReference ref, IRpcLifeCycle* lc) -> Ptr<IRpcWrapperBase>
			{
				auto argList = IValueList::Create();
				argList->Add(BoxValue(ref));
				argList->Add(BoxValue<IRpcLifeCycle*>(lc));
				auto result = wrapperCreateFunc->Invoke(argList);
				auto wrapper = Ptr(result.GetRawPtr()->SafeAggregationCast<IRpcWrapperBase>());
				CHECK_ERROR(wrapper, L"rpcwrapper_Create did not return IRpcWrapperBase.");
				return wrapper;
			});
		}

		vint DecideTypeId(IDescriptable* obj)const override
		{
			auto baseResult = RpcDualLifecycleMock::DecideTypeId(obj);
			if (baseResult != RpcTypeId_NotFound) return baseResult;

			auto descriptable = dynamic_cast<DescriptableObject*>(obj);
			if (descriptable)
			{
				auto td = descriptable->GetTypeDescriptor();
				if (td)
				{
					for (auto&& [typeName, typeId] : idMap)
					{
						if (typeName.Length() == 0 || wcschr(typeName.Buffer(), L'.') != nullptr) continue;
						auto knownTd = description::GetTypeDescriptor(typeName);
						if (knownTd && td->CanConvertTo(knownTd))
						{
							return typeId;
						}
					}
				}
			}

			return RpcTypeId_NotFound;
		}

		bool AttachLocalObjectEvents(RpcObjectReference ref, IDescriptable* obj, List<Func<void()>>& detachments) override
		{
			(void)detachments;
			if (!listenerAttachFunc) return false;
			if (ref.typeId < 0) return false;
			listenerAttachFunc(ref.typeId, this, ref, obj);
			return true;
		}
	};
}

TEST_FILE
{
	TEST_CATEGORY(L"RPC binary assemblies")
	{
		List<WString> rpcNames;
		LoadSampleIndex(L"Rpc", rpcNames);

		for (auto rpcLine : rpcNames)
		{
			WString itemName, itemResult;
			if (!DecodeRpcName(rpcLine, itemName, itemResult)) continue;

			TEST_CASE(itemName)
			{
				Ptr<WfAssembly> assembly;
				LoadSampleAssemblyBinary(L"Rpc", itemName, assembly);
				TEST_ASSERT(assembly);

				auto globalContext = Ptr(new WfRuntimeGlobalContext(assembly));
				LoadFunction<void()>(globalContext, L"<initialize>")();

				// Get the id map from rpc_GetIds
				auto getIds = LoadFunction<Ptr<IValueDictionary>()>(globalContext, L"rpc_GetIds");
				auto idDictObj = getIds();
				Dictionary<WString, vint> idMap;
				{
					auto keys = idDictObj->GetKeys();
					auto values = idDictObj->GetValues();
					for (vint i = 0; i < idDictObj->GetCount(); i++)
					{
						auto key = UnboxValue<WString>(keys->Get(i));
						auto val = UnboxValue<vint>(values->Get(i));
						idMap.Set(key, val);
					}
				}

				// Create two lifecycle mocks
				auto lc1 = Ptr(new RpcWorkflowLifecycleMock(1));
				auto lc2 = Ptr(new RpcWorkflowLifecycleMock(2));
				lc1->SetGlobalContext(globalContext);
				lc2->SetGlobalContext(globalContext);
				lc1->SetIdMapWithReflection(idMap);
				lc2->SetIdMapWithReflection(idMap);

				// Create list ops default implementations
				auto lo1 = Ptr(new RpcCalleeListOps(lc1.Obj()));
				auto leo1 = Ptr(new RpcCalleeListEventBridge(lc1.Obj()));
				auto lo2 = Ptr(new RpcCalleeListOps(lc2.Obj()));
				auto leo2 = Ptr(new RpcCalleeListEventBridge(lc2.Obj()));

				// Create object ops implementations from the assembly
				auto createObjectOps = LoadFunction<Ptr<IRpcObjectOps>(IRpcLifeCycle*)>(globalContext, L"rpc_IRpcObjectOps");
				auto createEventOps = LoadFunction<Ptr<IRpcObjectEventOps>(IRpcLifeCycle*)>(globalContext, L"rpc_IRpcObjectEventOps");
				auto oo1 = createObjectOps(lc1.Obj());
				auto oeo1 = createEventOps(lc1.Obj());
				auto oo2 = createObjectOps(lc2.Obj());
				auto oeo2 = createEventOps(lc2.Obj());

				lc1->GetController()->Register(oo1, oeo1, lo1, leo1);
				lc2->GetController()->Register(oo2, oeo2, lo2, leo2);
				RpcDualDispatcherMock dispatcher(lc1.Obj(), lc2.Obj());

				// Run serviceMain with lc1
				auto serviceMain = LoadFunction<void(IRpcLifeCycle*)>(globalContext, L"serviceMain");
				serviceMain(lc1.Obj());

				// Run clientMain with lc2 and get the result
				auto clientMain = LoadFunction<WString(IRpcLifeCycle*)>(globalContext, L"clientMain");
				auto actual = clientMain(lc2.Obj());

				TEST_PRINT(L"    expected  : " + itemResult);
				TEST_PRINT(L"    actual    : " + actual);
				TEST_ASSERT(actual == itemResult);

				// Explicit cleanup order to avoid circular references
				// between WfRuntimeGlobalContext and Workflow objects
				lc2->DisconnectTrackedWrappersBeforeDispose();
				lc1->DisconnectTrackedWrappersBeforeDispose();
				lc2->GetController()->UnregisterAllLocalObjects(true);
				lc1->GetController()->UnregisterAllLocalObjects(true);
				oeo2 = nullptr;
				oo2 = nullptr;
				oeo1 = nullptr;
				oo1 = nullptr;
				leo2 = nullptr;
				lo2 = nullptr;
				leo1 = nullptr;
				lo1 = nullptr;
				globalContext->globalVariables = nullptr;
				lc2 = nullptr;
				lc1 = nullptr;
				globalContext = nullptr;
			});
		}
	});
}
