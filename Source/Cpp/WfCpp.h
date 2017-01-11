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
				__vwsn_CATEGORY			: <CATEGORY>
				__vwsn_CATEGORY_NAME	: <CATEGORY>NAME
				__vwsno#_ASSEMBLY_*		: ordered lambda class name
				__vwsnf#_ASSEMBLY_*		: function expression class name
				__vwsnc#_ASSEMBLY_*		: anonymous interface class name
				__vwsnthis_#			: captured this pointer in fields
				__vwsnctor_*			: captured symbol in constructor arguments, assigned to "this->*"
				__vwsnctorthis_#		: captured this pointer in constructor arguments assigned to "this->__vwsnthis_#'
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
				collections::Dictionary<Ptr<WfExpression>, WString>							lambdaExprs;
				collections::Dictionary<Ptr<WfNewInterfaceExpression>, WString>				classExprs;
				collections::Dictionary<Ptr<WfExpression>, Ptr<ClosureInfo>>				closureInfos;

				WfCppConfig(analyzer::WfLexicalScopeManager* _manager, const WString& _assemblyName);
				~WfCppConfig();

				void					WriteFunctionBody(stream::StreamWriter& writer, Ptr<WfExpression> stat, const WString& prefix);
				void					WriteFunctionBody(stream::StreamWriter& writer, Ptr<WfStatement> stat, const WString& prefix);

				WString					ConvertName(const WString& name);
				WString					ConvertFullName(const WString& fullName, WString delimiter = L"::");
				WString					ConvertFunctionType(IMethodInfo* methodInfo);
				WString					ConvertFunctionType(ITypeInfo* typeInfo);
				WString					ConvertType(ITypeInfo* typeInfo);
				WString					ConvertArgumentType(ITypeInfo* typeInfo);
				WString					DefaultValue(ITypeInfo* typeInfo);

				vint					CountClassNamespace(Ptr<WfClassDeclaration> decl);
				void					GetClassNamespace(Ptr<WfClassDeclaration> decl, collections::List<WString>& nss);
				WString					GetClassBaseName(Ptr<WfClassDeclaration> decl);
				WString					WriteNamespace(stream::StreamWriter& writer, collections::List<WString>& nss, collections::List<WString>& nss2);
				WString					WriteNamespace(stream::StreamWriter& writer, const WString& fullName, collections::List<WString>& nss, WString& name);
				void					WriteNamespaceEnd(stream::StreamWriter& writer, collections::List<WString>& nss);

				void					WriteFunctionHeader(stream::StreamWriter& writer, collections::List<WString>& arguments, ITypeInfo* typeInfo, const WString& name, bool writeReturnType);
				void					WriteFunctionHeader(stream::StreamWriter& writer, Ptr<WfOrderedLambdaExpression> ordered, const WString& name, bool writeReturnType);
				void					WriteFunctionHeader(stream::StreamWriter& writer, Ptr<WfFunctionExpression> funcExpr, const WString& name, bool writeReturnType);
				void					WriteFunctionHeader(stream::StreamWriter& writer, Ptr<WfFunctionDeclaration> decl, const WString& name, bool writeReturnType);
				void					WriteFunctionHeader(stream::StreamWriter& writer, IMethodInfo* methodInfo, const WString& name, bool writeReturnType);

				void					WriteHeader_Enum(stream::StreamWriter& writer, Ptr<WfEnumDeclaration> decl, const WString& name, const WString& prefix);
				void					WriteHeader_Enum(stream::StreamWriter& writer, Ptr<WfEnumDeclaration> decl, collections::List<WString>& nss);

				void					WriteHeader_Struct(stream::StreamWriter& writer, Ptr<WfStructDeclaration> decl, const WString& name, const WString& prefix);
				void					WriteHeader_Struct(stream::StreamWriter& writer, Ptr<WfStructDeclaration> decl, collections::List<WString>& nss);

				void					WriteCpp_ClosureMembers(stream::StreamWriter& writer, Ptr<WfExpression> closure);
				void					WriteCpp_ClosureCtor(stream::StreamWriter& writer, Ptr<WfExpression> closure, const WString& name);
				void					WriteCpp_ClosureCtorInitList(stream::StreamWriter& writer, Ptr<WfExpression> closure);
				void					WriteCpp_LambdaExprDecl(stream::StreamWriter& writer, Ptr<WfExpression> lambda);
				void					WriteCpp_LambdaExprImpl(stream::StreamWriter& writer, Ptr<WfExpression> lambda);
				void					WriteCpp_ClassExprDecl(stream::StreamWriter& writer, Ptr<WfNewInterfaceExpression> lambda);
				void					WriteCpp_ClassExprImpl(stream::StreamWriter& writer, Ptr<WfNewInterfaceExpression> lambda);

				void					WriteHeader_ClassPreDecl(stream::StreamWriter& writer, Ptr<WfClassDeclaration> decl, const WString& name, const WString& prefix);
				void					WriteHeader_ClassPreDecl(stream::StreamWriter& writer, Ptr<WfClassDeclaration> decl, collections::List<WString>& nss);
				void					WriteHeader_Class(stream::StreamWriter& writer, Ptr<WfClassDeclaration> decl, const WString& name, const WString& prefix);
				void					WriteHeader_Class(stream::StreamWriter& writer, Ptr<WfClassDeclaration> decl, collections::List<WString>& nss);
				bool					WriteCpp_ClassMember(stream::StreamWriter& writer, Ptr<WfClassDeclaration> decl, Ptr<WfClassMember> member, collections::List<WString>& nss);

				void					WriteHeader_Global(stream::StreamWriter& writer);
				void					WriteCpp_Global(stream::StreamWriter& writer);
			};

/***********************************************************************
WfCppConfig::Collect
***********************************************************************/

			extern void CollectExpression(WfCppConfig* config, Ptr<WfExpression> node);
			extern void CollectStatement(WfCppConfig* config, Ptr<WfStatement> node);
			extern void CollectClassMember(WfCppConfig* config, Ptr<WfClassMember> node, Ptr<WfClassDeclaration> classDecl);
			extern void CollectDeclaration(WfCppConfig* config, Ptr<WfDeclaration> node, Ptr<WfClassDeclaration> classDecl);

/***********************************************************************
WfCppConfig::Write
***********************************************************************/

			extern void GenerateExpression(WfCppConfig* config, stream::StreamWriter& writer, Ptr<WfExpression> node, const WString& prefix);
			extern void GenerateStatement(WfCppConfig* config, stream::StreamWriter& writer, Ptr<WfStatement> node, const WString& prefix);
			extern void GenerateClassMemberDecl(WfCppConfig* config, stream::StreamWriter& writer, const WString& className, Ptr<WfClassMember> member, const WString& prefix);
			extern bool GenerateClassMemberImpl(WfCppConfig* config, stream::StreamWriter& writer, const WString& classBaseName, const WString& className, Ptr<WfClassMember> member, const WString& prefix);
		}
	}
}

#endif