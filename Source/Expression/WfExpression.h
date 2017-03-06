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

#include "WfExpression_Ast.h"
#include "WfExpression_Parser.h"
#include "WfExpression_Copy.h"
#include "WfExpression_Traverse.h"
#include "WfExpression_Empty.h"

namespace vl
{
	namespace workflow
	{
		/// <summary>Print a Workflow attribute.</summary>
		/// <param name="node">The Workflow attribute.</param>
		/// <param name="indent">The indentation.</param>
		/// <param name="writer">The target writer.</param>
		extern void			WfPrint(Ptr<WfAttribute> node, const WString& indent, parsing::ParsingWriter& writer);
		/// <summary>Print a Workflow type.</summary>
		/// <param name="node">The Workflow type.</param>
		/// <param name="indent">The indentation.</param>
		/// <param name="writer">The target writer.</param>
		extern void			WfPrint(Ptr<WfType> node, const WString& indent, parsing::ParsingWriter& writer);
		/// <summary>Print a Workflow expression.</summary>
		/// <param name="node">The Workflow expression.</param>
		/// <param name="indent">The indentation.</param>
		/// <param name="writer">The target writer.</param>
		extern void			WfPrint(Ptr<WfExpression> node, const WString& indent, parsing::ParsingWriter& writer);
		/// <summary>Print a Workflow statement.</summary>
		/// <param name="node">The Workflow statement.</param>
		/// <param name="indent">The indentation.</param>
		/// <param name="writer">The target writer.</param>
		extern void			WfPrint(Ptr<WfStatement> node, const WString& indent, parsing::ParsingWriter& writer);
		/// <summary>Print a Workflow declaration.</summary>
		/// <param name="node">The Workflow declaration.</param>
		/// <param name="indent">The indentation.</param>
		/// <param name="writer">The target writer.</param>
		extern void			WfPrint(Ptr<WfDeclaration> node, const WString& indent, parsing::ParsingWriter& writer);
		/// <summary>Print a Workflow module.</summary>
		/// <param name="node">The Workflow module.</param>
		/// <param name="indent">The indentation.</param>
		/// <param name="writer">The target writer.</param>
		extern void			WfPrint(Ptr<WfModule> node, const WString& indent, parsing::ParsingWriter& writer);

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