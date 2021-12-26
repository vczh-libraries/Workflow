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

	for (vint i = 0; i < 4; i++)
	{
		auto category = categories[i];
		TEST_CATEGORY(L"Test against legal script: " + WString(category))
		{
			auto&& parser = GetWorkflowParser();
			List<WString> itemNames;
			LoadSampleIndex(category, itemNames);
			for (auto itemName : itemNames)
			{
				TEST_CASE(itemName)
				{
					TEST_PRINT(itemName);
					WString sample = LoadSample(category, itemName);

					List<glr::ParsingError> errors;
					auto handler = glr::InstallDefaultErrorMessageGenerator(parser, errors);
					Ptr<glr::ParsingAstBase> node;
					switch (i)
					{
					case 0:
						node = ParseExpression(sample, parser);
						break;
					case 1:
						node = ParseStatement(sample, parser);
						break;
					case 2:
						node = ParseDeclaration(sample, parser);
						break;
					case 3:
						node = ParseModule(sample, parser);
						break;
					}
					parser.OnError.Remove(handler);
					TEST_ASSERT(node);

					LogSampleParseResult(category, itemName, sample, node);
				});
			}
		});
	}
}