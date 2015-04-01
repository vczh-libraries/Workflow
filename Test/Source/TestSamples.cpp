#include "TestFunctions.h"

TEST_CASE(TestSampleExpressions)
{
	Ptr<ParsingTable> table = GetWorkflowTable();
	List<WString> itemNames;
	LoadSampleIndex(L"Expression", itemNames);
	FOREACH(WString, itemName, itemNames)
	{
		UnitTest::PrintInfo(itemName);
		WString sample = LoadSample(L"Expression", itemName);
		List<Ptr<ParsingError>> errors;
		Ptr<ParsingTreeNode> node = WfParseExpressionAsParsingTreeNode(sample, table, errors);
		TEST_ASSERT(node);
		LogSampleParseResult(L"Expression", itemName, sample, node);
	}
}

TEST_CASE(TestSampleStatements)
{
	Ptr<ParsingTable> table = GetWorkflowTable();
	List<WString> itemNames;
	LoadSampleIndex(L"Statement", itemNames);
	FOREACH(WString, itemName, itemNames)
	{
		UnitTest::PrintInfo(itemName);
		WString sample = LoadSample(L"Statement", itemName);
		List<Ptr<ParsingError>> errors;
		Ptr<ParsingTreeNode> node = WfParseStatementAsParsingTreeNode(sample, table, errors);
		TEST_ASSERT(node);
		LogSampleParseResult(L"Statement", itemName, sample, node);
	}
}

TEST_CASE(TestSampleDeclarations)
{
	Ptr<ParsingTable> table = GetWorkflowTable();
	List<WString> itemNames;
	LoadSampleIndex(L"Declaration", itemNames);
	FOREACH(WString, itemName, itemNames)
	{
		UnitTest::PrintInfo(itemName);
		WString sample = LoadSample(L"Declaration", itemName);
		List<Ptr<ParsingError>> errors;
		Ptr<ParsingTreeNode> node = WfParseDeclarationAsParsingTreeNode(sample, table, errors);
		TEST_ASSERT(node);
		LogSampleParseResult(L"Declaration", itemName, sample, node);
	}
}

TEST_CASE(TestSampleModules)
{
	Ptr<ParsingTable> table = GetWorkflowTable();
	List<WString> itemNames;
	LoadSampleIndex(L"Module", itemNames);
	FOREACH(WString, itemName, itemNames)
	{
		UnitTest::PrintInfo(itemName);
		WString sample = LoadSample(L"Module", itemName);
		List<Ptr<ParsingError>> errors;
		Ptr<ParsingTreeNode> node = WfParseModuleAsParsingTreeNode(sample, table, errors);
		TEST_ASSERT(node);
		LogSampleParseResult(L"Module", itemName, sample, node);
	}
}