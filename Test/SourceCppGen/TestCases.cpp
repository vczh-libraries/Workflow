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
#include "NewInterface.h"
#include "NewCustomInterfaceReflection.h"
#include "NewCustomInterface2Reflection.h"
#include "NewCustomInterface3Reflection.h"
#include "BindSimpleReflection.h"
#include "BindLetReflection.h"
#include "BindComplex.h"
#include "BindFormat.h"
#include "BindCustomInterfaceReflection.h"
#include "BindCustomInterface2Reflection.h"
#include "ClassField.h"
#include "CallStaticMethodReflection.h"
#include "NestedLambdaReflection.h"
#include "ClassMethodReflection.h"
#include "ClassCtorReflection.h"
#include "ClassDtorReflection.h"
#include "StructCtor.h"
#include "StructCtor2Reflection.h"
#include "EnumCtor.h"
#include "EnumCtor2Reflection.h"
#include "WorkflowHintsReflection.h"
#include "WorkflowAttributesReflection.h"
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
	 LoadNewCustomInterfaceTypes();
	 LoadNewCustomInterface2Types();
	 LoadNewCustomInterface3Types();
	 LoadBindSimpleTypes();
	 LoadBindLetTypes();
	 LoadBindCustomInterfaceTypes();
	 LoadBindCustomInterface2Types();
	 LoadCallStaticMethodTypes();
	 LoadNestedLambdaTypes();
	 LoadClassMethodTypes();
	 LoadClassCtorTypes();
	 LoadClassDtorTypes();
	 LoadStructCtor2Types();
	 LoadEnumCtor2Types();
	 LoadWorkflowHintsTypes();
	 LoadWorkflowAttributesTypes();
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

TEST_CASE(HelloWorld)
{
	WString expected = L"Hello, world!";
	WString actual = ::vl_workflow_global::HelloWorld::Instance().main();
	Console::WriteLine(L"    expected : " + expected);
	Console::WriteLine(L"    actual   : " + actual);
	TEST_ASSERT(actual == expected);
}

TEST_CASE(OpNot)
{
	WString expected = L"true, -2, -3, -5, -9, 254, 65533, 4294967291, 18446744073709551607";
	WString actual = ::vl_workflow_global::OpNot::Instance().main();
	Console::WriteLine(L"    expected : " + expected);
	Console::WriteLine(L"    actual   : " + actual);
	TEST_ASSERT(actual == expected);
}

TEST_CASE(OpPositive)
{
	WString expected = L"1, 2, 4, 8, 1, 2, 4, 8";
	WString actual = ::vl_workflow_global::OpPositive::Instance().main();
	Console::WriteLine(L"    expected : " + expected);
	Console::WriteLine(L"    actual   : " + actual);
	TEST_ASSERT(actual == expected);
}

TEST_CASE(OpNegative)
{
	WString expected = L"-1, -2, -4, -8";
	WString actual = ::vl_workflow_global::OpNegative::Instance().main();
	Console::WriteLine(L"    expected : " + expected);
	Console::WriteLine(L"    actual   : " + actual);
	TEST_ASSERT(actual == expected);
}

TEST_CASE(OpAdd)
{
	WString expected = L"3, 6, 12, 3, 6, 12, 4.5";
	WString actual = ::vl_workflow_global::OpAdd::Instance().main();
	Console::WriteLine(L"    expected : " + expected);
	Console::WriteLine(L"    actual   : " + actual);
	TEST_ASSERT(actual == expected);
}

TEST_CASE(OpSub)
{
	WString expected = L"-1, -2, -4, 4294967295, 4294967294, 18446744073709551612, -1.5";
	WString actual = ::vl_workflow_global::OpSub::Instance().main();
	Console::WriteLine(L"    expected : " + expected);
	Console::WriteLine(L"    actual   : " + actual);
	TEST_ASSERT(actual == expected);
}

TEST_CASE(OpMul)
{
	WString expected = L"2, 8, 32, 2, 8, 32, 4.5";
	WString actual = ::vl_workflow_global::OpMul::Instance().main();
	Console::WriteLine(L"    expected : " + expected);
	Console::WriteLine(L"    actual   : " + actual);
	TEST_ASSERT(actual == expected);
}

TEST_CASE(OpDiv)
{
	WString expected = L"0, 0, 0, 0, 0, 0, 0.5";
	WString actual = ::vl_workflow_global::OpDiv::Instance().main();
	Console::WriteLine(L"    expected : " + expected);
	Console::WriteLine(L"    actual   : " + actual);
	TEST_ASSERT(actual == expected);
}

TEST_CASE(OpMod)
{
	WString expected = L"1, 2, 4, 1, 2, 4";
	WString actual = ::vl_workflow_global::OpMod::Instance().main();
	Console::WriteLine(L"    expected : " + expected);
	Console::WriteLine(L"    actual   : " + actual);
	TEST_ASSERT(actual == expected);
}

TEST_CASE(OpShl)
{
	WString expected = L"4, 32, 1024, 4, 32, 1024";
	WString actual = ::vl_workflow_global::OpShl::Instance().main();
	Console::WriteLine(L"    expected : " + expected);
	Console::WriteLine(L"    actual   : " + actual);
	TEST_ASSERT(actual == expected);
}

TEST_CASE(OpShr)
{
	WString expected = L"0, 0, 0, 0, 0, 0";
	WString actual = ::vl_workflow_global::OpShr::Instance().main();
	Console::WriteLine(L"    expected : " + expected);
	Console::WriteLine(L"    actual   : " + actual);
	TEST_ASSERT(actual == expected);
}

TEST_CASE(OpAnd)
{
	WString expected = L"false, false, 0, 0, 0, 0, 0, 0";
	WString actual = ::vl_workflow_global::OpAnd::Instance().main();
	Console::WriteLine(L"    expected : " + expected);
	Console::WriteLine(L"    actual   : " + actual);
	TEST_ASSERT(actual == expected);
}

TEST_CASE(OpOr)
{
	WString expected = L"true, false, 3, 6, 12, 3, 6, 12";
	WString actual = ::vl_workflow_global::OpOr::Instance().main();
	Console::WriteLine(L"    expected : " + expected);
	Console::WriteLine(L"    actual   : " + actual);
	TEST_ASSERT(actual == expected);
}

TEST_CASE(OpXor)
{
	WString expected = L"true, false, 3, 6, 12, 3, 6, 12";
	WString actual = ::vl_workflow_global::OpXor::Instance().main();
	Console::WriteLine(L"    expected : " + expected);
	Console::WriteLine(L"    actual   : " + actual);
	TEST_ASSERT(actual == expected);
}

TEST_CASE(OpCompare)
{
	WString expected = L"true, false, false, true, false, true, true, false, false, false, false, false, true, true, true, true, false, false, false, true, true, false, false, false, true, false, false, true";
	WString actual = ::vl_workflow_global::OpCompare::Instance().main();
	Console::WriteLine(L"    expected : " + expected);
	Console::WriteLine(L"    actual   : " + actual);
	TEST_ASSERT(actual == expected);
}

TEST_CASE(OpCompareReference)
{
	WString expected = L"false, true, false, false, true, true, false, false, true, true, true";
	WString actual = ::vl_workflow_global::OpCompareReference::Instance().main();
	Console::WriteLine(L"    expected : " + expected);
	Console::WriteLine(L"    actual   : " + actual);
	TEST_ASSERT(actual == expected);
}

TEST_CASE(ListProcessing)
{
	WString expected = L"5, 1, 5, 6, 1, 6";
	WString actual = ::vl_workflow_global::ListProcessing::Instance().main();
	Console::WriteLine(L"    expected : " + expected);
	Console::WriteLine(L"    actual   : " + actual);
	TEST_ASSERT(actual == expected);
}

TEST_CASE(MapProcessing)
{
	WString expected = L"5, 1, 5, 6, 1, 6";
	WString actual = ::vl_workflow_global::MapProcessing::Instance().main();
	Console::WriteLine(L"    expected : " + expected);
	Console::WriteLine(L"    actual   : " + actual);
	TEST_ASSERT(actual == expected);
}

TEST_CASE(ObservableList)
{
	WString expected = L"[1:0:1][3:0:1][5:0:1][7:0:1][9:0:1][0:1:0][1:1:0][2:1:0][3:1:0][4:1:0][2][4][6][8][10][0:5:0]";
	WString actual = ::vl_workflow_global::ObservableList::Instance().main();
	Console::WriteLine(L"    expected : " + expected);
	Console::WriteLine(L"    actual   : " + actual);
	TEST_ASSERT(actual == expected);
}

TEST_CASE(ElementInSet)
{
	WString expected = L"true, false, true, false, true, false, false, true, true, false";
	WString actual = ::vl_workflow_global::ElementInSet::Instance().main();
	Console::WriteLine(L"    expected : " + expected);
	Console::WriteLine(L"    actual   : " + actual);
	TEST_ASSERT(actual == expected);
}

TEST_CASE(GlobalVariable)
{
	WString expected = L"30, 10";
	WString actual = ::vl_workflow_global::GlobalVariable::Instance().main();
	Console::WriteLine(L"    expected : " + expected);
	Console::WriteLine(L"    actual   : " + actual);
	TEST_ASSERT(actual == expected);
}

TEST_CASE(RecursiveFunction)
{
	WString expected = L"1, 1, 2, 3, 5";
	WString actual = ::vl_workflow_global::RecursiveFunction::Instance().main();
	Console::WriteLine(L"    expected : " + expected);
	Console::WriteLine(L"    actual   : " + actual);
	TEST_ASSERT(actual == expected);
}

TEST_CASE(LetIn)
{
	WString expected = L"30";
	WString actual = ::vl_workflow_global::LetIn::Instance().main();
	Console::WriteLine(L"    expected : " + expected);
	Console::WriteLine(L"    actual   : " + actual);
	TEST_ASSERT(actual == expected);
}

TEST_CASE(Property)
{
	WString expected = L"10, 20, 10, , This is , 20, Workflow, This is Workflow";
	WString actual = ::vl_workflow_global::Property::Instance().main();
	Console::WriteLine(L"    expected : " + expected);
	Console::WriteLine(L"    actual   : " + actual);
	TEST_ASSERT(actual == expected);
}

TEST_CASE(MethodClosure)
{
	WString expected = L"10, 20";
	WString actual = ::vl_workflow_global::MethodClosure::Instance().main();
	Console::WriteLine(L"    expected : " + expected);
	Console::WriteLine(L"    actual   : " + actual);
	TEST_ASSERT(actual == expected);
}

TEST_CASE(StaticMethod)
{
	WString expected = L"1, This is one, 2, This is two";
	WString actual = ::vl_workflow_global::StaticMethod::Instance().main();
	Console::WriteLine(L"    expected : " + expected);
	Console::WriteLine(L"    actual   : " + actual);
	TEST_ASSERT(actual == expected);
}

TEST_CASE(Event)
{
	WString expected = L"2, 10, 20, 2, 20, 30, true, false";
	WString actual = ::vl_workflow_global::Event::Instance().main();
	Console::WriteLine(L"    expected : " + expected);
	Console::WriteLine(L"    actual   : " + actual);
	TEST_ASSERT(actual == expected);
}

TEST_CASE(ControlFlow)
{
	WString expected = L"55, 55, 55, 55, 55";
	WString actual = ::vl_workflow_global::ControlFlow::Instance().main();
	Console::WriteLine(L"    expected : " + expected);
	Console::WriteLine(L"    actual   : " + actual);
	TEST_ASSERT(actual == expected);
}

TEST_CASE(IfNotNull)
{
	WString expected = L"{1} is a list, {1:2} is not a list";
	WString actual = ::vl_workflow_global::IfNotNull::Instance().main();
	Console::WriteLine(L"    expected : " + expected);
	Console::WriteLine(L"    actual   : " + actual);
	TEST_ASSERT(actual == expected);
}

TEST_CASE(ForEach)
{
	WString expected = L"[1, 2, 3, 4, 5][5, 4, 3, 2, 1]";
	WString actual = ::vl_workflow_global::ForEach::Instance().main();
	Console::WriteLine(L"    expected : " + expected);
	Console::WriteLine(L"    actual   : " + actual);
	TEST_ASSERT(actual == expected);
}

TEST_CASE(AnonymousLambda)
{
	WString expected = L"2, 3, 3, 4";
	WString actual = ::vl_workflow_global::AnonymousLambda::Instance().main();
	Console::WriteLine(L"    expected : " + expected);
	Console::WriteLine(L"    actual   : " + actual);
	TEST_ASSERT(actual == expected);
}

TEST_CASE(NamedLambda)
{
	WString expected = L"1, 1, 2, 3, 5";
	WString actual = ::vl_workflow_global::NamedLambda::Instance().main();
	Console::WriteLine(L"    expected : " + expected);
	Console::WriteLine(L"    actual   : " + actual);
	TEST_ASSERT(actual == expected);
}

TEST_CASE(OrderedLambda)
{
	WString expected = L"2, 3, 3, 4";
	WString actual = ::vl_workflow_global::OrderedLambda::Instance().main();
	Console::WriteLine(L"    expected : " + expected);
	Console::WriteLine(L"    actual   : " + actual);
	TEST_ASSERT(actual == expected);
}

TEST_CASE(TryFinally)
{
	WString expected = L"3115";
	WString actual = ::vl_workflow_global::TryFinally::Instance().main();
	Console::WriteLine(L"    expected : " + expected);
	Console::WriteLine(L"    actual   : " + actual);
	TEST_ASSERT(actual == expected);
}

TEST_CASE(TryFinally2)
{
	WString expected = L"3115";
	WString actual = ::vl_workflow_global::TryFinally2::Instance().main();
	Console::WriteLine(L"    expected : " + expected);
	Console::WriteLine(L"    actual   : " + actual);
	TEST_ASSERT(actual == expected);
}

TEST_CASE(TryCatch)
{
	WString expected = L"[Test1::catch][Test2::catch][Test2::finally][Test3::catch1][Test3::finally1][Test3::catch2][Test3::finally2][Test4::finally1][Test4::finally2]";
	WString actual = ::vl_workflow_global::TryCatch::Instance().main();
	Console::WriteLine(L"    expected : " + expected);
	Console::WriteLine(L"    actual   : " + actual);
	TEST_ASSERT(actual == expected);
}

TEST_CASE(Delete)
{
	WString expected = L"false, false, false, false, true";
	WString actual = ::vl_workflow_global::Delete::Instance().main();
	Console::WriteLine(L"    expected : " + expected);
	Console::WriteLine(L"    actual   : " + actual);
	TEST_ASSERT(actual == expected);
}

TEST_CASE(FailedThen)
{
	WString expected = L"-1, 110";
	WString actual = ::vl_workflow_global::FailedThen::Instance().main();
	Console::WriteLine(L"    expected : " + expected);
	Console::WriteLine(L"    actual   : " + actual);
	TEST_ASSERT(actual == expected);
}

TEST_CASE(NewInterface)
{
	WString expected = L"[1, 2, 3, 4, 5][5, 4, 3, 2, 1]";
	WString actual = ::vl_workflow_global::NewInterface::Instance().main();
	Console::WriteLine(L"    expected : " + expected);
	Console::WriteLine(L"    actual   : " + actual);
	TEST_ASSERT(actual == expected);
}

TEST_CASE(NewCustomInterface)
{
	WString expected = L"[100][15][15]";
	WString actual = ::vl_workflow_global::NewCustomInterface::Instance().main();
	Console::WriteLine(L"    expected : " + expected);
	Console::WriteLine(L"    actual   : " + actual);
	TEST_ASSERT(actual == expected);
}

TEST_CASE(NewCustomInterface2)
{
	WString expected = L"[100][15][15]";
	WString actual = ::vl_workflow_global::NewCustomInterface2::Instance().main();
	Console::WriteLine(L"    expected : " + expected);
	Console::WriteLine(L"    actual   : " + actual);
	TEST_ASSERT(actual == expected);
}

TEST_CASE(NewCustomInterface3)
{
	WString expected = L"[100][15][15]";
	WString actual = ::vl_workflow_global::NewCustomInterface3::Instance().main();
	Console::WriteLine(L"    expected : " + expected);
	Console::WriteLine(L"    actual   : " + actual);
	TEST_ASSERT(actual == expected);
}

TEST_CASE(BindSimple)
{
	WString expected = L"[10][30][60]";
	WString actual = ::vl_workflow_global::BindSimple::Instance().main();
	Console::WriteLine(L"    expected : " + expected);
	Console::WriteLine(L"    actual   : " + actual);
	TEST_ASSERT(actual == expected);
}

TEST_CASE(BindLet)
{
	WString expected = L"[1][2][22][42][42][342][642]";
	WString actual = ::vl_workflow_global::BindLet::Instance().main();
	Console::WriteLine(L"    expected : " + expected);
	Console::WriteLine(L"    actual   : " + actual);
	TEST_ASSERT(actual == expected);
}

TEST_CASE(BindComplex)
{
	WString expected = L"[10][30][60]";
	WString actual = ::vl_workflow_global::BindComplex::Instance().main();
	Console::WriteLine(L"    expected : " + expected);
	Console::WriteLine(L"    actual   : " + actual);
	TEST_ASSERT(actual == expected);
}

TEST_CASE(BindFormat)
{
	WString expected = L"[The value has changed to 10][The value has changed to 20][The value has changed to 30]";
	WString actual = ::vl_workflow_global::BindFormat::Instance().main();
	Console::WriteLine(L"    expected : " + expected);
	Console::WriteLine(L"    actual   : " + actual);
	TEST_ASSERT(actual == expected);
}

TEST_CASE(BindCustomInterface)
{
	WString expected = L"[1][3][6][10][15]";
	WString actual = ::vl_workflow_global::BindCustomInterface::Instance().main();
	Console::WriteLine(L"    expected : " + expected);
	Console::WriteLine(L"    actual   : " + actual);
	TEST_ASSERT(actual == expected);
}

TEST_CASE(BindCustomInterface2)
{
	WString expected = L"[1][3][6][10][15]";
	WString actual = ::vl_workflow_global::BindCustomInterface2::Instance().main();
	Console::WriteLine(L"    expected : " + expected);
	Console::WriteLine(L"    actual   : " + actual);
	TEST_ASSERT(actual == expected);
}

TEST_CASE(ClassField)
{
	WString expected = L"0, 0, 1, 2, true, false";
	WString actual = ::vl_workflow_global::ClassField::Instance().main();
	Console::WriteLine(L"    expected : " + expected);
	Console::WriteLine(L"    actual   : " + actual);
	TEST_ASSERT(actual == expected);
}

TEST_CASE(CallStaticMethod)
{
	WString expected = L"55";
	WString actual = ::vl_workflow_global::CallStaticMethod::Instance().main();
	Console::WriteLine(L"    expected : " + expected);
	Console::WriteLine(L"    actual   : " + actual);
	TEST_ASSERT(actual == expected);
}

TEST_CASE(NestedLambda)
{
	WString expected = L"[1][2][[4][5][[3, 12]]]";
	WString actual = ::vl_workflow_global::NestedLambda::Instance().main();
	Console::WriteLine(L"    expected : " + expected);
	Console::WriteLine(L"    actual   : " + actual);
	TEST_ASSERT(actual == expected);
}

TEST_CASE(ClassMethod)
{
	WString expected = L"[0][100][2][1][100]";
	WString actual = ::vl_workflow_global::ClassMethod::Instance().main();
	Console::WriteLine(L"    expected : " + expected);
	Console::WriteLine(L"    actual   : " + actual);
	TEST_ASSERT(actual == expected);
}

TEST_CASE(ClassCtor)
{
	WString expected = L"[0][0][1][0][0][2][1][2]";
	WString actual = ::vl_workflow_global::ClassCtor::Instance().main();
	Console::WriteLine(L"    expected : " + expected);
	Console::WriteLine(L"    actual   : " + actual);
	TEST_ASSERT(actual == expected);
}

TEST_CASE(ClassDtor)
{
	WString expected = L"[x][y][z]";
	WString actual = ::vl_workflow_global::ClassDtor::Instance().main();
	Console::WriteLine(L"    expected : " + expected);
	Console::WriteLine(L"    actual   : " + actual);
	TEST_ASSERT(actual == expected);
}

TEST_CASE(StructCtor)
{
	WString expected = L"21";
	WString actual = ::vl_workflow_global::StructCtor::Instance().main();
	Console::WriteLine(L"    expected : " + expected);
	Console::WriteLine(L"    actual   : " + actual);
	TEST_ASSERT(actual == expected);
}

TEST_CASE(StructCtor2)
{
	WString expected = L"21";
	WString actual = ::vl_workflow_global::StructCtor2::Instance().main();
	Console::WriteLine(L"    expected : " + expected);
	Console::WriteLine(L"    actual   : " + actual);
	TEST_ASSERT(actual == expected);
}

TEST_CASE(EnumCtor)
{
	WString expected = L"1, 2, 4, 8, 3, 1, 0";
	WString actual = ::vl_workflow_global::EnumCtor::Instance().main();
	Console::WriteLine(L"    expected : " + expected);
	Console::WriteLine(L"    actual   : " + actual);
	TEST_ASSERT(actual == expected);
}

TEST_CASE(EnumCtor2)
{
	WString expected = L"1, 2, 4, 8, 3, 1, 0";
	WString actual = ::vl_workflow_global::EnumCtor2::Instance().main();
	Console::WriteLine(L"    expected : " + expected);
	Console::WriteLine(L"    actual   : " + actual);
	TEST_ASSERT(actual == expected);
}

TEST_CASE(WorkflowHints)
{
	WString expected = L"[1][10][100][1000][2][20][1][100][3][4]";
	WString actual = ::vl_workflow_global::WorkflowHints::Instance().main();
	Console::WriteLine(L"    expected : " + expected);
	Console::WriteLine(L"    actual   : " + actual);
	TEST_ASSERT(actual == expected);
}

TEST_CASE(WorkflowAttributes)
{
	WString expected = L"((1 + 2) * (3 + 4)) = 21";
	WString actual = ::vl_workflow_global::WorkflowAttributes::Instance().main();
	Console::WriteLine(L"    expected : " + expected);
	Console::WriteLine(L"    actual   : " + actual);
	TEST_ASSERT(actual == expected);
}

TEST_CASE(CoRawCoroutine)
{
	WString expected = L"[+0][-0][+1][-1][+2][-2][+3][-3][+4][-4][+Enough!][-5]";
	WString actual = ::vl_workflow_global::CoRawCoroutine::Instance().main();
	Console::WriteLine(L"    expected : " + expected);
	Console::WriteLine(L"    actual   : " + actual);
	TEST_ASSERT(actual == expected);
}

TEST_CASE(CoRawCoroutine2)
{
	WString expected = L"[+0][-0]![*]![+2][-1]![*]![-2]";
	WString actual = ::vl_workflow_global::CoRawCoroutine2::Instance().main();
	Console::WriteLine(L"    expected : " + expected);
	Console::WriteLine(L"    actual   : " + actual);
	TEST_ASSERT(actual == expected);
}

TEST_CASE(CoRawCoroutine3)
{
	WString expected = L"[+0][-0]!![-1][*]";
	WString actual = ::vl_workflow_global::CoRawCoroutine3::Instance().main();
	Console::WriteLine(L"    expected : " + expected);
	Console::WriteLine(L"    actual   : " + actual);
	TEST_ASSERT(actual == expected);
}

TEST_CASE(CoRawCoroutine4)
{
	WString expected = L"[+0][-0][+1][-1][-2][*]";
	WString actual = ::vl_workflow_global::CoRawCoroutine4::Instance().main();
	Console::WriteLine(L"    expected : " + expected);
	Console::WriteLine(L"    actual   : " + actual);
	TEST_ASSERT(actual == expected);
}

TEST_CASE(CoRawCoroutine5)
{
	WString expected = L"[+0][-0][+1][-1][+2][-2][+3][-3][+4][-4][+Enough!][-5]";
	WString actual = ::vl_workflow_global::CoRawCoroutine5::Instance().main();
	Console::WriteLine(L"    expected : " + expected);
	Console::WriteLine(L"    actual   : " + actual);
	TEST_ASSERT(actual == expected);
}

TEST_CASE(CoRawCoroutine6)
{
	WString expected = L"[+0][-0][+1][-1][+2][-2][+3][-3][+4][-4][+Enough!][-5]";
	WString actual = ::vl_workflow_global::CoRawCoroutine6::Instance().main();
	Console::WriteLine(L"    expected : " + expected);
	Console::WriteLine(L"    actual   : " + actual);
	TEST_ASSERT(actual == expected);
}

TEST_CASE(CoRawCoroutine7)
{
	WString expected = L"[+0][-0][+1][-1][+2][-2][+3][-3][+4][-4][+Enough!][-5]";
	WString actual = ::vl_workflow_global::CoRawCoroutine7::Instance().main();
	Console::WriteLine(L"    expected : " + expected);
	Console::WriteLine(L"    actual   : " + actual);
	TEST_ASSERT(actual == expected);
}

TEST_CASE(CoEnum)
{
	WString expected = L"[0][1][2][3][4]";
	WString actual = ::vl_workflow_global::CoEnum::Instance().main();
	Console::WriteLine(L"    expected : " + expected);
	Console::WriteLine(L"    actual   : " + actual);
	TEST_ASSERT(actual == expected);
}

TEST_CASE(CoEnum2)
{
	WString expected = L"[0][1][2][1][2][3][2][3][4]";
	WString actual = ::vl_workflow_global::CoEnum2::Instance().main();
	Console::WriteLine(L"    expected : " + expected);
	Console::WriteLine(L"    actual   : " + actual);
	TEST_ASSERT(actual == expected);
}

TEST_CASE(CoAsync)
{
	WString expected = L"[0][+0][0][-0]![1][+1][1][-1]!";
	WString actual = ::vl_workflow_global::CoAsync::Instance().main();
	Console::WriteLine(L"    expected : " + expected);
	Console::WriteLine(L"    actual   : " + actual);
	TEST_ASSERT(actual == expected);
}

TEST_CASE(CoAsync2)
{
	WString expected = L"[1][2][3][4][5][Cancelled]";
	WString actual = ::vl_workflow_global::CoAsync2::Instance().main();
	Console::WriteLine(L"    expected : " + expected);
	Console::WriteLine(L"    actual   : " + actual);
	TEST_ASSERT(actual == expected);
}

TEST_CASE(Overloading)
{
	WString expected = L"[2][2][2][2][2][2][2][2][2][2]";
	WString actual = ::vl_workflow_global::Overloading::Instance().main();
	Console::WriteLine(L"    expected : " + expected);
	Console::WriteLine(L"    actual   : " + actual);
	TEST_ASSERT(actual == expected);
}

TEST_CASE(CoSmcSwitchD1)
{
	WString expected = L"[A].";
	WString actual = ::vl_workflow_global::CoSmcSwitchD1::Instance().main();
	Console::WriteLine(L"    expected : " + expected);
	Console::WriteLine(L"    actual   : " + actual);
	TEST_ASSERT(actual == expected);
}

TEST_CASE(CoSmcSwitchD2)
{
	WString expected = L"!";
	WString actual = ::vl_workflow_global::CoSmcSwitchD2::Instance().main();
	Console::WriteLine(L"    expected : " + expected);
	Console::WriteLine(L"    actual   : " + actual);
	TEST_ASSERT(actual == expected);
}

TEST_CASE(CoSmcSwitchI1)
{
	WString expected = L"[A].";
	WString actual = ::vl_workflow_global::CoSmcSwitchI1::Instance().main();
	Console::WriteLine(L"    expected : " + expected);
	Console::WriteLine(L"    actual   : " + actual);
	TEST_ASSERT(actual == expected);
}

TEST_CASE(CoSmcSwitchI2)
{
	WString expected = L".";
	WString actual = ::vl_workflow_global::CoSmcSwitchI2::Instance().main();
	Console::WriteLine(L"    expected : " + expected);
	Console::WriteLine(L"    actual   : " + actual);
	TEST_ASSERT(actual == expected);
}

TEST_CASE(CoSmcSwitchIR1)
{
	WString expected = L"[A].";
	WString actual = ::vl_workflow_global::CoSmcSwitchIR1::Instance().main();
	Console::WriteLine(L"    expected : " + expected);
	Console::WriteLine(L"    actual   : " + actual);
	TEST_ASSERT(actual == expected);
}

TEST_CASE(CoSmcSwitchIR2)
{
	WString expected = L"";
	WString actual = ::vl_workflow_global::CoSmcSwitchIR2::Instance().main();
	Console::WriteLine(L"    expected : " + expected);
	Console::WriteLine(L"    actual   : " + actual);
	TEST_ASSERT(actual == expected);
}

TEST_CASE(CoSmcSwitchP1)
{
	WString expected = L"[A][B].";
	WString actual = ::vl_workflow_global::CoSmcSwitchP1::Instance().main();
	Console::WriteLine(L"    expected : " + expected);
	Console::WriteLine(L"    actual   : " + actual);
	TEST_ASSERT(actual == expected);
}

TEST_CASE(CoSmcSwitchP2)
{
	WString expected = L"[B].!";
	WString actual = ::vl_workflow_global::CoSmcSwitchP2::Instance().main();
	Console::WriteLine(L"    expected : " + expected);
	Console::WriteLine(L"    actual   : " + actual);
	TEST_ASSERT(actual == expected);
}

TEST_CASE(CoSmcSwitchPR1)
{
	WString expected = L"[A][B].";
	WString actual = ::vl_workflow_global::CoSmcSwitchPR1::Instance().main();
	Console::WriteLine(L"    expected : " + expected);
	Console::WriteLine(L"    actual   : " + actual);
	TEST_ASSERT(actual == expected);
}

TEST_CASE(CoSmcSwitchPR2)
{
	WString expected = L"!";
	WString actual = ::vl_workflow_global::CoSmcSwitchPR2::Instance().main();
	Console::WriteLine(L"    expected : " + expected);
	Console::WriteLine(L"    actual   : " + actual);
	TEST_ASSERT(actual == expected);
}

TEST_CASE(CoSmcGoto)
{
	WString expected = L"[A].A!";
	WString actual = ::vl_workflow_global::CoSmcGoto::Instance().main();
	Console::WriteLine(L"    expected : " + expected);
	Console::WriteLine(L"    actual   : " + actual);
	TEST_ASSERT(actual == expected);
}

TEST_CASE(CoSmcPush)
{
	WString expected = L"[A].A[B].B.";
	WString actual = ::vl_workflow_global::CoSmcPush::Instance().main();
	Console::WriteLine(L"    expected : " + expected);
	Console::WriteLine(L"    actual   : " + actual);
	TEST_ASSERT(actual == expected);
}

TEST_CASE(CoSmcTryCatch1)
{
	WString expected = L"[A].A[B].B.";
	WString actual = ::vl_workflow_global::CoSmcTryCatch1::Instance().main();
	Console::WriteLine(L"    expected : " + expected);
	Console::WriteLine(L"    actual   : " + actual);
	TEST_ASSERT(actual == expected);
}

TEST_CASE(CoSmcTryCatch2)
{
	WString expected = L"!A!B.";
	WString actual = ::vl_workflow_global::CoSmcTryCatch2::Instance().main();
	Console::WriteLine(L"    expected : " + expected);
	Console::WriteLine(L"    actual   : " + actual);
	TEST_ASSERT(actual == expected);
}

TEST_CASE(CoSmcCalculator)
{
	WString expected = L"[0][0.][0.5][1][1.][1.2][1.25][1.75][3.0625]";
	WString actual = ::vl_workflow_global::CoSmcCalculator::Instance().main();
	Console::WriteLine(L"    expected : " + expected);
	Console::WriteLine(L"    actual   : " + actual);
	TEST_ASSERT(actual == expected);
}
