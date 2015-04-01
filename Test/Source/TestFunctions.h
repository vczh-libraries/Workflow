#ifndef WORKFLOW_UNITTEST_TESTFUNCTIONS
#define WORKFLOW_UNITTEST_TESTFUNCTIONS

#include "../../../../Common/Source/UnitTest/UnitTest.h"
#include "../../Source/Analyzer/WfAnalyzer.h"
#include "../../Source/Runtime/WfRuntime.h"

using namespace vl;
using namespace vl::unittest;
using namespace vl::collections;
using namespace vl::stream;
using namespace vl::regex;
using namespace vl::parsing;
using namespace vl::parsing::tabling;
using namespace vl::workflow;
using namespace vl::workflow::analyzer;
using namespace vl::workflow::runtime;
using namespace vl::reflection;
using namespace vl::reflection::description;

extern Ptr<ParsingTable>	GetWorkflowTable();
extern WString				GetPath();
extern void					LoadSampleIndex(const WString& sampleName, List<WString>& itemNames);
extern WString				LoadSample(const WString& sampleName, const WString& itemName);
extern void					LogSampleParseResult(const WString& sampleName, const WString& itemName, const WString& sample, Ptr<ParsingTreeNode> node, WfLexicalScopeManager* manager = 0);
extern void					LogSampleCodegenResult(const WString& sampleName, const WString& itemName, Ptr<WfAssembly> assembly);

#endif