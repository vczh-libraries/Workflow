#include "../../Source/Helper.h"

TEST_FILE
{
	TEST_CASE(L"Indexed variant syntax survives copying and printed round trips")
	{
		auto&& parser = GetWorkflowParser();
		auto print = [](auto node)
		{
			return stream::GenerateToStream([&](TextWriter& writer)
			{
				WfPrint(node, L"", writer);
			});
		};
		List<glr::ParsingError> errors;
		auto handler = glr::InstallDefaultErrorMessageGenerator(parser, errors);
		auto declaration = ParseDeclaration(LoadSample(L"Declaration", L"VariantTypes"), parser);
		auto copiedDeclaration = CopyDeclaration(declaration, false);
		auto declarationText = print(copiedDeclaration);
		TEST_ASSERT(declarationText == print(declaration));
		TEST_ASSERT(declarationText == print(ParseDeclaration(declarationText, parser)));

		auto expression = ParseExpression(LoadSample(L"Expression", L"VariantType"), parser);
		auto copiedExpression = CopyExpression(expression, false);
		auto expressionText = print(copiedExpression);
		TEST_ASSERT(expressionText == print(expression));
		TEST_ASSERT(expressionText == print(ParseExpression(expressionText, parser)));
		parser.OnError.Remove(handler);
		TEST_ASSERT(errors.Count() == 0);
	});

	TEST_CATEGORY(L"Test malformed variant type syntax")
	{
		auto&& parser = GetWorkflowParser();
		List<WString> itemNames;
		LoadSampleIndex(L"InvalidType", itemNames);
		for (auto itemName : itemNames)
		{
			TEST_CASE(itemName)
			{
				List<glr::ParsingError> errors;
				auto handler = glr::InstallDefaultErrorMessageGenerator(parser, errors);
				ParseType(LoadSample(L"InvalidType", itemName), parser);
				parser.OnError.Remove(handler);
				TEST_ASSERT(errors.Count() > 0);
			});
		}
	});

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
