/***********************************************************************
Vczh Library++ 3.0
Developer: Zihan Chen(vczh)
Workflow::C++ Code Generator

Interfaces:
**********************************************************************/

#ifndef VCZH_WORKFLOW_CPP_WFCPP
#define VCZH_WORKFLOW_CPP_WFCPP

#include "../Analyzer/WfAnalyzer.h"

namespace vl
{
	namespace workflow
	{
		namespace cppcodegen
		{
			/*
			Vczh Workflow Special Name:
				__vwsn_not_exists__		: Represents all not supported code
				__vwsn_temp_(#|_)		: Temporary variable
				__vwsn_CATEGORY			: <CATEGORY>
				__vwsn_CATEGORY_NAME	: <CATEGORY>NAME
				__vwsno#_ASSEMBLY_*		: ordered lambda class name
				__vwsnf#_ASSEMBLY_*		: function expression class name
				__vwsnc#_ASSEMBLY_*		: anonymous interface class name
				__vwsnthis_#			: captured this pointer in fields
				__vwsnctor_*			: captured symbol in constructor arguments, assigned to "this->*"
				__vwsnctorthis_#		: captured this pointer in constructor arguments assigned to "this->__vwsnthis_#"?
				__vwsnt_#				: Temporary type
				__vwsne_#				: Temporary variable (not for lambda)
				__vwsnb_#				: Temporary block
			*/

			class WfCppConfig : public Object
			{
				using ITypeInfo = reflection::description::ITypeInfo;
				using IMethodInfo = reflection::description::IMethodInfo;
				using ITypeDescriptor = reflection::description::ITypeDescriptor;

			public:
				class ClosureInfo : public Object
				{
					using SymbolMap = collections::Dictionary<WString, Ptr<analyzer::WfLexicalSymbol>>;
				public:
					WString											lambdaClassName;
					SymbolMap										symbols;
					collections::List<ITypeDescriptor*>				thisTypes;				// nearer this pointer first
				};

			public:
				regex::Regex										regexSplitName;
				regex::Regex										regexSpecialName;
				regex::Regex										regexTemplate;

			protected:
				Ptr<ClosureInfo>									CollectClosureInfo(Ptr<WfExpression> closure);
				void												Collect();
				void												Sort(collections::List<Ptr<WfStructDeclaration>>& structDecls);

			public:
				analyzer::WfLexicalScopeManager*											manager;
				WString																		assemblyNamespace;
				WString																		assemblyName;

				collections::Group<Ptr<WfClassDeclaration>, Ptr<WfEnumDeclaration>>			enumDecls;
				collections::Group<Ptr<WfClassDeclaration>, Ptr<WfStructDeclaration>>		structDecls;
				collections::Group<Ptr<WfClassDeclaration>, Ptr<WfClassDeclaration>>		classDecls;
				collections::List<Ptr<WfVariableDeclaration>>								varDecls;
				collections::List<Ptr<WfFunctionDeclaration>>								funcDecls;

				collections::Group<WString, Ptr<WfClassDeclaration>>						topLevelClassDeclsForFiles;
				collections::Dictionary<Ptr<WfDeclaration>, WString>						declFiles;
				collections::Group<Ptr<WfDeclaration>, Ptr<WfDeclaration>>					declDependencies;

				collections::Dictionary<Ptr<WfExpression>, WString>							lambdaExprs;
				collections::Dictionary<Ptr<WfNewInterfaceExpression>, WString>				classExprs;
				collections::Dictionary<Ptr<WfExpression>, Ptr<ClosureInfo>>				closureInfos;
				collections::Group<Ptr<WfClassDeclaration>, Ptr<WfExpression>>				classClosures;

				WfCppConfig(analyzer::WfLexicalScopeManager* _manager, const WString& _assemblyName, const WString& _assemblyNamespace);
				~WfCppConfig();

				void					WriteFunctionBody(stream::StreamWriter& writer, Ptr<WfExpression> expr, const WString& prefix, ITypeInfo* expectedType);
				void					WriteFunctionBody(stream::StreamWriter& writer, Ptr<WfStatement> stat, const WString& prefix, ITypeInfo* expectedType);

				WString					ConvertName(const WString& name);
				WString					ConvertFullName(const WString& fullName, WString delimiter = L"::");
				WString					ConvertFunctionType(IMethodInfo* methodInfo, WString name = WString::Empty);
				WString					ConvertFunctionType(ITypeInfo* typeInfo);
				bool					IsSpecialGenericType(ITypeInfo* typeInfo);
				WString					ConvertType(ITypeDescriptor* typeInfo, WString delimiter = L"::");
				WString					ConvertType(ITypeInfo* typeInfo);
				WString					ConvertArgumentType(ITypeInfo* typeInfo);
				WString					DefaultValue(ITypeInfo* typeInfo);

				vint					CountClassNamespace(Ptr<WfClassDeclaration> decl);
				void					GetClassNamespace(Ptr<WfClassDeclaration> decl, collections::List<WString>& nss);
				WString					GetClassBaseName(Ptr<WfClassDeclaration> decl);
				WString					WriteNamespace(stream::StreamWriter& writer, collections::List<WString>& nss, collections::List<WString>& nss2);
				WString					WriteNamespace(stream::StreamWriter& writer, const WString& fullName, collections::List<WString>& nss, WString& name);
				void					WriteNamespaceEnd(stream::StreamWriter& writer, collections::List<WString>& nss);

				ITypeInfo*				WriteFunctionHeader(stream::StreamWriter& writer, ITypeInfo* typeInfo, collections::List<WString>& arguments, const WString& name, bool writeReturnType);
				ITypeInfo*				WriteFunctionHeader(stream::StreamWriter& writer, Ptr<WfOrderedLambdaExpression> ordered, const WString& name, bool writeReturnType);
				ITypeInfo*				WriteFunctionHeader(stream::StreamWriter& writer, Ptr<WfFunctionExpression> funcExpr, const WString& name, bool writeReturnType);
				ITypeInfo*				WriteFunctionHeader(stream::StreamWriter& writer, Ptr<WfFunctionDeclaration> decl, const WString& name, bool writeReturnType);
				ITypeInfo*				WriteFunctionHeader(stream::StreamWriter& writer, IMethodInfo* methodInfo, collections::List<WString>& arguments, const WString& name, bool writeReturnType);
				ITypeInfo*				WriteFunctionHeader(stream::StreamWriter& writer, IMethodInfo* methodInfo, const WString& name, bool writeReturnType);

				void					WriteHeader_Enum(stream::StreamWriter& writer, Ptr<WfEnumDeclaration> decl, const WString& name, const WString& prefix);
				void					WriteHeader_EnumOp(stream::StreamWriter& writer, Ptr<WfEnumDeclaration> decl, const WString& name, const WString& prefix);
				void					WriteHeader_Enum(stream::StreamWriter& writer, Ptr<WfEnumDeclaration> decl, collections::List<WString>& nss);

				void					WriteHeader_Struct(stream::StreamWriter& writer, Ptr<WfStructDeclaration> decl, const WString& name, const WString& prefix);
				void					WriteHeader_StructOp(stream::StreamWriter& writer, Ptr<WfStructDeclaration> decl, const WString& name, const WString& prefix);
				void					WriteHeader_Struct(stream::StreamWriter& writer, Ptr<WfStructDeclaration> decl, collections::List<WString>& nss);

				void					WriteHeader_ClosurePreDecl(stream::StreamWriter& writer, Ptr<WfExpression> closure);
				void					WriteHeader_LambdaExprDecl(stream::StreamWriter& writer, Ptr<WfExpression> lambda);
				void					WriteHeader_ClassExprDecl(stream::StreamWriter& writer, Ptr<WfNewInterfaceExpression> lambda);
				bool					WriteCpp_ClosureMembers(stream::StreamWriter& writer, Ptr<WfExpression> closure);
				void					WriteCpp_ClosureCtor(stream::StreamWriter& writer, Ptr<WfExpression> closure, const WString& name);
				void					WriteCpp_ClosureCtorInitList(stream::StreamWriter& writer, Ptr<WfExpression> closure);
				void					WriteCpp_LambdaExprImpl(stream::StreamWriter& writer, Ptr<WfExpression> lambda);
				void					WriteCpp_ClassExprImpl(stream::StreamWriter& writer, Ptr<WfNewInterfaceExpression> lambda);

				void					WriteHeader_ClassPreDecl(stream::StreamWriter& writer, Ptr<WfClassDeclaration> decl, const WString& name, const WString& prefix);
				void					WriteHeader_ClassPreDecl(stream::StreamWriter& writer, Ptr<WfClassDeclaration> decl, collections::List<WString>& nss);
				void					WriteHeader_Class(stream::StreamWriter& writer, Ptr<WfClassDeclaration> decl, const WString& name, const WString& prefix);
				WString					WriteHeader_Class(stream::StreamWriter& writer, Ptr<WfClassDeclaration> decl, collections::List<WString>& nss);
				void					WriteHeader_TopLevelClass(stream::StreamWriter& writer, Ptr<WfClassDeclaration> decl, collections::List<WString>& nss);
				bool					WriteCpp_ClassMember(stream::StreamWriter& writer, Ptr<WfClassDeclaration> decl, Ptr<WfClassMember> member, collections::List<WString>& nss);
				void					WriteCpp_Class(stream::StreamWriter& writer, Ptr<WfClassDeclaration> decl, collections::List<WString>& nss);

				void					SortClosure(collections::Dictionary<WString, Ptr<WfExpression>>& reversedLambdaExprs, collections::Dictionary<WString, Ptr<WfNewInterfaceExpression>>& reversedClassExprs);
				void					WriteHeader_Global(stream::StreamWriter& writer);
				void					WriteCpp_Global(stream::StreamWriter& writer);

				void					WriteHeader_Reflection(stream::StreamWriter& writer);
				void					WriteCpp_Reflection(stream::StreamWriter& writer);

				void					WritePushCompileOptions(stream::StreamWriter& writer);
				void					WritePopCompileOptions(stream::StreamWriter& writer);
				void					WriteCpp_PushMacros(stream::StreamWriter& writer);
				void					WriteCpp_PopMacros(stream::StreamWriter& writer);

				void					WriteHeader(stream::StreamWriter& writer, bool multiFile);
				void					WriteCpp(stream::StreamWriter& writer, bool multiFile);
				void					WriteSubHeader(stream::StreamWriter& writer, const WString& fileName);
				void					WriteSubCpp(stream::StreamWriter& writer, const WString& fileName);
			};

/***********************************************************************
WfCppConfig::Collect
***********************************************************************/

			extern void CollectExpression(WfCppConfig* config, Ptr<WfExpression> node, Ptr<WfClassDeclaration> memberOfClass);
			extern void CollectStatement(WfCppConfig* config, Ptr<WfStatement> node, Ptr<WfClassDeclaration> memberOfClass);
			extern void CollectClassMember(WfCppConfig* config, Ptr<WfClassMember> node, Ptr<WfClassDeclaration> classDecl, Ptr<WfClassDeclaration> memberOfClass);
			extern void CollectDeclaration(WfCppConfig* config, Ptr<WfDeclaration> node, Ptr<WfClassDeclaration> classDecl, Ptr<WfClassDeclaration> memberOfClass);

/***********************************************************************
WfCppConfig::Write
***********************************************************************/

			class FunctionRecord : public Object
			{
			public:
				vint					typeCounter = 0;
				vint					exprCounter = 0;
				vint					blockCounter = 0;
			};

			extern void					GenerateExpression(WfCppConfig* config, stream::StreamWriter& writer, Ptr<WfExpression> node, reflection::description::ITypeInfo* expectedType, bool useReturnValue = true);
			extern void					GenerateStatement(WfCppConfig* config, Ptr<FunctionRecord> functionRecord, stream::StreamWriter& writer, Ptr<WfStatement> node, const WString& prefix, const WString& prefixDelta, reflection::description::ITypeInfo* returnType);
			extern void					GenerateClassMemberDecl(WfCppConfig* config, stream::StreamWriter& writer, const WString& className, Ptr<WfDeclaration> decl, const WString& prefix, bool forClassExpr);
			extern bool					GenerateClassMemberImpl(WfCppConfig* config, stream::StreamWriter& writer, const WString& classBaseName, const WString& className, const WString& classFullName, Ptr<WfDeclaration> decl, const WString& prefix);

			extern void					ConvertType(WfCppConfig* config, stream::StreamWriter& writer, reflection::description::ITypeInfo* fromType, reflection::description::ITypeInfo* toType, const Func<void()>& writeExpression, bool strongCast);

/***********************************************************************
GenerateCppFiles
***********************************************************************/

			enum class WfCppMultiFile
			{
				Enabled,
				Disabled,
				OnDemand,
			};

			class WfCppInput : public Object
			{
			public:
				WfCppMultiFile									multiFile = WfCppMultiFile::Enabled;
				WString											comment;
				WString											headerGuardPrefix;
				WString											assemblyName;
				WString											assemblyNamespace;
				WString											includeFileName;
				WString											defaultFileName;
				collections::List<WString>						normalIncludes;
				collections::List<WString>						reflectionIncludes;

				WfCppInput(const WString& _assemblyName);
			};

			class WfCppOutput : public Object
			{
			public:
				collections::Dictionary<WString, WString>		cppFiles;
			};

			extern Ptr<WfCppOutput>		GenerateCppFiles(Ptr<WfCppInput> input, analyzer::WfLexicalScopeManager* manager);
			extern WString				MergeCppFileContent(const WString& dst, const WString& src);
		}
	}
}

#endif