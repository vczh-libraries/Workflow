#ifndef WORKFLOW_UNITTEST_TESTFUNCTIONS
#define WORKFLOW_UNITTEST_TESTFUNCTIONS

#include "../../Source/Cpp/WfCpp.h"

using namespace vl;
using namespace vl::unittest;
using namespace vl::collections;
using namespace vl::stream;
using namespace vl::regex;
using namespace vl::parsing;
using namespace vl::parsing::tabling;
using namespace vl::workflow;
using namespace vl::workflow::analyzer;
using namespace vl::workflow::emitter;
using namespace vl::workflow::cppcodegen;
using namespace vl::workflow::runtime;
using namespace vl::reflection;
using namespace vl::reflection::description;
using namespace vl::filesystem;

extern Ptr<ParsingTable>	GetWorkflowTable();
extern void					ReleaseWorkflowTable();
extern WString				GetTestResourcePath();
extern WString				GetTestOutputPath();
extern WString				GetCppOutputPath();
extern WString				GetCppMergePath();
extern void					LoadSampleIndex(const WString& sampleName, List<WString>& itemNames);
extern WString				LoadSample(const WString& sampleName, const WString& itemName);
extern void					LogSampleParseResult(const WString& sampleName, const WString& itemName, const WString& sample, Ptr<ParsingTreeNode> node, Ptr<ParsingTreeCustomBase> typedNode, WfLexicalScopeManager* manager = 0);
extern void					LogSampleCodegenResult(const WString& sampleName, const WString& itemName, Ptr<WfAssembly> assembly);
extern void					LogSampleAssemblyBinary(const WString& sampleName, const WString& itemName, Ptr<WfAssembly>& assembly);
#endif