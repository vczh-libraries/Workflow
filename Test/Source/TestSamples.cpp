#include "Helper.h"

TEST_FILE
{
	const wchar_t* categories[] =
	{
		L"Expression",
		L"Statement",
		L"Declaration",
		L"Module"
	};

	using ParseProc = Ptr<ParsingTreeNode>(*)(const WString&, Ptr<ParsingTable>, List<Ptr<ParsingError>>&, vint);
	const ParseProc parseProcs[] =
	{
		WfParseExpressionAsParsingTreeNode,
		WfParseStatementAsParsingTreeNode,
		WfParseDeclarationAsParsingTreeNode,
		WfParseModuleAsParsingTreeNode
	};

	for (vint i = 0; i < 4; i++)
	{
		auto category = categories[i];
		auto parseProc = parseProcs[i];
		TEST_CATEGORY(L"Test against legal script: " + WString(category))
		{
			Ptr<ParsingTable> table = GetWorkflowTable();
			List<WString> itemNames;
			LoadSampleIndex(category, itemNames);
			FOREACH(WString, itemName, itemNames)
			{
				TEST_PRINT(itemName);
				WString sample = LoadSample(category, itemName);
				List<Ptr<ParsingError>> errors;
				Ptr<ParsingTreeNode> node = parseProc(sample, table, errors, -1);
				TEST_ASSERT(node);

				List<RegexToken> tokens;
				auto typedNode = WfConvertParsingTreeNode(node, tokens);
				LogSampleParseResult(category, itemName, sample, node, typedNode);
			}
		});
	}
}