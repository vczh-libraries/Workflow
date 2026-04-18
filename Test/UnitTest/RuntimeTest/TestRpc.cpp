#include "../../Source/Helper.h"
#include "../../Source/RpcDualLifecycleMock.h"
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

				// Create two lifecycle mocks with adapters
				auto lc1 = Ptr(new RpcDualLifecycleMock(1));
				auto lc2 = Ptr(new RpcDualLifecycleMock(2));
				auto adapter1 = Ptr(new RpcDualLifeCycleAdapter(lc1.Obj()));
				auto adapter2 = Ptr(new RpcDualLifeCycleAdapter(lc2.Obj()));
				lc1->SetPeer(lc2.Obj());
				lc2->SetPeer(lc1.Obj());
				lc1->SetGlobalContext(globalContext);
				lc2->SetGlobalContext(globalContext);
				lc1->SetIdMap(idMap);
				lc2->SetIdMap(idMap);
				lc1->SetAdapter(adapter1.Obj());
				lc2->SetAdapter(adapter2.Obj());

				// Create list ops default implementations
				auto lo1 = Ptr(new RpcCalleeListOps(adapter1.Obj()));
				auto leo1 = Ptr(new RpcCalleeListEventBridge(adapter1.Obj()));
				auto lo2 = Ptr(new RpcCalleeListOps(adapter2.Obj()));
				auto leo2 = Ptr(new RpcCalleeListEventBridge(adapter2.Obj()));

				// Create object ops implementations from the assembly
				auto createObjectOps = LoadFunction<Ptr<IRpcObjectOps>(IRpcLifeCycle*)>(globalContext, L"rpc_IRpcObjectOps");
				auto createEventOps = LoadFunction<Ptr<IRpcObjectEventOps>(IRpcLifeCycle*)>(globalContext, L"rpc_IRpcObjectEventOps");
				auto oo1 = createObjectOps(adapter1.Obj());
				auto oeo1 = createEventOps(adapter1.Obj());
				auto oo2 = createObjectOps(adapter2.Obj());
				auto oeo2 = createEventOps(adapter2.Obj());

				// Register cross: ops from lc1 go to lc2, and vice versa
				lc2->Register(oo1, oeo1, lo1, leo1);
				lc1->Register(oo2, oeo2, lo2, leo2);

				// Run serviceMain with lc1
				auto serviceMain = LoadFunction<void(IRpcLifeCycle*)>(globalContext, L"serviceMain");
				serviceMain(adapter1.Obj());

				// Run clientMain with lc2 and get the result
				auto clientMain = LoadFunction<WString(IRpcLifeCycle*)>(globalContext, L"clientMain");
				auto actual = clientMain(adapter2.Obj());

				TEST_PRINT(L"    expected  : " + itemResult);
				TEST_PRINT(L"    actual    : " + actual);
				TEST_ASSERT(actual == itemResult);
			});
		}
	});
}
