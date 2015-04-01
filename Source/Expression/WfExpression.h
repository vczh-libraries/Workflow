/***********************************************************************
Vczh Library++ 3.0
Developer: Zihan Chen(vczh)
Parser::WfExpression

Interfaces:
***********************************************************************/

#ifndef VCZH_WORKFLOW_EXPRESSION_WFEXPRESSION
#define VCZH_WORKFLOW_EXPRESSION_WFEXPRESSION

#include "WfExpressionParser.h"

namespace vl
{
	namespace workflow
	{
		extern void			WfPrint(Ptr<WfType> node, const WString& indent, parsing::ParsingWriter& writer);
		extern void			WfPrint(Ptr<WfExpression> node, const WString& indent, parsing::ParsingWriter& writer);
		extern void			WfPrint(Ptr<WfStatement> node, const WString& indent, parsing::ParsingWriter& writer);
		extern void			WfPrint(Ptr<WfDeclaration> node, const WString& indent, parsing::ParsingWriter& writer);
		extern void			WfPrint(Ptr<WfModule> node, const WString& indent, parsing::ParsingWriter& writer);

		extern void			WfPrint(Ptr<WfType> node, const WString& indent, stream::TextWriter& writer);
		extern void			WfPrint(Ptr<WfExpression> node, const WString& indent, stream::TextWriter& writer);
		extern void			WfPrint(Ptr<WfStatement> node, const WString& indent, stream::TextWriter& writer);
		extern void			WfPrint(Ptr<WfDeclaration> node, const WString& indent, stream::TextWriter& writer);
		extern void			WfPrint(Ptr<WfModule> node, const WString& indent, stream::TextWriter& writer);
	}
}
#endif