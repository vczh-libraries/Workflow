#include "../../Source/Helper.h"

TEST_FILE
{
	TEST_CATEGORY(L"RPC binary assemblies")
	{
		List<WString> rpcNames;
		LoadSampleIndex(L"Rpc", rpcNames);

		for (auto itemName : rpcNames)
		{
			TEST_CASE(itemName)
			{
				TEST_PRINT(itemName);
				Ptr<WfAssembly> assembly;
				LoadSampleAssemblyBinary(L"Rpc", itemName, assembly);
				TEST_ASSERT(assembly);
			});
		}
	});
}
