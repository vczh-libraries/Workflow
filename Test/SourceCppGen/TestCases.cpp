#include "HelloWorld.h"
#include "OpNot.h"
#include "OpPositive.h"
#include "OpNegative.h"
#include "OpAdd.h"
#include "OpSub.h"
#include "OpMul.h"
#include "OpDiv.h"
#include "OpMod.h"
#include "OpShl.h"
#include "OpShr.h"
#include "OpAnd.h"
#include "OpOr.h"
#include "OpXor.h"
#include "OpCompareReflection.h"
#include "OpCompareReference.h"
#include "ListProcessing.h"
#include "MapProcessing.h"
#include "ObservableList.h"
#include "ElementInSet.h"
#include "GlobalVariable.h"
#include "RecursiveFunction.h"
#include "LetIn.h"
#include "Property.h"
#include "MethodClosure.h"
#include "StaticMethod.h"
#include "Event.h"
#include "ControlFlow.h"
#include "IfNotNull.h"
#include "ForEach.h"
#include "AnonymousLambda.h"
#include "NamedLambda.h"
#include "OrderedLambda.h"
#include "TryFinally.h"
#include "TryFinally2.h"
#include "TryCatch.h"
#include "Delete.h"
#include "FailedThen.h"
#include "NullableCastReflection.h"
#include "NewInterface.h"
#include "NewCustomInterfaceReflection.h"
#include "NewCustomInterface2Reflection.h"
#include "NewCustomInterface3Reflection.h"
#include "BindSimpleReflection.h"
#include "BindSimple2Reflection.h"
#include "BindLetReflection.h"
#include "BindComplex.h"
#include "BindFormat.h"
#include "BindCustomInterfaceReflection.h"
#include "BindCustomInterface2Reflection.h"
#include "BindNullReflection.h"
#include "ClassField.h"
#include "CallStaticMethodReflection.h"
#include "NestedLambdaReflection.h"
#include "ClassMethodReflection.h"
#include "ClassCtorReflection.h"
#include "ClassDtorReflection.h"
#include "StructCtorReflection.h"
#include "StructCtor2Reflection.h"
#include "EnumCtor.h"
#include "EnumCtor2Reflection.h"
#include "WorkflowHintsReflection.h"
#include "WorkflowAttributesReflection.h"
#include "WorkflowMultiFilesReflection.h"
#include "CoRawCoroutine.h"
#include "CoRawCoroutine2.h"
#include "CoRawCoroutine3.h"
#include "CoRawCoroutine4.h"
#include "CoRawCoroutine5.h"
#include "CoRawCoroutine6.h"
#include "CoRawCoroutine7.h"
#include "CoEnum.h"
#include "CoEnum2.h"
#include "CoAsyncReflection.h"
#include "CoAsync2Reflection.h"
#include "Overloading.h"
#include "CoSmcSwitchD1Reflection.h"
#include "CoSmcSwitchD2Reflection.h"
#include "CoSmcSwitchI1Reflection.h"
#include "CoSmcSwitchI2Reflection.h"
#include "CoSmcSwitchIR1Reflection.h"
#include "CoSmcSwitchIR2Reflection.h"
#include "CoSmcSwitchP1Reflection.h"
#include "CoSmcSwitchP2Reflection.h"
#include "CoSmcSwitchPR1Reflection.h"
#include "CoSmcSwitchPR2Reflection.h"
#include "CoSmcGotoReflection.h"
#include "CoSmcPushReflection.h"
#include "CoSmcTryCatch1Reflection.h"
#include "CoSmcTryCatch2Reflection.h"
#include "CoSmcCalculatorReflection.h"

using namespace vl;
using namespace vl::console;
using namespace vl::reflection::description;

void LoadTestCaseTypes()
{
	 LoadOpCompareTypes();
	 LoadNullableCastTypes();
	 LoadNewCustomInterfaceTypes();
	 LoadNewCustomInterface2Types();
	 LoadNewCustomInterface3Types();
	 LoadBindSimpleTypes();
	 LoadBindSimple2Types();
	 LoadBindLetTypes();
	 LoadBindCustomInterfaceTypes();
	 LoadBindCustomInterface2Types();
	 LoadBindNullTypes();
	 LoadCallStaticMethodTypes();
	 LoadNestedLambdaTypes();
	 LoadClassMethodTypes();
	 LoadClassCtorTypes();
	 LoadClassDtorTypes();
	 LoadStructCtorTypes();
	 LoadStructCtor2Types();
	 LoadEnumCtor2Types();
	 LoadWorkflowHintsTypes();
	 LoadWorkflowAttributesTypes();
	 LoadWorkflowMultiFilesTypes();
	 LoadCoAsyncTypes();
	 LoadCoAsync2Types();
	 LoadCoSmcSwitchD1Types();
	 LoadCoSmcSwitchD2Types();
	 LoadCoSmcSwitchI1Types();
	 LoadCoSmcSwitchI2Types();
	 LoadCoSmcSwitchIR1Types();
	 LoadCoSmcSwitchIR2Types();
	 LoadCoSmcSwitchP1Types();
	 LoadCoSmcSwitchP2Types();
	 LoadCoSmcSwitchPR1Types();
	 LoadCoSmcSwitchPR2Types();
	 LoadCoSmcGotoTypes();
	 LoadCoSmcPushTypes();
	 LoadCoSmcTryCatch1Types();
	 LoadCoSmcTryCatch2Types();
	 LoadCoSmcCalculatorTypes();
}

TEST_FILE
{

TEST_CASE(L"HelloWorld")
{
	WString expected = L"Hello, world!";
	WString actual = ::vl_workflow_global::HelloWorld::Instance().main();
	Console::WriteLine(L"    expected : " + expected);
	Console::WriteLine(L"    actual   : " + actual);
	TEST_ASSERT(actual == expected);
});

TEST_CASE(L"OpNot")
{
	WString expected = L"true, -2, -3, -5, -9, 254, 65533, 4294967291, 18446744073709551607";
	WString actual = ::vl_workflow_global::OpNot::Instance().main();
	Console::WriteLine(L"    expected : " + expected);
	Console::WriteLine(L"    actual   : " + actual);
	TEST_ASSERT(actual == expected);
});

TEST_CASE(L"OpPositive")
{
	WString expected = L"1, 2, 4, 8, 1, 2, 4, 8";
	WString actual = ::vl_workflow_global::OpPositive::Instance().main();
	Console::WriteLine(L"    expected : " + expected);
	Console::WriteLine(L"    actual   : " + actual);
	TEST_ASSERT(actual == expected);
});

TEST_CASE(L"OpNegative")
{
	WString expected = L"-1, -2, -4, -8";
	WString actual = ::vl_workflow_global::OpNegative::Instance().main();
	Console::WriteLine(L"    expected : " + expected);
	Console::WriteLine(L"    actual   : " + actual);
	TEST_ASSERT(actual == expected);
});

TEST_CASE(L"OpAdd")
{
	WString expected = L"3, 6, 12, 3, 6, 12, 4.5";
	WString actual = ::vl_workflow_global::OpAdd::Instance().main();
	Console::WriteLine(L"    expected : " + expected);
	Console::WriteLine(L"    actual   : " + actual);
	TEST_ASSERT(actual == expected);
});

TEST_CASE(L"OpSub")
{
	WString expected = L"-1, -2, -4, 4294967295, 4294967294, 18446744073709551612, -1.5";
	WString actual = ::vl_workflow_global::OpSub::Instance().main();
	Console::WriteLine(L"    expected : " + expected);
	Console::WriteLine(L"    actual   : " + actual);
	TEST_ASSERT(actual == expected);
});

TEST_CASE(L"OpMul")
{
	WString expected = L"2, 8, 32, 2, 8, 32, 4.5";
	WString actual = ::vl_workflow_global::OpMul::Instance().main();
	Console::WriteLine(L"    expected : " + expected);
	Console::WriteLine(L"    actual   : " + actual);
	TEST_ASSERT(actual == expected);
});

TEST_CASE(L"OpDiv")
{
	WString expected = L"0, 0, 0, 0, 0, 0, 0.5";
	WString actual = ::vl_workflow_global::OpDiv::Instance().main();
	Console::WriteLine(L"    expected : " + expected);
	Console::WriteLine(L"    actual   : " + actual);
	TEST_ASSERT(actual == expected);
});

TEST_CASE(L"OpMod")
{
	WString expected = L"1, 2, 4, 1, 2, 4";
	WString actual = ::vl_workflow_global::OpMod::Instance().main();
	Console::WriteLine(L"    expected : " + expected);
	Console::WriteLine(L"    actual   : " + actual);
	TEST_ASSERT(actual == expected);
});

TEST_CASE(L"OpShl")
{
	WString expected = L"4, 32, 1024, 4, 32, 1024";
	WString actual = ::vl_workflow_global::OpShl::Instance().main();
	Console::WriteLine(L"    expected : " + expected);
	Console::WriteLine(L"    actual   : " + actual);
	TEST_ASSERT(actual == expected);
});

TEST_CASE(L"OpShr")
{
	WString expected = L"0, 0, 0, 0, 0, 0";
	WString actual = ::vl_workflow_global::OpShr::Instance().main();
	Console::WriteLine(L"    expected : " + expected);
	Console::WriteLine(L"    actual   : " + actual);
	TEST_ASSERT(actual == expected);
});

TEST_CASE(L"OpAnd")
{
	WString expected = L"false, false, 0, 0, 0, 0, 0, 0";
	WString actual = ::vl_workflow_global::OpAnd::Instance().main();
	Console::WriteLine(L"    expected : " + expected);
	Console::WriteLine(L"    actual   : " + actual);
	TEST_ASSERT(actual == expected);
});

TEST_CASE(L"OpOr")
{
	WString expected = L"true, false, 3, 6, 12, 3, 6, 12";
	WString actual = ::vl_workflow_global::OpOr::Instance().main();
	Console::WriteLine(L"    expected : " + expected);
	Console::WriteLine(L"    actual   : " + actual);
	TEST_ASSERT(actual == expected);
});

TEST_CASE(L"OpXor")
{
	WString expected = L"true, false, 3, 6, 12, 3, 6, 12";
	WString actual = ::vl_workflow_global::OpXor::Instance().main();
	Console::WriteLine(L"    expected : " + expected);
	Console::WriteLine(L"    actual   : " + actual);
	TEST_ASSERT(actual == expected);
});

TEST_CASE(L"OpCompare")
{
	WString expected = L"true, false, false, true, false, true, true, false, false, false, false, false, true, true, true, true, false, false, false, true, true, false, false, false, true, false, false, true";
	WString actual = ::vl_workflow_global::OpCompare::Instance().main();
	Console::WriteLine(L"    expected : " + expected);
	Console::WriteLine(L"    actual   : " + actual);
	TEST_ASSERT(actual == expected);
});

TEST_CASE(L"OpCompareReference")
{
	WString expected = L"false, true, false, false, true, true, false, false, true, true, true";
	WString actual = ::vl_workflow_global::OpCompareReference::Instance().main();
	Console::WriteLine(L"    expected : " + expected);
	Console::WriteLine(L"    actual   : " + actual);
	TEST_ASSERT(actual == expected);
});

TEST_CASE(L"ListProcessing")
{
	WString expected = L"5, 1, 5, 6, 1, 6";
	WString actual = ::vl_workflow_global::ListProcessing::Instance().main();
	Console::WriteLine(L"    expected : " + expected);
	Console::WriteLine(L"    actual   : " + actual);
	TEST_ASSERT(actual == expected);
});

TEST_CASE(L"MapProcessing")
{
	WString expected = L"5, 1, 5, 6, 1, 6";
	WString actual = ::vl_workflow_global::MapProcessing::Instance().main();
	Console::WriteLine(L"    expected : " + expected);
	Console::WriteLine(L"    actual   : " + actual);
	TEST_ASSERT(actual == expected);
});

TEST_CASE(L"ObservableList")
{
	WString expected = L"[1:0:1][3:0:1][5:0:1][7:0:1][9:0:1][0:1:0][1:1:0][2:1:0][3:1:0][4:1:0][2][4][6][8][10][0:5:0]";
	WString actual = ::vl_workflow_global::ObservableList::Instance().main();
	Console::WriteLine(L"    expected : " + expected);
	Console::WriteLine(L"    actual   : " + actual);
	TEST_ASSERT(actual == expected);
});

TEST_CASE(L"ElementInSet")
{
	WString expected = L"true, false, true, false, true, false, false, true, true, false";
	WString actual = ::vl_workflow_global::ElementInSet::Instance().main();
	Console::WriteLine(L"    expected : " + expected);
	Console::WriteLine(L"    actual   : " + actual);
	TEST_ASSERT(actual == expected);
});

TEST_CASE(L"GlobalVariable")
{
	WString expected = L"30, 10";
	WString actual = ::vl_workflow_global::GlobalVariable::Instance().main();
	Console::WriteLine(L"    expected : " + expected);
	Console::WriteLine(L"    actual   : " + actual);
	TEST_ASSERT(actual == expected);
});

TEST_CASE(L"RecursiveFunction")
{
	WString expected = L"1, 1, 2, 3, 5";
	WString actual = ::vl_workflow_global::RecursiveFunction::Instance().main();
	Console::WriteLine(L"    expected : " + expected);
	Console::WriteLine(L"    actual   : " + actual);
	TEST_ASSERT(actual == expected);
});

TEST_CASE(L"LetIn")
{
	WString expected = L"30";
	WString actual = ::vl_workflow_global::LetIn::Instance().main();
	Console::WriteLine(L"    expected : " + expected);
	Console::WriteLine(L"    actual   : " + actual);
	TEST_ASSERT(actual == expected);
});

TEST_CASE(L"Property")
{
	WString expected = L"10, 20, 10, , This is , 20, Workflow, This is Workflow";
	WString actual = ::vl_workflow_global::Property::Instance().main();
	Console::WriteLine(L"    expected : " + expected);
	Console::WriteLine(L"    actual   : " + actual);
	TEST_ASSERT(actual == expected);
});

TEST_CASE(L"MethodClosure")
{
	WString expected = L"10, 20";
	WString actual = ::vl_workflow_global::MethodClosure::Instance().main();
	Console::WriteLine(L"    expected : " + expected);
	Console::WriteLine(L"    actual   : " + actual);
	TEST_ASSERT(actual == expected);
});

TEST_CASE(L"StaticMethod")
{
	WString expected = L"1, This is one, 2, This is two";
	WString actual = ::vl_workflow_global::StaticMethod::Instance().main();
	Console::WriteLine(L"    expected : " + expected);
	Console::WriteLine(L"    actual   : " + actual);
	TEST_ASSERT(actual == expected);
});

TEST_CASE(L"Event")
{
	WString expected = L"2, 10, 20, 2, 20, 30, true, false";
	WString actual = ::vl_workflow_global::Event::Instance().main();
	Console::WriteLine(L"    expected : " + expected);
	Console::WriteLine(L"    actual   : " + actual);
	TEST_ASSERT(actual == expected);
});

TEST_CASE(L"ControlFlow")
{
	WString expected = L"55, 55, 55, 55, 55";
	WString actual = ::vl_workflow_global::ControlFlow::Instance().main();
	Console::WriteLine(L"    expected : " + expected);
	Console::WriteLine(L"    actual   : " + actual);
	TEST_ASSERT(actual == expected);
});

TEST_CASE(L"IfNotNull")
{
	WString expected = L"{1} is a list, {1:2} is not a list";
	WString actual = ::vl_workflow_global::IfNotNull::Instance().main();
	Console::WriteLine(L"    expected : " + expected);
	Console::WriteLine(L"    actual   : " + actual);
	TEST_ASSERT(actual == expected);
});

TEST_CASE(L"ForEach")
{
	WString expected = L"[1, 2, 3, 4, 5][5, 4, 3, 2, 1]";
	WString actual = ::vl_workflow_global::ForEach::Instance().main();
	Console::WriteLine(L"    expected : " + expected);
	Console::WriteLine(L"    actual   : " + actual);
	TEST_ASSERT(actual == expected);
});

TEST_CASE(L"AnonymousLambda")
{
	WString expected = L"2, 3, 3, 4";
	WString actual = ::vl_workflow_global::AnonymousLambda::Instance().main();
	Console::WriteLine(L"    expected : " + expected);
	Console::WriteLine(L"    actual   : " + actual);
	TEST_ASSERT(actual == expected);
});

TEST_CASE(L"NamedLambda")
{
	WString expected = L"1, 1, 2, 3, 5";
	WString actual = ::vl_workflow_global::NamedLambda::Instance().main();
	Console::WriteLine(L"    expected : " + expected);
	Console::WriteLine(L"    actual   : " + actual);
	TEST_ASSERT(actual == expected);
});

TEST_CASE(L"OrderedLambda")
{
	WString expected = L"2, 3, 3, 4";
	WString actual = ::vl_workflow_global::OrderedLambda::Instance().main();
	Console::WriteLine(L"    expected : " + expected);
	Console::WriteLine(L"    actual   : " + actual);
	TEST_ASSERT(actual == expected);
});

TEST_CASE(L"TryFinally")
{
	WString expected = L"3115";
	WString actual = ::vl_workflow_global::TryFinally::Instance().main();
	Console::WriteLine(L"    expected : " + expected);
	Console::WriteLine(L"    actual   : " + actual);
	TEST_ASSERT(actual == expected);
});

TEST_CASE(L"TryFinally2")
{
	WString expected = L"3115";
	WString actual = ::vl_workflow_global::TryFinally2::Instance().main();
	Console::WriteLine(L"    expected : " + expected);
	Console::WriteLine(L"    actual   : " + actual);
	TEST_ASSERT(actual == expected);
});

TEST_CASE(L"TryCatch")
{
	WString expected = L"[Test1::catch][Test2::catch][Test2::finally][Test3::catch1][Test3::finally1][Test3::catch2][Test3::finally2][Test4::finally1][Test4::finally2]";
	WString actual = ::vl_workflow_global::TryCatch::Instance().main();
	Console::WriteLine(L"    expected : " + expected);
	Console::WriteLine(L"    actual   : " + actual);
	TEST_ASSERT(actual == expected);
});

TEST_CASE(L"Delete")
{
	WString expected = L"false, false, false, false, true";
	WString actual = ::vl_workflow_global::Delete::Instance().main();
	Console::WriteLine(L"    expected : " + expected);
	Console::WriteLine(L"    actual   : " + actual);
	TEST_ASSERT(actual == expected);
});

TEST_CASE(L"FailedThen")
{
	WString expected = L"-1, 110";
	WString actual = ::vl_workflow_global::FailedThen::Instance().main();
	Console::WriteLine(L"    expected : " + expected);
	Console::WriteLine(L"    actual   : " + actual);
	TEST_ASSERT(actual == expected);
});

TEST_CASE(L"NullableCast")
{
	WString expected = L"null, 0";
	WString actual = ::vl_workflow_global::NullableCast::Instance().main();
	Console::WriteLine(L"    expected : " + expected);
	Console::WriteLine(L"    actual   : " + actual);
	TEST_ASSERT(actual == expected);
});

TEST_CASE(L"NewInterface")
{
	WString expected = L"[1, 2, 3, 4, 5][5, 4, 3, 2, 1]";
	WString actual = ::vl_workflow_global::NewInterface::Instance().main();
	Console::WriteLine(L"    expected : " + expected);
	Console::WriteLine(L"    actual   : " + actual);
	TEST_ASSERT(actual == expected);
});

TEST_CASE(L"NewCustomInterface")
{
	WString expected = L"[100][15][15]";
	WString actual = ::vl_workflow_global::NewCustomInterface::Instance().main();
	Console::WriteLine(L"    expected : " + expected);
	Console::WriteLine(L"    actual   : " + actual);
	TEST_ASSERT(actual == expected);
});

TEST_CASE(L"NewCustomInterface2")
{
	WString expected = L"[100][15][15]";
	WString actual = ::vl_workflow_global::NewCustomInterface2::Instance().main();
	Console::WriteLine(L"    expected : " + expected);
	Console::WriteLine(L"    actual   : " + actual);
	TEST_ASSERT(actual == expected);
});

TEST_CASE(L"NewCustomInterface3")
{
	WString expected = L"[100][15][15]";
	WString actual = ::vl_workflow_global::NewCustomInterface3::Instance().main();
	Console::WriteLine(L"    expected : " + expected);
	Console::WriteLine(L"    actual   : " + actual);
	TEST_ASSERT(actual == expected);
});

TEST_CASE(L"BindSimple")
{
	WString expected = L"[10][30][60]";
	WString actual = ::vl_workflow_global::BindSimple::Instance().main();
	Console::WriteLine(L"    expected : " + expected);
	Console::WriteLine(L"    actual   : " + actual);
	TEST_ASSERT(actual == expected);
});

TEST_CASE(L"BindSimple2")
{
	WString expected = L"[10][30][60]";
	WString actual = ::vl_workflow_global::BindSimple2::Instance().main();
	Console::WriteLine(L"    expected : " + expected);
	Console::WriteLine(L"    actual   : " + actual);
	TEST_ASSERT(actual == expected);
});

TEST_CASE(L"BindLet")
{
	WString expected = L"[1][2][22][42][42][342][642]";
	WString actual = ::vl_workflow_global::BindLet::Instance().main();
	Console::WriteLine(L"    expected : " + expected);
	Console::WriteLine(L"    actual   : " + actual);
	TEST_ASSERT(actual == expected);
});

TEST_CASE(L"BindComplex")
{
	WString expected = L"[10][30][60]";
	WString actual = ::vl_workflow_global::BindComplex::Instance().main();
	Console::WriteLine(L"    expected : " + expected);
	Console::WriteLine(L"    actual   : " + actual);
	TEST_ASSERT(actual == expected);
});

TEST_CASE(L"BindFormat")
{
	WString expected = L"[The value has changed to 10][The value has changed to 20][The value has changed to 30]";
	WString actual = ::vl_workflow_global::BindFormat::Instance().main();
	Console::WriteLine(L"    expected : " + expected);
	Console::WriteLine(L"    actual   : " + actual);
	TEST_ASSERT(actual == expected);
});

TEST_CASE(L"BindCustomInterface")
{
	WString expected = L"[1][3][6][10][15]";
	WString actual = ::vl_workflow_global::BindCustomInterface::Instance().main();
	Console::WriteLine(L"    expected : " + expected);
	Console::WriteLine(L"    actual   : " + actual);
	TEST_ASSERT(actual == expected);
});

TEST_CASE(L"BindCustomInterface2")
{
	WString expected = L"[1][3][6][10][15]";
	WString actual = ::vl_workflow_global::BindCustomInterface2::Instance().main();
	Console::WriteLine(L"    expected : " + expected);
	Console::WriteLine(L"    actual   : " + actual);
	TEST_ASSERT(actual == expected);
});

TEST_CASE(L"BindNull")
{
	WString expected = L"[*][0][100][*][*]";
	WString actual = ::vl_workflow_global::BindNull::Instance().main();
	Console::WriteLine(L"    expected : " + expected);
	Console::WriteLine(L"    actual   : " + actual);
	TEST_ASSERT(actual == expected);
});

TEST_CASE(L"ClassField")
{
	WString expected = L"0, 0, 1, 2, true, false";
	WString actual = ::vl_workflow_global::ClassField::Instance().main();
	Console::WriteLine(L"    expected : " + expected);
	Console::WriteLine(L"    actual   : " + actual);
	TEST_ASSERT(actual == expected);
});

TEST_CASE(L"CallStaticMethod")
{
	WString expected = L"55";
	WString actual = ::vl_workflow_global::CallStaticMethod::Instance().main();
	Console::WriteLine(L"    expected : " + expected);
	Console::WriteLine(L"    actual   : " + actual);
	TEST_ASSERT(actual == expected);
});

TEST_CASE(L"NestedLambda")
{
	WString expected = L"[1][2][[4][5][[3, 12]]]";
	WString actual = ::vl_workflow_global::NestedLambda::Instance().main();
	Console::WriteLine(L"    expected : " + expected);
	Console::WriteLine(L"    actual   : " + actual);
	TEST_ASSERT(actual == expected);
});

TEST_CASE(L"ClassMethod")
{
	WString expected = L"[0][100][2][1][100]";
	WString actual = ::vl_workflow_global::ClassMethod::Instance().main();
	Console::WriteLine(L"    expected : " + expected);
	Console::WriteLine(L"    actual   : " + actual);
	TEST_ASSERT(actual == expected);
});

TEST_CASE(L"ClassCtor")
{
	WString expected = L"[0][0][1][0][0][2][1][2]";
	WString actual = ::vl_workflow_global::ClassCtor::Instance().main();
	Console::WriteLine(L"    expected : " + expected);
	Console::WriteLine(L"    actual   : " + actual);
	TEST_ASSERT(actual == expected);
});

TEST_CASE(L"ClassDtor")
{
	WString expected = L"[x][y][z]";
	WString actual = ::vl_workflow_global::ClassDtor::Instance().main();
	Console::WriteLine(L"    expected : " + expected);
	Console::WriteLine(L"    actual   : " + actual);
	TEST_ASSERT(actual == expected);
});

TEST_CASE(L"StructCtor")
{
	WString expected = L"21";
	WString actual = ::vl_workflow_global::StructCtor::Instance().main();
	Console::WriteLine(L"    expected : " + expected);
	Console::WriteLine(L"    actual   : " + actual);
	TEST_ASSERT(actual == expected);
});

TEST_CASE(L"StructCtor2")
{
	WString expected = L"21";
	WString actual = ::vl_workflow_global::StructCtor2::Instance().main();
	Console::WriteLine(L"    expected : " + expected);
	Console::WriteLine(L"    actual   : " + actual);
	TEST_ASSERT(actual == expected);
});

TEST_CASE(L"EnumCtor")
{
	WString expected = L"1, 2, 4, 8, 3, 1, 0";
	WString actual = ::vl_workflow_global::EnumCtor::Instance().main();
	Console::WriteLine(L"    expected : " + expected);
	Console::WriteLine(L"    actual   : " + actual);
	TEST_ASSERT(actual == expected);
});

TEST_CASE(L"EnumCtor2")
{
	WString expected = L"1, 2, 4, 8, 3, 1, 0";
	WString actual = ::vl_workflow_global::EnumCtor2::Instance().main();
	Console::WriteLine(L"    expected : " + expected);
	Console::WriteLine(L"    actual   : " + actual);
	TEST_ASSERT(actual == expected);
});

TEST_CASE(L"WorkflowHints")
{
	WString expected = L"[1][10][100][1000][2][20][1][100][3][4]";
	WString actual = ::vl_workflow_global::WorkflowHints::Instance().main();
	Console::WriteLine(L"    expected : " + expected);
	Console::WriteLine(L"    actual   : " + actual);
	TEST_ASSERT(actual == expected);
});

TEST_CASE(L"WorkflowAttributes")
{
	WString expected = L"((1 + 2) * (3 + 4)) = 21";
	WString actual = ::vl_workflow_global::WorkflowAttributes::Instance().main();
	Console::WriteLine(L"    expected : " + expected);
	Console::WriteLine(L"    actual   : " + actual);
	TEST_ASSERT(actual == expected);
});

TEST_CASE(L"WorkflowMultiFiles")
{
	WString expected = L"MultiFiles";
	WString actual = ::vl_workflow_global::WorkflowMultiFiles::Instance().main();
	Console::WriteLine(L"    expected : " + expected);
	Console::WriteLine(L"    actual   : " + actual);
	TEST_ASSERT(actual == expected);
});

TEST_CASE(L"CoRawCoroutine")
{
	WString expected = L"[+0][-0][+1][-1][+2][-2][+3][-3][+4][-4][+Enough!][-5]";
	WString actual = ::vl_workflow_global::CoRawCoroutine::Instance().main();
	Console::WriteLine(L"    expected : " + expected);
	Console::WriteLine(L"    actual   : " + actual);
	TEST_ASSERT(actual == expected);
});

TEST_CASE(L"CoRawCoroutine2")
{
	WString expected = L"[+0][-0]![*]![+2][-1]![*]![-2]";
	WString actual = ::vl_workflow_global::CoRawCoroutine2::Instance().main();
	Console::WriteLine(L"    expected : " + expected);
	Console::WriteLine(L"    actual   : " + actual);
	TEST_ASSERT(actual == expected);
});

TEST_CASE(L"CoRawCoroutine3")
{
	WString expected = L"[+0][-0]!![-1][*]";
	WString actual = ::vl_workflow_global::CoRawCoroutine3::Instance().main();
	Console::WriteLine(L"    expected : " + expected);
	Console::WriteLine(L"    actual   : " + actual);
	TEST_ASSERT(actual == expected);
});

TEST_CASE(L"CoRawCoroutine4")
{
	WString expected = L"[+0][-0][+1][-1][-2][*]";
	WString actual = ::vl_workflow_global::CoRawCoroutine4::Instance().main();
	Console::WriteLine(L"    expected : " + expected);
	Console::WriteLine(L"    actual   : " + actual);
	TEST_ASSERT(actual == expected);
});

TEST_CASE(L"CoRawCoroutine5")
{
	WString expected = L"[+0][-0][+1][-1][+2][-2][+3][-3][+4][-4][+Enough!][-5]";
	WString actual = ::vl_workflow_global::CoRawCoroutine5::Instance().main();
	Console::WriteLine(L"    expected : " + expected);
	Console::WriteLine(L"    actual   : " + actual);
	TEST_ASSERT(actual == expected);
});

TEST_CASE(L"CoRawCoroutine6")
{
	WString expected = L"[+0][-0][+1][-1][+2][-2][+3][-3][+4][-4][+Enough!][-5]";
	WString actual = ::vl_workflow_global::CoRawCoroutine6::Instance().main();
	Console::WriteLine(L"    expected : " + expected);
	Console::WriteLine(L"    actual   : " + actual);
	TEST_ASSERT(actual == expected);
});

TEST_CASE(L"CoRawCoroutine7")
{
	WString expected = L"[+0][-0][+1][-1][+2][-2][+3][-3][+4][-4][+Enough!][-5]";
	WString actual = ::vl_workflow_global::CoRawCoroutine7::Instance().main();
	Console::WriteLine(L"    expected : " + expected);
	Console::WriteLine(L"    actual   : " + actual);
	TEST_ASSERT(actual == expected);
});

TEST_CASE(L"CoEnum")
{
	WString expected = L"[0][1][2][3][4]";
	WString actual = ::vl_workflow_global::CoEnum::Instance().main();
	Console::WriteLine(L"    expected : " + expected);
	Console::WriteLine(L"    actual   : " + actual);
	TEST_ASSERT(actual == expected);
});

TEST_CASE(L"CoEnum2")
{
	WString expected = L"[0][1][2][1][2][3][2][3][4]";
	WString actual = ::vl_workflow_global::CoEnum2::Instance().main();
	Console::WriteLine(L"    expected : " + expected);
	Console::WriteLine(L"    actual   : " + actual);
	TEST_ASSERT(actual == expected);
});

TEST_CASE(L"CoAsync")
{
	WString expected = L"[0][+0][0][-0]![1][+1][1][-1]!";
	WString actual = ::vl_workflow_global::CoAsync::Instance().main();
	Console::WriteLine(L"    expected : " + expected);
	Console::WriteLine(L"    actual   : " + actual);
	TEST_ASSERT(actual == expected);
});

TEST_CASE(L"CoAsync2")
{
	WString expected = L"[1][2][3][4][5][Cancelled]";
	WString actual = ::vl_workflow_global::CoAsync2::Instance().main();
	Console::WriteLine(L"    expected : " + expected);
	Console::WriteLine(L"    actual   : " + actual);
	TEST_ASSERT(actual == expected);
});

TEST_CASE(L"Overloading")
{
	WString expected = L"[2][2][2][2][2][2][2][2][2][2]";
	WString actual = ::vl_workflow_global::Overloading::Instance().main();
	Console::WriteLine(L"    expected : " + expected);
	Console::WriteLine(L"    actual   : " + actual);
	TEST_ASSERT(actual == expected);
});

TEST_CASE(L"CoSmcSwitchD1")
{
	WString expected = L"[A].";
	WString actual = ::vl_workflow_global::CoSmcSwitchD1::Instance().main();
	Console::WriteLine(L"    expected : " + expected);
	Console::WriteLine(L"    actual   : " + actual);
	TEST_ASSERT(actual == expected);
});

TEST_CASE(L"CoSmcSwitchD2")
{
	WString expected = L"!";
	WString actual = ::vl_workflow_global::CoSmcSwitchD2::Instance().main();
	Console::WriteLine(L"    expected : " + expected);
	Console::WriteLine(L"    actual   : " + actual);
	TEST_ASSERT(actual == expected);
});

TEST_CASE(L"CoSmcSwitchI1")
{
	WString expected = L"[A].";
	WString actual = ::vl_workflow_global::CoSmcSwitchI1::Instance().main();
	Console::WriteLine(L"    expected : " + expected);
	Console::WriteLine(L"    actual   : " + actual);
	TEST_ASSERT(actual == expected);
});

TEST_CASE(L"CoSmcSwitchI2")
{
	WString expected = L".";
	WString actual = ::vl_workflow_global::CoSmcSwitchI2::Instance().main();
	Console::WriteLine(L"    expected : " + expected);
	Console::WriteLine(L"    actual   : " + actual);
	TEST_ASSERT(actual == expected);
});

TEST_CASE(L"CoSmcSwitchIR1")
{
	WString expected = L"[A].";
	WString actual = ::vl_workflow_global::CoSmcSwitchIR1::Instance().main();
	Console::WriteLine(L"    expected : " + expected);
	Console::WriteLine(L"    actual   : " + actual);
	TEST_ASSERT(actual == expected);
});

TEST_CASE(L"CoSmcSwitchIR2")
{
	WString expected = L"";
	WString actual = ::vl_workflow_global::CoSmcSwitchIR2::Instance().main();
	Console::WriteLine(L"    expected : " + expected);
	Console::WriteLine(L"    actual   : " + actual);
	TEST_ASSERT(actual == expected);
});

TEST_CASE(L"CoSmcSwitchP1")
{
	WString expected = L"[A][B].";
	WString actual = ::vl_workflow_global::CoSmcSwitchP1::Instance().main();
	Console::WriteLine(L"    expected : " + expected);
	Console::WriteLine(L"    actual   : " + actual);
	TEST_ASSERT(actual == expected);
});

TEST_CASE(L"CoSmcSwitchP2")
{
	WString expected = L"[B].!";
	WString actual = ::vl_workflow_global::CoSmcSwitchP2::Instance().main();
	Console::WriteLine(L"    expected : " + expected);
	Console::WriteLine(L"    actual   : " + actual);
	TEST_ASSERT(actual == expected);
});

TEST_CASE(L"CoSmcSwitchPR1")
{
	WString expected = L"[A][B].";
	WString actual = ::vl_workflow_global::CoSmcSwitchPR1::Instance().main();
	Console::WriteLine(L"    expected : " + expected);
	Console::WriteLine(L"    actual   : " + actual);
	TEST_ASSERT(actual == expected);
});

TEST_CASE(L"CoSmcSwitchPR2")
{
	WString expected = L"!";
	WString actual = ::vl_workflow_global::CoSmcSwitchPR2::Instance().main();
	Console::WriteLine(L"    expected : " + expected);
	Console::WriteLine(L"    actual   : " + actual);
	TEST_ASSERT(actual == expected);
});

TEST_CASE(L"CoSmcGoto")
{
	WString expected = L"[A].A!";
	WString actual = ::vl_workflow_global::CoSmcGoto::Instance().main();
	Console::WriteLine(L"    expected : " + expected);
	Console::WriteLine(L"    actual   : " + actual);
	TEST_ASSERT(actual == expected);
});

TEST_CASE(L"CoSmcPush")
{
	WString expected = L"[A].A[B].B.";
	WString actual = ::vl_workflow_global::CoSmcPush::Instance().main();
	Console::WriteLine(L"    expected : " + expected);
	Console::WriteLine(L"    actual   : " + actual);
	TEST_ASSERT(actual == expected);
});

TEST_CASE(L"CoSmcTryCatch1")
{
	WString expected = L"[A].A[B].B.";
	WString actual = ::vl_workflow_global::CoSmcTryCatch1::Instance().main();
	Console::WriteLine(L"    expected : " + expected);
	Console::WriteLine(L"    actual   : " + actual);
	TEST_ASSERT(actual == expected);
});

TEST_CASE(L"CoSmcTryCatch2")
{
	WString expected = L"!A!B.";
	WString actual = ::vl_workflow_global::CoSmcTryCatch2::Instance().main();
	Console::WriteLine(L"    expected : " + expected);
	Console::WriteLine(L"    actual   : " + actual);
	TEST_ASSERT(actual == expected);
});

TEST_CASE(L"CoSmcCalculator")
{
	WString expected = L"[0][1][1.][1.5][2][21][21.][21.2][21.25][22.75][2][45.5][0]";
	WString actual = ::vl_workflow_global::CoSmcCalculator::Instance().main();
	Console::WriteLine(L"    expected : " + expected);
	Console::WriteLine(L"    actual   : " + actual);
	TEST_ASSERT(actual == expected);
});
}
