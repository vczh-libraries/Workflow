/***********************************************************************
Vczh Library++ 3.0
Developer: Zihan Chen(vczh)
Parser::WfExpression

Interfaces:
***********************************************************************/

#ifdef VCZH_DEBUG_NO_REFLECTION
static_assert(false, "Don't use VlppWorkflow.(h|cpp) or VlppWorkflowCompiler.(h|cpp) if VCZH_DEBUG_NO_REFLECTION is defined.");
#endif

#ifndef VCZH_WORKFLOW_EXPRESSION_WFEXPRESSION
#define VCZH_WORKFLOW_EXPRESSION_WFEXPRESSION

#include "Generated/WorkflowAst.h"
#include "Generated/WorkflowAst_Copy.h"
#include "Generated/WorkflowAst_Traverse.h"
#include "Generated/WorkflowAst_Empty.h"
#include "Generated/WorkflowParser.h"

namespace vl
{
	namespace workflow
	{
		
/***********************************************************************
Parsing
***********************************************************************/

		extern Ptr<WfType>				ParseType(const WString& input, const Parser& parser, vint codeIndex = -1);
		extern Ptr<WfExpression>		ParseExpression(const WString& input, const Parser& parser, vint codeIndex = -1);
		extern Ptr<WfStatement>			ParseStatement(const WString& input, const Parser& parser, vint codeIndex = -1);
		extern Ptr<WfDeclaration>		ParserDeclaration(const WString& input, const Parser& parser, vint codeIndex = -1);
		extern Ptr<WfModule>			ParseModule(const WString& input, const Parser& parser, vint codeIndex = -1);
		
/***********************************************************************
Print to ParsingWriter
***********************************************************************/

		/// <summary>Print a Workflow attribute.</summary>
		/// <param name="node">The Workflow attribute.</param>
		/// <param name="indent">The indentation.</param>
		/// <param name="writer">The target writer.</param>
		extern void			WfPrint(Ptr<WfAttribute> node, const WString& indent, glr::ParsingWriter& writer);
		/// <summary>Print a Workflow type.</summary>
		/// <param name="node">The Workflow type.</param>
		/// <param name="indent">The indentation.</param>
		/// <param name="writer">The target writer.</param>
		extern void			WfPrint(Ptr<WfType> node, const WString& indent, glr::ParsingWriter& writer);
		/// <summary>Print a Workflow expression.</summary>
		/// <param name="node">The Workflow expression.</param>
		/// <param name="indent">The indentation.</param>
		/// <param name="writer">The target writer.</param>
		extern void			WfPrint(Ptr<WfExpression> node, const WString& indent, glr::ParsingWriter& writer);
		/// <summary>Print a Workflow statement.</summary>
		/// <param name="node">The Workflow statement.</param>
		/// <param name="indent">The indentation.</param>
		/// <param name="writer">The target writer.</param>
		extern void			WfPrint(Ptr<WfStatement> node, const WString& indent, glr::ParsingWriter& writer);
		/// <summary>Print a Workflow declaration.</summary>
		/// <param name="node">The Workflow declaration.</param>
		/// <param name="indent">The indentation.</param>
		/// <param name="writer">The target writer.</param>
		extern void			WfPrint(Ptr<WfDeclaration> node, const WString& indent, glr::ParsingWriter& writer);
		/// <summary>Print a Workflow module.</summary>
		/// <param name="node">The Workflow module.</param>
		/// <param name="indent">The indentation.</param>
		/// <param name="writer">The target writer.</param>
		extern void			WfPrint(Ptr<WfModule> node, const WString& indent, glr::ParsingWriter& writer);
		
/***********************************************************************
Print to TextWriter
***********************************************************************/

		/// <summary>Print a Workflow attribute.</summary>
		/// <param name="node">The Workflow attribute.</param>
		/// <param name="indent">The indentation.</param>
		/// <param name="writer">The target writer.</param>
		extern void			WfPrint(Ptr<WfAttribute> node, const WString& indent, stream::TextWriter& writer);
		/// <summary>Print a Workflow type.</summary>
		/// <param name="node">The Workflow type.</param>
		/// <param name="indent">The indentation.</param>
		/// <param name="writer">The target writer.</param>
		extern void			WfPrint(Ptr<WfType> node, const WString& indent, stream::TextWriter& writer);
		/// <summary>Print a Workflow expression.</summary>
		/// <param name="node">The Workflow expression.</param>
		/// <param name="indent">The indentation.</param>
		/// <param name="writer">The target writer.</param>
		extern void			WfPrint(Ptr<WfExpression> node, const WString& indent, stream::TextWriter& writer);
		/// <summary>Print a Workflow statement.</summary>
		/// <param name="node">The Workflow statement.</param>
		/// <param name="indent">The indentation.</param>
		/// <param name="writer">The target writer.</param>
		extern void			WfPrint(Ptr<WfStatement> node, const WString& indent, stream::TextWriter& writer);
		/// <summary>Print a Workflow declaration.</summary>
		/// <param name="node">The Workflow declaration.</param>
		/// <param name="indent">The indentation.</param>
		/// <param name="writer">The target writer.</param>
		extern void			WfPrint(Ptr<WfDeclaration> node, const WString& indent, stream::TextWriter& writer);
		/// <summary>Print a Workflow module.</summary>
		/// <param name="node">The Workflow module.</param>
		/// <param name="indent">The indentation.</param>
		/// <param name="writer">The target writer.</param>
		extern void			WfPrint(Ptr<WfModule> node, const WString& indent, stream::TextWriter& writer);
	}
}
#endif