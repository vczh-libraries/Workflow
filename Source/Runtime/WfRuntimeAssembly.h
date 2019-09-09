/***********************************************************************
Vczh Library++ 3.0
Developer: Zihan Chen(vczh)
Workflow::Runtime

Interfaces:
**********************************************************************/

#ifndef VCZH_WORKFLOW_RUNTIME_WFRUNTIMEASSEMBLY
#define VCZH_WORKFLOW_RUNTIME_WFRUNTIMEASSEMBLY

#include "../../Import/VlppParser.h"
#include "WfRuntimeInstruction.h"
#include "WfRuntimeTypeDescriptor.h"

namespace vl
{
	namespace workflow
	{
		namespace runtime
		{

/***********************************************************************
Assembly
***********************************************************************/

			/// <summary>Representing a compiled function.</summary>
			class WfAssemblyFunction : public Object
			{
			public:
				/// <summary>Name of the function.</summary>
				WString												name;
				/// <summary>Argument names of the function. This index is for accessing [F:vl.workflow.runtime.WfRuntimeVariableContext.variables] in [F:vl.workflow.runtime.WfRuntimeCallStackInfo.arguments] when debugging.</summary>
				collections::List<WString>							argumentNames;
				/// <summary>Captured variable names of the function. This index is for accessing [F:vl.workflow.runtime.WfRuntimeVariableContext.variables] in [F:vl.workflow.runtime.WfRuntimeCallStackInfo.captured] when debugging.</summary>
				collections::List<WString>							capturedVariableNames;
				/// <summary>Local variable names of the function. This index is for accessing [F:vl.workflow.runtime.WfRuntimeVariableContext.variables] in [F:vl.workflow.runtime.WfRuntimeCallStackInfo.localVariables] when debugging.</summary>
				collections::List<WString>							localVariableNames;
				/// <summary>First instruction index of the function. This index is for accessing [F:vl.workflow.runtime.WfAssembly.instructions].</summary>
				vint												firstInstruction = -1;
				/// <summary>Last instruction index of the function. This index is for accessing [F:vl.workflow.runtime.WfAssembly.instructions].</summary>
				vint												lastInstruction = -1;
			};

			/// <summary>Representing debug informations.</summary>
			class WfInstructionDebugInfo : public Object
			{
			public:

				/// <summary>Module codes.</summary>
				collections::List<WString>							moduleCodes;				// codeIndex -> code
				/// <summary>Mapping information from every instruction to code. Use[F:vl.parsing.ParsingTextRange.codeIndex] for accessing <see cref="moduleCodes"/>.</summary>
				collections::List<parsing::ParsingTextRange>		instructionCodeMapping;		// instruction -> range
				/// <summary>Mapping information from every row in codes to instructions. Value of the key is (codeIndex, row).</summary>
				collections::Group<Tuple<vint, vint>, vint>			codeInstructionMapping;		// (codeIndex, row) -> instruction [generated]

				void												Initialize();
			};

			/// <summary>Representing failures during loading an assembly</summary>
			class WfAssemblyLoadErrors
			{
			public:
				collections::List<WString>							unresolvedTypes;
				collections::List<WString>							duplicatedTypes;
				collections::List<WString>							unresolvedMembers;
			};

			/// <summary>Representing a Workflow assembly.</summary>
			class WfAssembly : public Object, public reflection::Description<WfAssembly>
			{
			public:
				/// <summary>Debug informations using the module code.</summary>
				Ptr<WfInstructionDebugInfo>							insBeforeCodegen;
				/// <summary>Debug informations using the module code from generated syntax trees from the final compiling pass.</summary>
				Ptr<WfInstructionDebugInfo>							insAfterCodegen;
				/// <summary>Global variable names. This index is for accessing [F:vl.workflow.runtime.WfRuntimeVariableContext.variables] in [F:vl.workflow.runtime.WfRuntimeCallStackInfo.global] when debugging.</summary>
				collections::List<WString>							variableNames;
				/// <summary>Mapping from function names to function indexes for accessing <see cref="functions"/>.</summary>
				collections::Group<WString, vint>					functionByName;
				/// <summary>Functions.</summary>
				collections::List<Ptr<WfAssemblyFunction>>			functions;
				/// <summary>Instructions.</summary>
				collections::List<WfInstruction>					instructions;
				/// <summary>Custom types.</summary>
				Ptr<typeimpl::WfTypeImpl>							typeImpl;

				/// <summary>Create an empty assembly.</summary>
				WfAssembly();
				
				void												Initialize();

				/// <summary>Deserialize an assembly.</summary>
				/// <returns>The deserialized assembly. Returns null if there are errors.</returns>
				/// <param name="input">Serialized binary data.</param>
				/// <param name="errors">Errors during loading an assembly.</param>
				static Ptr<WfAssembly>								Deserialize(stream::IStream& input, WfAssemblyLoadErrors& errors);

				/// <summary>Serialize an assembly.</summary>
				/// <param name="output">Serialized binary data.</param>
				void												Serialize(stream::IStream& output);
			};
		}
	}
}

#endif
