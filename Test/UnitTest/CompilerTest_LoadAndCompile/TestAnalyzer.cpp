#include "../../Source/Helper.h"
#include "../../Source/CppTypes.h"
#include "../../../Source/Runtime/WfRuntimeTypeDescriptor.h"

extern WfCpuArchitecture testCpuArchitecture;

namespace vl::workflow::analyzer::rpc_compiling
{
	bool IsSerializableType(ITypeInfo* type, ITypeDescriptor* rpcInterfaceAttrTd, const SortedList<ITypeDescriptor*>& rpcInterfaceTds);
}

void LoadMultipleSamples(WfLexicalScopeManager* manager, const WString& sampleName)
{
	List<WString> itemNames;
	LoadSampleIndex(sampleName, itemNames);
	for (auto itemName : itemNames)
	{
		WString sample = LoadSample(sampleName, itemName);
		auto module = ParseModule(sample, GetWorkflowParser());
		manager->AddModule(module);

		auto sampleModule = GenerateToStream([&](StreamWriter& writer)
		{
			WfPrint(module, L"", writer);
		});
		LogSampleParseResult(sampleName, itemName, sampleModule, module);
	}
}

TEST_FILE
{
	TEST_CASE(L"Test building global name")
	{
		WfLexicalScopeManager manager(GetWorkflowParser(), testCpuArchitecture);
		LoadMultipleSamples(&manager, L"AnalyzerScope");
		manager.Rebuild(false);

		{
			auto parent = manager.globalName;
			auto name = parent->children[L"system"];
			TEST_ASSERT(name->typeDescriptor == 0);
			TEST_ASSERT(name->declarations.Count() == 0);
			TEST_ASSERT(name->parent == parent.Obj());
		}
		{
			auto parent = manager.globalName->children[L"system"];
			auto name = parent->children[L"Object"];
			TEST_ASSERT(name->typeDescriptor == GetTypeDescriptor<Value>());
			TEST_ASSERT(name->declarations.Count() == 0);
			TEST_ASSERT(name->parent == parent.Obj());
		}
		{
			auto parent = manager.globalName->children[L"system"]->children[L"reflection"];
			auto name = parent->children[L"TypeDescriptor"];
			TEST_ASSERT(name->typeDescriptor == GetTypeDescriptor<ITypeDescriptor>());
			TEST_ASSERT(name->declarations.Count() == 0);
			TEST_ASSERT(name->parent == parent.Obj());
		}
		{
			auto parent = manager.globalName;
			auto name = parent->children[L"test"];
			TEST_ASSERT(name->typeDescriptor == 0);
			TEST_ASSERT(name->declarations.Count() == 2);
			TEST_ASSERT(manager.namespaceNames[name->declarations[0].Cast<WfNamespaceDeclaration>().Obj()] == name);
			TEST_ASSERT(manager.namespaceNames[name->declarations[1].Cast<WfNamespaceDeclaration>().Obj()] == name);
			TEST_ASSERT(name->parent == parent.Obj());
		}
		{
			auto parent = manager.globalName->children[L"test"];
			auto name = parent->children[L"itoa"];
			TEST_ASSERT(name->typeDescriptor == 0);
			TEST_ASSERT(name->declarations.Count() == 1);
			TEST_ASSERT(name->parent == parent.Obj());

			auto function = name->declarations[0].Cast<WfFunctionDeclaration>();
			TEST_ASSERT(function->name.value == L"itoa");
		}
		{
			auto parent = manager.globalName;
			auto name = parent->children[L"main"];
			TEST_ASSERT(name->typeDescriptor == 0);
			TEST_ASSERT(name->declarations.Count() == 1);
			TEST_ASSERT(name->parent == parent.Obj());

			auto function = name->declarations[0].Cast<WfFunctionDeclaration>();
			TEST_ASSERT(function->name.value == L"main");
		}
	});

	TEST_CASE(L"RPC variant validation terminates on repeated and cyclic struct metadata")
	{
		using namespace vl::workflow::typeimpl;
		auto leaf = Ptr(new WfStruct(L"test::RpcMetadataLeaf"));
		auto node = Ptr(new WfStruct(L"test::RpcMetadataNode"));
		auto leafType = Ptr(new TypeDescriptorTypeInfo(leaf.Obj(), TypeInfoHint::Normal));
		auto nodeType = Ptr(new TypeDescriptorTypeInfo(node.Obj(), TypeInfoHint::Normal));
		auto childrenType = Ptr(new GenericTypeInfo(Ptr(new TypeDescriptorTypeInfo(GetTypeDescriptor<IValueList>(), TypeInfoHint::Normal))));
		childrenType->AddGenericArgument(nodeType);
		auto children = Ptr(new WfStructField(node.Obj(), L"children"));
		children->SetReturn(Ptr(new SharedPtrTypeInfo(childrenType)));
		node->AddMember(children);
		for (auto name : { L"first", L"second" })
		{
			auto field = Ptr(new WfStructField(node.Obj(), name));
			field->SetReturn(leafType);
			node->AddMember(field);
		}

		SortedList<ITypeDescriptor*> rpcInterfaces;
		auto rpcAttribute = GetTypeDescriptor<vl::__vwsn::att_rpc_Interface>();
		TEST_ASSERT(rpc_compiling::IsSerializableType(nodeType.Obj(), rpcAttribute, rpcInterfaces));

		auto payload = Ptr(new WfStructField(leaf.Obj(), L"payload"));
		payload->SetReturn(TypeInfoRetriver<Variant<vint, WString>>::CreateTypeInfo());
		leaf->AddMember(payload);
		TEST_ASSERT(!rpc_compiling::IsSerializableType(nodeType.Obj(), rpcAttribute, rpcInterfaces));
	});

	TEST_CATEGORY(L"Test against illegal scripts")
	{
		List<WString> itemNames;
		LoadSampleIndex(L"AnalyzerError", itemNames);

		WfLexicalScopeManager manager(GetWorkflowParser(), testCpuArchitecture);

		for (auto itemName : itemNames)
		{
			TEST_CASE(itemName)
			{
				manager.Clear(true, true);
				WString sample = LoadSample(L"AnalyzerError", itemName);
				auto module = ParseModule(sample, GetWorkflowParser());
				TEST_ASSERT(module);
				manager.AddModule(sample);
				manager.Rebuild(true);

				if (manager.errors.Count() == 0)
				{
					WfCppConfig(&manager, L"Assembly", L"Namespace");
				}

				auto sampleModule = GenerateToStream([&](StreamWriter& writer)
				{
					WfPrint(module, L"", writer);
				});
				LogSampleParseResult(L"AnalyzerError", itemName, sampleModule, module, &manager);

				const wchar_t* reading = itemName.Buffer();
				vint index = wcschr(reading, L'_') - reading;
				WString errorCode = itemName.Left(index);
				TEST_ASSERT(manager.errors.Count() > 0);
				TEST_ASSERT(manager.errors[0].message.Left(index + 1) == errorCode + L":");
			});
		}
	});
}
