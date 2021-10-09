/***********************************************************************
Vczh Library++ 3.0
Developer: Zihan Chen(vczh)
Workflow::C++ Code Generator

Interfaces:
**********************************************************************/

#ifndef VCZH_WORKFLOW_CPP_WFCPP
#define VCZH_WORKFLOW_CPP_WFCPP

#include "../Emitter/WfEmitter.h"

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
				__vwsno#_ASSEMBLY_*		: Ordered lambda class name
				__vwsnf#_ASSEMBLY_*		: Function expression class name
				__vwsnc#_ASSEMBLY_*		: Anonymous interface class name
				__vwsnthis_#			: Captured this pointer in fields
				__vwsnctor_*			: Captured symbol in constructor arguments, assigned to "this->*"
				__vwsnctorthis_#		: Captured this pointer in constructor arguments assigned to "this->__vwsnthis_#"?
				__vwsnt_#				: Temporary type
				__vwsne_#				: Temporary variable (not for lambda)
				__vwsnb_#				: Temporary block
				__vwnsl_#_LABEL_NAME	: Goto label
				__vwsn_structs::NAME	: Struct definition
				__vwsn_enums::NAME		: Struct definition
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
					SymbolMap										ctorArgumentSymbols;
					collections::List<ITypeDescriptor*>				thisTypes;				// nearer this pointer first
				};

			public:
				regex::Regex										regexSplitName;
				regex::Regex										regexSpecialName;
				regex::Regex										regexTemplate;

			protected:
				Ptr<ClosureInfo>									CollectClosureInfo(Ptr<WfExpression> closure);
				void												Collect();

				struct GlobalDep
				{
					collections::Dictionary<WString, ITypeDescriptor*>		allTds;				// Type name to type descriptor of class decls. Class index is the position in this map.
					collections::Group<vint, vint>							expandedClassDecls;	// Class index (-1 means top level) to all direct and indirect internal classes
					collections::Group<vint, vint>							dependencies;		// Class dependencies
					collections::SortedList<vint>							topLevelClasses;	// Class index
					collections::Group<vint, vint>							topLevelClassDep;	// Class dependencies for top level classes
				};

				struct ClassLevelDep
				{
					Ptr<WfClassDeclaration>									parentClass;
					vint													parentIndexKey = -1;
					collections::Group<vint, vint>							depGroup;			// A slice of GlobalDep::dependencies
					collections::Dictionary<vint, vint>						subClass;			// Classes to their ancestor, which are direct internal classes of the selected parent class
				};

				void												ExpandClassDeclGroup(Ptr<WfClassDeclaration> parent, GlobalDep& globalDep);
				void												GenerateClassDependencies(GlobalDep& globalDep);
				void												GenerateGlobalDep(GlobalDep& globalDep);

				void												CollectExpandedDepGroup(vint parentIndexKey, GlobalDep& globalDep, ClassLevelDep& classLevelDep);
				void												CollectExpandedSubClass(vint subDeclIndexKey, GlobalDep& globalDep, ClassLevelDep& classLevelDep);
				void												GenerateClassLevelDep(Ptr<WfClassDeclaration> parent, GlobalDep& globalDep, ClassLevelDep& classLevelDep);

				void												SortClassDecls(GlobalDep& globalDep);
				void												GenerateFileClassMaps(GlobalDep& globalDep);
				void												SortFileClassMaps(GlobalDep& globalDep);
				void												AssignClassDeclsToFiles();

				template<typename T>
				void SortDeclsByName(collections::List<Ptr<T>>& decls)
				{
					collections::Sort<Ptr<T>>(&decls[0], decls.Count(), [=](Ptr<T> a, Ptr<T> b)
					{
						auto tdA = manager->declarationTypes[a.Obj()].Obj();
						auto tdB = manager->declarationTypes[b.Obj()].Obj();
						return WString::Compare(tdA->GetTypeName(), tdB->GetTypeName());
					});
				}

			public:
				analyzer::WfLexicalScopeManager*											manager;
				Ptr<emitter::WfAttributeEvaluator>											attributeEvaluator;
				WString																		assemblyNamespace;
				WString																		assemblyName;

				collections::Dictionary<ITypeDescriptor*, Ptr<WfDeclaration>>				tdDecls;			// type descriptor to declaration
				collections::Group<Ptr<WfClassDeclaration>, Ptr<WfEnumDeclaration>>			enumDecls;			// class (nullable) to direct internal enums
				collections::Group<Ptr<WfClassDeclaration>, Ptr<WfStructDeclaration>>		structDecls;		// class (nullable) to direct internal structs
				collections::Group<Ptr<WfClassDeclaration>, Ptr<WfClassDeclaration>>		classDecls;			// class (nullable) to direct internal classes
				collections::List<Ptr<WfVariableDeclaration>>								varDecls;			// global variables
				collections::List<Ptr<WfFunctionDeclaration>>								funcDecls;			// global functions

				collections::Group<WString, Ptr<WfClassDeclaration>>						customFilesClasses;	// @cpp:File to top level classes, empty key means the default cpp
				collections::Group<vint, Ptr<WfClassDeclaration>>							headerFilesClasses;	// non-@cpp:File header file to top level classes, 0 means the default header
				collections::Group<vint, vint>												headerIncludes;		// 0:default header, positive:@cpp:File indexed in customFilesClasses, negative:non-@cpp:File headers

				collections::Dictionary<Ptr<WfExpression>, WString>							lambdaExprs;
				collections::Dictionary<Ptr<WfNewInterfaceExpression>, WString>				classExprs;
				collections::Dictionary<Ptr<WfExpression>, Ptr<ClosureInfo>>				closureInfos;
				collections::Group<Ptr<WfClassDeclaration>, Ptr<WfExpression>>				classClosures;

				WfCppConfig(analyzer::WfLexicalScopeManager* _manager, const WString& _assemblyName, const WString& _assemblyNamespace);
				~WfCppConfig();

				void					WriteFunctionBody(stream::StreamWriter& writer, Ptr<WfExpression> expr, const WString& prefix, ITypeInfo* expectedType);
				void					WriteFunctionBody(stream::StreamWriter& writer, Ptr<WfStatement> stat, const WString& prefix, ITypeInfo* expectedType);

				WString					CppNameToHeaderEnumStructName(const WString& fullName, const WString& type);
				WString					ConvertNameInternal(const WString& name, const WString& specialNameCategory, bool alwaysUseCategory);
				WString					ConvertName(const WString& name);
				WString					ConvertName(const WString& name, const WString& specialNameCategory);
				WString					ConvertFullName(const WString& fullName, WString delimiter = L"::");
				WString					ConvertFunctionType(IMethodInfo* methodInfo, WString name = WString::Empty);
				WString					ConvertFunctionType(ITypeInfo* typeInfo);
				bool					IsSpecialGenericType(ITypeInfo* typeInfo);
				WString					ConvertType(ITypeDescriptor* typeInfo, WString delimiter = L"::");
				WString					ConvertType(ITypeInfo* typeInfo, bool useHeaderEnumStructName = false);
				WString					ConvertArgumentType(ITypeInfo* typeInfo);
				WString					DefaultValue(ITypeInfo* typeInfo);
				bool					IsClassHasUserImplMethods(Ptr<WfClassDeclaration> decl, bool searchInternalClasses);

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

				void					WriteHeader_Enum(stream::StreamWriter& writer, Ptr<WfEnumDeclaration> decl, const WString& name, const WString& prefix, bool mainHeaderDefinition);
				void					WriteHeader_EnumOp(stream::StreamWriter& writer, Ptr<WfEnumDeclaration> decl, const WString& name, const WString& prefix);
				void					WriteHeader_Enum(stream::StreamWriter& writer, Ptr<WfEnumDeclaration> decl, collections::List<WString>& nss, bool mainHeaderDefinition);
				void					WriteHeader_MainHeaderEnums(stream::StreamWriter& writer, collections::List<WString>& nss);

				void					WriteHeader_Struct(stream::StreamWriter& writer, Ptr<WfStructDeclaration> decl, const WString& name, const WString& prefix, bool mainHeaderDefinition);
				void					WriteHeader_StructOp(stream::StreamWriter& writer, Ptr<WfStructDeclaration> decl, const WString& name, const WString& prefix);
				void					WriteHeader_Struct(stream::StreamWriter& writer, Ptr<WfStructDeclaration> decl, collections::List<WString>& nss, bool mainHeaderDefinition);
				void					WriteHeader_MainHeaderStructs(stream::StreamWriter& writer, collections::List<WString>& nss);

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
				bool					WriteCpp_ClassMember(stream::StreamWriter& writer, Ptr<WfClassDeclaration> decl, Ptr<WfDeclaration> memberDecl, collections::List<WString>& nss);
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
				void					WriteNonCustomSubHeader(stream::StreamWriter& writer, vint fileIndex);
				void					WriteCpp(stream::StreamWriter& writer, bool multiFile);
				void					WriteSubHeader(stream::StreamWriter& writer, const WString& fileName);
				void					WriteSubCpp(stream::StreamWriter& writer, const WString& fileName);
				void					WriteReflectionHeader(stream::StreamWriter& writer, bool multiFile);
				void					WriteReflectionCpp(stream::StreamWriter& writer, bool multiFile);
			};

/***********************************************************************
WfCppConfig::Collect
***********************************************************************/

			extern void CollectModule(WfCppConfig* config, Ptr<WfModule> module);

/***********************************************************************
WfCppConfig::Write
***********************************************************************/

			class FunctionRecord : public Object
			{
				using LabelMap = collections::Dictionary<WString, WString>;
			public:
				vint					exprCounter = 0;
				vint					blockCounter = 0;
				vint					labelCounter = 0;
				LabelMap				labelNames;
			};

			extern void					GenerateExpression(WfCppConfig* config, stream::StreamWriter& writer, Ptr<WfExpression> node, reflection::description::ITypeInfo* expectedType, bool useReturnValue = true);
			extern void					GenerateStatement(WfCppConfig* config, Ptr<FunctionRecord> functionRecord, stream::StreamWriter& writer, Ptr<WfStatement> node, const WString& prefix, const WString& prefixDelta, reflection::description::ITypeInfo* returnType);
			extern void					GenerateClassMemberDecl(WfCppConfig* config, stream::StreamWriter& writer, const WString& className, Ptr<WfDeclaration> memberDecl, const WString& prefix, bool forClassExpr);
			extern bool					GenerateClassMemberImpl(WfCppConfig* config, stream::StreamWriter& writer, WfClassDeclaration* classDef, const WString& classBaseName, const WString& className, const WString& classFullName, Ptr<WfDeclaration> memberDecl, const WString& prefix);

			extern void					ConvertType(WfCppConfig* config, stream::StreamWriter& writer, reflection::description::ITypeInfo* fromType, reflection::description::ITypeInfo* toType, const Func<void()>& writeExpression, bool strongCast);

/***********************************************************************
GenerateCppFiles
***********************************************************************/

			enum class WfCppFileSwitch
			{
				Enabled,
				Disabled,
				OnDemand,
			};

			class WfCppInput : public Object
			{
			public:
				WfCppFileSwitch									multiFile = WfCppFileSwitch::Enabled;
				WfCppFileSwitch									reflection = WfCppFileSwitch::Enabled;
				WString											comment;
				WString											headerGuardPrefix;
				WString											assemblyName;
				WString											assemblyNamespace;
				WString											includeFileName;
				WString											reflectionFileName;
				WString											defaultFileName;
				collections::List<WString>						normalIncludes;
				collections::List<WString>						reflectionIncludes;

				WfCppInput(const WString& _assemblyName);
			};

			class WfCppOutput : public Object
			{
			public:
				collections::Dictionary<WString, WString>		cppFiles;
				bool											containsReflectionInfo = false;
				bool											multiFile = false;
				bool											reflection = false;
				WString											entryFileName;
			};

			extern Ptr<WfCppOutput>		GenerateCppFiles(Ptr<WfCppInput> input, analyzer::WfLexicalScopeManager* manager);
		}
	}
}

#endif