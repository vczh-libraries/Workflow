#include "WfExpression.h"

namespace vl
{
	namespace workflow
	{
		using namespace stream;
		using namespace collections;
		using namespace glr;

/***********************************************************************
Unescaping Functions
***********************************************************************/

		void UnescapeStringInternal(glr::ParsingToken& value, bool formatString)
		{
			value.value = GenerateToStream([&](StreamWriter& writer)
			{
				WString input = formatString ? value.value.Sub(2, value.value.Length() - 3) : value.value.Sub(1, value.value.Length() - 2);
				const wchar_t* reading = input.Buffer();

				while (wchar_t c = *reading++)
				{
					if (c == L'\\')
					{
						switch (wchar_t e = *reading++)
						{
						case L'r':
							writer.WriteChar('\r');
							break;
						case L'n':
							writer.WriteChar('\n');
							break;
						case L't':
							writer.WriteChar('\t');
							break;
						default:
							writer.WriteChar(e);
						}
					}
					else
					{
						writer.WriteChar(c);
					}
				}
			});
		}

		void UnescapeFormatString(glr::ParsingToken& value, const vl::collections::List<vl::regex::RegexToken>& tokens)
		{
			UnescapeStringInternal(value, true);
		}

		void UnescapeString(glr::ParsingToken& value, const vl::collections::List<vl::regex::RegexToken>& tokens)
		{
			UnescapeStringInternal(value, false);
		}

		void EscapeString(const WString& text, ParsingWriter& writer)
		{
			writer.WriteChar(L'\"');
			const wchar_t* reading = text.Buffer();
			while (wchar_t c = *reading++)
			{
				switch (c)
				{
				case '\r':
					writer.WriteString(L"\\r");
					break;
				case '\n':
					writer.WriteString(L"\\n");
					break;
				case '\t':
					writer.WriteString(L"\\t");
					break;
				case '\"':
					writer.WriteString(L"\\\"");
					break;
				case '\\':
					writer.WriteString(L"\\\\");
					break;
				default:
					writer.WriteChar(c);
				}
			}
			writer.WriteChar(L'\"');
		}

/***********************************************************************
WorkflowUnescapeVisitor
***********************************************************************/

		class WorkflowUnescapeVisitor : public traverse_visitor::AstVisitor
		{
		protected:
			List<regex::RegexToken>&			tokens;

			void Traverse(WfStringExpression* node) override
			{
				UnescapeString(node->value, tokens);
			}

			void Traverse(WfFormatExpression* node) override
			{
				UnescapeFormatString(node->value, tokens);
			}
		public:
			WorkflowUnescapeVisitor(List<regex::RegexToken>& _tokens)
				:tokens(_tokens)
			{
			}
		};

/***********************************************************************
Parsing
***********************************************************************/

		Ptr<WfType> ParseType(const WString& input, const Parser& parser, vint codeIndex)
		{
			List<regex::RegexToken> tokens;
			parser.Tokenize(input, tokens, codeIndex);
			auto ast = parser.Parse_Type(tokens, codeIndex);
			WorkflowUnescapeVisitor(tokens).InspectInto(ast.Obj());
			return ast;
		}

		Ptr<WfExpression> ParseExpression(const WString& input, const Parser& parser, vint codeIndex)
		{
			List<regex::RegexToken> tokens;
			parser.Tokenize(input, tokens, codeIndex);
			auto ast = parser.Parse_Expression(tokens, codeIndex);
			WorkflowUnescapeVisitor(tokens).InspectInto(ast.Obj());
			return ast;
		}

		Ptr<WfStatement> ParseStatement(const WString& input, const Parser& parser, vint codeIndex)
		{
			List<regex::RegexToken> tokens;
			parser.Tokenize(input, tokens, codeIndex);
			auto ast = parser.Parse_Statement(tokens, codeIndex);
			WorkflowUnescapeVisitor(tokens).InspectInto(ast.Obj());
			return ast;
		}

		Ptr<WfCoProviderStatement> ParseCoProviderStatement(const WString& input, const Parser& parser, vint codeIndex)
		{
			List<regex::RegexToken> tokens;
			parser.Tokenize(input, tokens, codeIndex);
			auto ast = parser.Parse_CoProvider(tokens, codeIndex);
			WorkflowUnescapeVisitor(tokens).InspectInto(ast.Obj());
			return ast;
		}

		Ptr<WfDeclaration> ParseDeclaration(const WString& input, const Parser& parser, vint codeIndex)
		{
			List<regex::RegexToken> tokens;
			parser.Tokenize(input, tokens, codeIndex);
			auto ast = parser.Parse_Declaration(tokens, codeIndex);
			WorkflowUnescapeVisitor(tokens).InspectInto(ast.Obj());
			return ast;
		}

		Ptr<WfModule> ParseModule(const WString& input, const Parser& parser, vint codeIndex)
		{
			List<regex::RegexToken> tokens;
			parser.Tokenize(input, tokens, codeIndex);
			auto ast = parser.Parse_Module(tokens, codeIndex);
			WorkflowUnescapeVisitor(tokens).InspectInto(ast.Obj());
			return ast;
		}

/***********************************************************************
Print (Type)
***********************************************************************/

		class PrintTypeVisitor : public Object, public WfType::IVisitor
		{
		public:
			WString								indent;
			ParsingWriter&						writer;

			PrintTypeVisitor(const WString& _indent, ParsingWriter& _writer)
				:indent(_indent), writer(_writer)
			{
			}

			void Visit(WfPredefinedType* node)override
			{
				writer.BeforePrint(node);
				switch (node->name)
				{
				case WfPredefinedTypeName::Void:
					writer.WriteString(L"void");
					break;
				case WfPredefinedTypeName::Object:
					writer.WriteString(L"object");
					break;
				case WfPredefinedTypeName::Interface:
					writer.WriteString(L"interface");
					break;
				case WfPredefinedTypeName::Int:
					writer.WriteString(L"int");
					break;
				case WfPredefinedTypeName::UInt:
					writer.WriteString(L"uint");
					break;
				case WfPredefinedTypeName::Float:
					writer.WriteString(L"float");
					break;
				case WfPredefinedTypeName::Double:
					writer.WriteString(L"double");
					break;
				case WfPredefinedTypeName::String:
					writer.WriteString(L"string");
					break;
				case WfPredefinedTypeName::Char:
					writer.WriteString(L"char");
					break;
				case WfPredefinedTypeName::Bool:
					writer.WriteString(L"bool");
					break;
				default:
					CHECK_FAIL(L"Internal error: Unknown value.");
				}
				writer.AfterPrint(node);
			}

			void Visit(WfTopQualifiedType* node)override
			{
				writer.BeforePrint(node);
				writer.WriteString(L"::" + node->name.value);
				writer.AfterPrint(node);
			}

			void Visit(WfReferenceType* node)override
			{
				writer.BeforePrint(node);
				writer.WriteString(node->name.value);
				writer.AfterPrint(node);
			}

			void Visit(WfRawPointerType* node)override
			{
				writer.BeforePrint(node);
				WfPrint(node->element, indent, writer);
				writer.WriteString(L"*");
				writer.AfterPrint(node);
			}

			void Visit(WfSharedPointerType* node)override
			{
				writer.BeforePrint(node);
				WfPrint(node->element, indent, writer);
				writer.WriteString(L"^");
				writer.AfterPrint(node);
			}

			void Visit(WfNullableType* node)override
			{
				writer.BeforePrint(node);
				WfPrint(node->element, indent, writer);
				writer.WriteString(L"?");
				writer.AfterPrint(node);
			}

			void Visit(WfEnumerableType* node)override
			{
				writer.BeforePrint(node);
				WfPrint(node->element, indent, writer);
				writer.WriteString(L"{}");
				writer.AfterPrint(node);
			}

			void Visit(WfMapType* node)override
			{
				writer.BeforePrint(node);
				switch (node->writability)
				{
				case WfMapWritability::Readonly:
					writer.WriteString(L"const ");
					break;
				case WfMapWritability::Writable:
					break;
				default:
					CHECK_FAIL(L"Internal error: Unknown value.");
				}
				WfPrint(node->value, indent, writer);
				writer.WriteString(L"[");
				if (node->key)
				{
					WfPrint(node->key, indent, writer);
				}
				writer.WriteString(L"]");
				writer.AfterPrint(node);
			}

			void Visit(WfObservableListType* node)override
			{
				writer.BeforePrint(node);
				writer.WriteString(L"observe ");
				WfPrint(node->element, indent, writer);
				writer.WriteString(L"[]");
				writer.AfterPrint(node);
			}

			void Visit(WfFunctionType* node)override
			{
				writer.BeforePrint(node);
				writer.WriteString(L"(func ");
				writer.WriteString(L"(");
				for (auto [type, index] : indexed(node->arguments))
				{
					if (index > 0)
					{
						writer.WriteString(L", ");
					}
					WfPrint(type, indent, writer);
				}
				writer.WriteString(L")");

				writer.WriteString(L" : (");
				WfPrint(node->result, indent, writer);
				writer.WriteString(L"))");
				writer.AfterPrint(node);
			}

			void Visit(WfChildType* node)override
			{
				writer.BeforePrint(node);
				WfPrint(node->parent, indent, writer);
				writer.WriteString(L"::");
				writer.WriteString(node->name.value);
				writer.AfterPrint(node);
			}
		};

/***********************************************************************
Print (Expression)
***********************************************************************/

		class PrintExpressionVisitor
			: public Object
			, public WfExpression::IVisitor
			, public WfVirtualCfeExpression::IVisitor
			, public WfVirtualCseExpression::IVisitor
		{
		public:
			WString								indent;
			ParsingWriter&						writer;

			PrintExpressionVisitor(const WString& _indent, ParsingWriter& _writer)
				:indent(_indent), writer(_writer)
			{
			}

			void Visit(WfThisExpression* node)override
			{
				writer.BeforePrint(node);
				writer.WriteString(L"this");
				writer.AfterPrint(node);
			}

			void Visit(WfTopQualifiedExpression* node)override
			{
				writer.BeforePrint(node);
				writer.WriteString(L"::");
				writer.WriteString(node->name.value);
				writer.AfterPrint(node);
			}

			void Visit(WfReferenceExpression* node)override
			{
				writer.BeforePrint(node);
				writer.WriteString(node->name.value);
				writer.AfterPrint(node);
			}

			void Visit(WfOrderedNameExpression* node)override
			{
				writer.BeforePrint(node);
				writer.WriteString(node->name.value);
				writer.AfterPrint(node);
			}

			void Visit(WfOrderedLambdaExpression* node)override
			{
				writer.BeforePrint(node);
				writer.WriteString(L"[");
				WfPrint(node->body, indent, writer);
				writer.WriteString(L"]");
				writer.AfterPrint(node);
			}

			void Visit(WfMemberExpression* node)override
			{
				writer.BeforePrint(node);
				WfPrint(node->parent, indent, writer);
				writer.WriteString(L".");
				writer.WriteString(node->name.value);
				writer.AfterPrint(node);
			}

			void Visit(WfChildExpression* node)override
			{
				writer.BeforePrint(node);
				WfPrint(node->parent, indent, writer);
				writer.WriteString(L"::");
				writer.WriteString(node->name.value);
				writer.AfterPrint(node);
			}

			void Visit(WfLiteralExpression* node)override
			{
				writer.BeforePrint(node);
				switch (node->value)
				{
				case WfLiteralValue::Null:
					writer.WriteString(L"null");
					break;
				case WfLiteralValue::True:
					writer.WriteString(L"true");
					break;
				case WfLiteralValue::False:
					writer.WriteString(L"false");
					break;
				default:
					CHECK_FAIL(L"Internal error: Unknown value.");
				}
				writer.AfterPrint(node);
			}

			void Visit(WfFloatingExpression* node)override
			{
				writer.BeforePrint(node);
				writer.WriteString(node->value.value);
				writer.AfterPrint(node);
			}

			void Visit(WfIntegerExpression* node)override
			{
				writer.BeforePrint(node);
				writer.WriteString(node->value.value);
				writer.AfterPrint(node);
			}

			void Visit(WfStringExpression* node)override
			{
				writer.BeforePrint(node);
				EscapeString(node->value.value, writer);
				writer.AfterPrint(node);
			}

			void Visit(WfUnaryExpression* node)override
			{
				writer.BeforePrint(node);
				writer.WriteString(L"(");
				switch (node->op)
				{
				case WfUnaryOperator::Positive:
					writer.WriteString(L"+");
					break;
				case WfUnaryOperator::Negative:
					writer.WriteString(L"-");
					break;
				case WfUnaryOperator::Not:
					writer.WriteString(L"!");
					break;
				default:
					CHECK_FAIL(L"Internal error: Unknown value.");
				}
				WfPrint(node->operand, indent, writer);
				writer.WriteString(L")");
				writer.AfterPrint(node);
			}

			void Visit(WfBinaryExpression* node)override
			{
				writer.BeforePrint(node);
				if (node->op == WfBinaryOperator::Index)
				{
					WfPrint(node->first, indent, writer);
					writer.WriteString(L"[");
					WfPrint(node->second, indent, writer);
					writer.WriteString(L"]");
				}
				else
				{
					writer.WriteString(L"(");
					WfPrint(node->first, indent, writer);
					switch (node->op)
					{
					case WfBinaryOperator::Assign:
						writer.WriteString(L" = ");
						break;
					case WfBinaryOperator::FlagAnd:
						writer.WriteString(L" & ");
						break;
					case WfBinaryOperator::FlagOr:
						writer.WriteString(L" | ");
						break;
					case WfBinaryOperator::FailedThen:
						writer.WriteString(L" ?? ");
						break;
					case WfBinaryOperator::Exp:
						writer.WriteString(L" ^ ");
						break;
					case WfBinaryOperator::Add:
						writer.WriteString(L" + ");
						break;
					case WfBinaryOperator::Sub:
						writer.WriteString(L" - ");
						break;
					case WfBinaryOperator::Mul:
						writer.WriteString(L" * ");
						break;
					case WfBinaryOperator::Div:
						writer.WriteString(L" / ");
						break;
					case WfBinaryOperator::Mod:
						writer.WriteString(L" % ");
						break;
					case WfBinaryOperator::Shl:
						writer.WriteString(L" shl ");
						break;
					case WfBinaryOperator::Shr:
						writer.WriteString(L" shr ");
						break;
					case WfBinaryOperator::LT:
						writer.WriteString(L" < ");
						break;
					case WfBinaryOperator::GT:
						writer.WriteString(L" > ");
						break;
					case WfBinaryOperator::LE:
						writer.WriteString(L" <= ");
						break;
					case WfBinaryOperator::GE:
						writer.WriteString(L" >= ");
						break;
					case WfBinaryOperator::EQ:
						writer.WriteString(L" == ");
						break;
					case WfBinaryOperator::NE:
						writer.WriteString(L" != ");
						break;
					case WfBinaryOperator::Xor:
						writer.WriteString(L" xor ");
						break;
					case WfBinaryOperator::And:
						writer.WriteString(L" and ");
						break;
					case WfBinaryOperator::Or:
						writer.WriteString(L" or ");
						break;
					default:
						CHECK_FAIL(L"Internal error: Unknown value.");
					}
					WfPrint(node->second, indent, writer);
					writer.WriteString(L")");
				}
				writer.AfterPrint(node);
			}

			void Visit(WfLetExpression* node)override
			{
				writer.BeforePrint(node);
				writer.WriteString(L"let ");
				for (auto [var, index] : indexed(node->variables))
				{
					if (index > 0)
					{
						writer.WriteString(L", ");
					}
					writer.BeforePrint(var.Obj());
					writer.WriteString(var->name.value);
					writer.WriteString(L" = ");
					WfPrint(var->value, indent, writer);
					writer.AfterPrint(var.Obj());
				}
				writer.WriteString(L" in (");
				WfPrint(node->expression, indent, writer);
				writer.WriteString(L")");
				writer.AfterPrint(node);
			}

			void Visit(WfIfExpression* node)override
			{
				writer.BeforePrint(node);
				WfPrint(node->condition, indent, writer);
				writer.WriteString(L" ? ");
				WfPrint(node->trueBranch, indent, writer);
				writer.WriteString(L" : ");
				WfPrint(node->falseBranch, indent, writer);
				writer.AfterPrint(node);
			}

			void Visit(WfRangeExpression* node)override
			{
				writer.BeforePrint(node);
				writer.WriteString(L"range ");
				switch (node->beginBoundary)
				{
				case WfRangeBoundary::Exclusive:
					writer.WriteString(L"(");
					break;
				case WfRangeBoundary::Inclusive:
					writer.WriteString(L"[");
					break;
				default:
					CHECK_FAIL(L"Internal error: Unknown value.");
				}
				WfPrint(node->begin, indent, writer);
				writer.WriteString(L", ");
				WfPrint(node->end, indent, writer);
				switch (node->beginBoundary)
				{
				case WfRangeBoundary::Exclusive:
					writer.WriteString(L")");
					break;
				case WfRangeBoundary::Inclusive:
					writer.WriteString(L"]");
					break;
				default:
					CHECK_FAIL(L"Internal error: Unknown value.");
				}
				writer.AfterPrint(node);
			}

			void Visit(WfSetTestingExpression* node)override
			{
				writer.BeforePrint(node);
				WfPrint(node->element, indent, writer);
				switch (node->test)
				{
				case WfSetTesting::In:
					writer.WriteString(L" in ");
					break;
				case WfSetTesting::NotIn:
					writer.WriteString(L" not in ");
					break;
				default:
					CHECK_FAIL(L"Internal error: Unknown value.");
				}
				WfPrint(node->collection, indent, writer);
				writer.AfterPrint(node);
			}

			void Visit(WfConstructorExpression* node)override
			{
				writer.BeforePrint(node);
				writer.WriteString(L"{");
				for (auto [argument, index] : indexed(node->arguments))
				{
					if (index > 0)
					{
						writer.WriteString(L" ");
					}
					WfPrint(argument->key, indent, writer);
					if (argument->value)
					{
						writer.WriteString(L" : ");
						WfPrint(argument->value, indent, writer);
					}
					writer.WriteString(L";");
				}
				writer.WriteString(L"}");
				writer.AfterPrint(node);
			}

			void Visit(WfInferExpression* node)override
			{
				writer.BeforePrint(node);
				WfPrint(node->expression, indent, writer);
				writer.WriteString(L" of (");
				WfPrint(node->type, indent, writer);
				writer.WriteString(L")");
				writer.AfterPrint(node);
			}

			void Visit(WfTypeCastingExpression* node)override
			{
				writer.BeforePrint(node);
				switch (node->strategy)
				{
				case WfTypeCastingStrategy::Strong:
					writer.WriteString(L"(cast (");
					WfPrint(node->type, indent, writer);
					writer.WriteString(L") ");
					WfPrint(node->expression, indent, writer);
					writer.WriteString(L")");
					break;
				case WfTypeCastingStrategy::Weak:
					writer.WriteString(L"(");
					WfPrint(node->expression, indent, writer);
					writer.WriteString(L" as (");
					WfPrint(node->type, indent, writer);
					writer.WriteString(L"))");
					break;
				default:
					CHECK_FAIL(L"Internal error: Unknown value.");
				}
				writer.AfterPrint(node);
			}

			void Visit(WfTypeTestingExpression* node)override
			{
				writer.BeforePrint(node);
				WfPrint(node->expression, indent, writer);
				switch (node->test)
				{
				case WfTypeTesting::IsType:
					writer.WriteString(L" is (");
					WfPrint(node->type, indent, writer);
					writer.WriteString(L")");
					break;
				case WfTypeTesting::IsNotType:
					writer.WriteString(L" is not (");
					WfPrint(node->type, indent, writer);
					writer.WriteString(L")");
					break;
				case WfTypeTesting::IsNull:
					writer.WriteString(L" is null");
					break;
				case WfTypeTesting::IsNotNull:
					writer.WriteString(L" is not null");
					break;
				default:
					CHECK_FAIL(L"Internal error: Unknown value.");
				}
				writer.AfterPrint(node);
			}

			void Visit(WfTypeOfTypeExpression* node)override
			{
				writer.BeforePrint(node);
				writer.WriteString(L"typeof(");
				WfPrint(node->type, indent, writer);
				writer.WriteString(L")");
				writer.AfterPrint(node);
			}

			void Visit(WfTypeOfExpressionExpression* node)override
			{
				writer.BeforePrint(node);
				writer.WriteString(L"type(");
				WfPrint(node->expression, indent, writer);
				writer.WriteString(L")");
				writer.AfterPrint(node);
			}

			void Visit(WfAttachEventExpression* node)override
			{
				writer.BeforePrint(node);
				writer.WriteString(L"attach(");
				WfPrint(node->event, indent, writer);
				writer.WriteString(L", ");
				WfPrint(node->function, indent, writer);
				writer.WriteString(L")");
				writer.AfterPrint(node);
			}

			void Visit(WfDetachEventExpression* node)override
			{
				writer.BeforePrint(node);
				writer.WriteString(L"detach(");
				WfPrint(node->event, indent, writer);
				writer.WriteString(L", ");
				WfPrint(node->handler, indent, writer);
				writer.WriteString(L")");
				writer.AfterPrint(node);
			}

			void Visit(WfObserveExpression* node)override
			{
				writer.BeforePrint(node);
				WfPrint(node->parent, indent, writer);
				writer.WriteString(L".observe");
				switch (node->observeType)
				{
				case WfObserveType::ExtendedObserve:
					writer.WriteString(L" as ");
					writer.WriteString(node->name.value);
					break;
				case WfObserveType::SimpleObserve:
					break;
				default:
					CHECK_FAIL(L"Internal error: Unknown value.");
				}
				writer.WriteString(L"(");
				WfPrint(node->expression, indent, writer);
				if (node->events.Count() > 0)
				{
					writer.WriteString(L" on ");
					for (auto [argument, index] : indexed(node->events))
					{
						if (index > 0)
						{
							writer.WriteString(L", ");
						}
						WfPrint(argument, indent, writer);
					}
				}
				writer.WriteString(L")");
				writer.AfterPrint(node);
			}

			void Visit(WfCallExpression* node)override
			{
				writer.BeforePrint(node);
				WfPrint(node->function, indent, writer);
				writer.WriteString(L"(");
				for (auto [argument, index] : indexed(node->arguments))
				{
					if (index > 0)
					{
						writer.WriteString(L", ");
					}
					WfPrint(argument, indent, writer);
				}
				writer.WriteString(L")");
				writer.AfterPrint(node);
			}

			void Visit(WfFunctionExpression* node)override
			{
				writer.BeforePrint(node);
				WfPrint(Ptr<WfDeclaration>(node->function), indent, writer);
				writer.AfterPrint(node);
			}

			void Visit(WfNewClassExpression* node)override
			{
				writer.BeforePrint(node);
				writer.WriteString(L"new (");
				WfPrint(node->type, indent, writer);
				writer.WriteString(L")");

				writer.WriteString(L"(");
				for (auto [argument, index] : indexed(node->arguments))
				{
					if (index > 0)
					{
						writer.WriteString(L", ");
					}
					WfPrint(argument, indent, writer);
				}
				writer.WriteString(L")");

				writer.AfterPrint(node);
			}

			void Visit(WfNewInterfaceExpression* node)override
			{
				writer.BeforePrint(node);
				writer.WriteString(L"new (");
				WfPrint(node->type, indent, writer);
				writer.WriteString(L")");

				writer.WriteLine(L"");
				writer.WriteString(indent);
				writer.WriteLine(L"{");
				for (auto [decl, index] : indexed(node->declarations))
				{
					if (index > 0)
					{
						writer.WriteLine(L"");
					}

					writer.WriteString(indent + L"    ");
					WfPrint(decl, indent + L"    ", writer);
					writer.WriteLine(L"");
				}
				writer.WriteString(indent);
				writer.WriteString(L"}");

				writer.AfterPrint(node);
			}

			void Visit(WfVirtualCfeExpression* node)override
			{
				if (node->expandedExpression)
				{
					writer.BeforePrint(node);
					WfPrint(node->expandedExpression, indent, writer);
					writer.AfterPrint(node);
				}
				else
				{
					node->Accept((WfVirtualCfeExpression::IVisitor*)this);
				}
			}

			void Visit(WfFormatExpression* node)override
			{
				writer.BeforePrint(node);
				writer.WriteString(L"$");
				EscapeString(node->value.value, writer);
				writer.AfterPrint(node);
			}

			void Visit(WfVirtualCseExpression* node)override
			{
				if (node->expandedExpression)
				{
					writer.BeforePrint(node);
					WfPrint(node->expandedExpression, indent, writer);
					writer.AfterPrint(node);
				}
				else
				{
					node->Accept((WfVirtualCseExpression::IVisitor*)this);
				}
			}

			void Visit(WfBindExpression* node)override
			{
				writer.BeforePrint(node);
				writer.WriteString(L"bind(");
				WfPrint(node->expression, indent, writer);
				writer.WriteString(L")");
				writer.AfterPrint(node);
			}

			void Visit(WfNewCoroutineExpression* node)override
			{
				writer.BeforePrint(node);
				if (node->name.value == L"")
				{
					writer.WriteLine(L"$coroutine");
				}
				else
				{
					writer.WriteString(L"$coroutine(");
					writer.WriteString(node->name.value);
					writer.WriteLine(L")");
				}
				writer.WriteString(indent);
				WfPrint(node->statement, indent, writer);
				writer.AfterPrint(node);
			}

			void Visit(WfMixinCastExpression* node)override
			{
				writer.BeforePrint(node);
				writer.WriteString(L"new (");
				WfPrint(node->type, indent, writer);
				writer.WriteString(L")");

				writer.WriteString(L"(using ");
				WfPrint(node->expression, indent, writer);
				writer.WriteString(L")");
				writer.AfterPrint(node);
			}

			void Visit(WfExpectedTypeCastExpression* node)override
			{
				writer.BeforePrint(node);
				switch (node->strategy)
				{
				case WfTypeCastingStrategy::Strong:
					writer.WriteString(L"(cast * ");
					WfPrint(node->expression, indent, writer);
					writer.WriteString(L")");
					break;
				case WfTypeCastingStrategy::Weak:
					writer.WriteString(L"(");
					WfPrint(node->expression, indent, writer);
					writer.WriteString(L" as *)");
					break;
				default:
					CHECK_FAIL(L"Internal error: Unknown value.");
				}
				writer.AfterPrint(node);
			}

			void Visit(WfCoOperatorExpression* node)override
			{
				writer.BeforePrint(node);
				writer.WriteString(L"$.");
				writer.WriteString(node->name.value);
				writer.AfterPrint(node);
			}
		};

/***********************************************************************
Print (Statement)
***********************************************************************/

		class PrintStatementVisitor
			: public Object
			, public WfStatement::IVisitor
			, public WfVirtualCseStatement::IVisitor
			, public WfCoroutineStatement::IVisitor
			, public WfStateMachineStatement::IVisitor
		{
		public:
			WString								indent;
			ParsingWriter&						writer;

			PrintStatementVisitor(const WString& _indent, ParsingWriter& _writer)
				:indent(_indent), writer(_writer)
			{
			}

			void Visit(WfBreakStatement* node)override
			{
				writer.BeforePrint(node);
				writer.WriteString(L"break;");
				writer.AfterPrint(node);
			}

			void Visit(WfContinueStatement* node)override
			{
				writer.BeforePrint(node);
				writer.WriteString(L"continue;");
				writer.AfterPrint(node);
			}

			void Visit(WfReturnStatement* node)override
			{
				writer.BeforePrint(node);
				if (node->expression)
				{
					writer.WriteString(L"return ");
					WfPrint(node->expression, indent, writer);
					writer.WriteString(L";");
				}
				else
				{
					writer.WriteString(L"return;");
				}
				writer.AfterPrint(node);
			}

			void Visit(WfDeleteStatement* node)override
			{
				writer.BeforePrint(node);
				writer.WriteString(L"delete ");
				WfPrint(node->expression, indent, writer);
				writer.WriteString(L";");
				writer.AfterPrint(node);
			}

			void Visit(WfRaiseExceptionStatement* node)override
			{
				writer.BeforePrint(node);
				if (node->expression)
				{
					writer.WriteString(L"raise ");
					WfPrint(node->expression, indent, writer);
					writer.WriteString(L";");
				}
				else
				{
					writer.WriteString(L"raise;");
				}
				writer.AfterPrint(node);
			}

			void Visit(WfIfStatement* node)override
			{
				writer.BeforePrint(node);
				writer.WriteString(L"if (");
				if (node->type)
				{
					writer.WriteString(L"var ");
					writer.WriteString(node->name.value);
					writer.WriteString(L" : ");
					WfPrint(node->type, indent, writer);
					writer.WriteString(L" = ");
				}
				WfPrint(node->expression, indent, writer);
				writer.WriteLine(L")");

				writer.WriteString(indent);
				WfPrint(node->trueBranch, indent, writer);
				if (node->falseBranch)
				{
					writer.WriteLine(L"");
					writer.WriteString(indent);
					if (node->falseBranch.Cast<WfIfStatement>())
					{
						writer.WriteString(L"else ");
						WfPrint(node->falseBranch, indent, writer);
					}
					else
					{
						writer.WriteLine(L"else");
						writer.WriteString(indent);
						WfPrint(node->falseBranch, indent, writer);
					}
				}
				writer.AfterPrint(node);
			}

			void Visit(WfWhileStatement* node)override
			{
				writer.BeforePrint(node);
				writer.WriteString(L"while (");
				WfPrint(node->condition, indent, writer);
				writer.WriteLine(L")");
				writer.WriteString(indent);
				WfPrint(node->statement, indent, writer);
				writer.AfterPrint(node);
			}

			void Visit(WfTryStatement* node)override
			{
				writer.BeforePrint(node);
				writer.WriteLine(L"try");
				writer.WriteString(indent);
				WfPrint(node->protectedStatement, indent, writer);

				if (node->catchStatement)
				{
					writer.WriteLine(L"");
					writer.WriteString(indent);
					writer.WriteString(L"catch (");
					writer.WriteString(node->name.value);
					writer.WriteLine(L")");
					writer.WriteString(indent);
					WfPrint(node->catchStatement, indent, writer);
				}

				if (node->finallyStatement)
				{
					writer.WriteLine(L"");
					writer.WriteString(indent);
					writer.WriteLine(L"finally");
					writer.WriteString(indent);
					WfPrint(node->finallyStatement, indent, writer);
				}
				writer.AfterPrint(node);
			}

			void Visit(WfBlockStatement* node)override
			{
				writer.BeforePrint(node);
				writer.WriteLine(L"{");
				for (auto statement : node->statements)
				{
					writer.WriteString(indent + L"    ");
					WfPrint(statement, indent + L"    ", writer);
					writer.WriteLine(L"");
				}

				if (node->endLabel.value != L"")
				{
					writer.WriteString(indent + L"    ");
					writer.WriteString(node->endLabel.value);
					writer.WriteLine(L":;");
				}
				writer.WriteString(indent);
				writer.WriteString(L"}");
				writer.AfterPrint(node);
			}

			void Visit(WfGotoStatement* node)override
			{
				writer.BeforePrint(node);
				writer.WriteString(L"goto ");
				writer.WriteString(node->label.value);
				writer.WriteString(L";");
				writer.AfterPrint(node);
			}

			void Visit(WfExpressionStatement* node)override
			{
				writer.BeforePrint(node);
				WfPrint(node->expression, indent, writer);
				writer.WriteString(L";");
				writer.AfterPrint(node);
			}

			void Visit(WfVariableStatement* node)override
			{
				writer.BeforePrint(node);
				WfPrint(Ptr<WfDeclaration>(node->variable), indent, writer);
				writer.AfterPrint(node);
			}

			void Visit(WfVirtualCseStatement* node)override
			{
				if (node->expandedStatement)
				{
					writer.BeforePrint(node);
					WfPrint(node->expandedStatement, indent, writer);
					writer.AfterPrint(node);
				}
				else
				{
					node->Accept((WfVirtualCseStatement::IVisitor*)this);
				}
			}

			void Visit(WfSwitchStatement* node)override
			{
				writer.BeforePrint(node);
				writer.WriteString(L"switch (");
				WfPrint(node->expression, indent, writer);
				writer.WriteLine(L")");

				writer.WriteString(indent);
				writer.WriteLine(L"{");

				for (auto switchCase : node->caseBranches)
				{
					writer.BeforePrint(switchCase.Obj());
					writer.WriteString(indent);
					writer.WriteString(L"    case ");
					WfPrint(switchCase->expression, indent, writer);
					writer.WriteLine(L":");
					writer.WriteString(indent + L"    ");
					WfPrint(switchCase->statement, indent + L"    ", writer);
					writer.AfterPrint(switchCase.Obj());
					writer.WriteLine(L"");
				}
				if (node->defaultBranch)
				{
					writer.WriteString(indent);
					writer.WriteLine(L"    default:");
					writer.WriteString(indent + L"    ");
					WfPrint(node->defaultBranch, indent + L"    ", writer);
					writer.WriteLine(L"");
				}

				writer.WriteString(indent);
				writer.WriteString(L"}");
				writer.AfterPrint(node);
			}

			void Visit(WfForEachStatement* node)override
			{
				writer.BeforePrint(node);
				writer.WriteString(L"for (");
				writer.WriteString(node->name.value);
				writer.WriteString(L" in ");
				switch (node->direction)
				{
				case WfForEachDirection::Reversed:
					writer.WriteString(L"reversed ");
					break;
				case WfForEachDirection::Normal:
					break;
				default:
					CHECK_FAIL(L"Internal error: Unknown value.");
				}
				WfPrint(node->collection, indent, writer);
				writer.WriteLine(L")");
				writer.WriteString(indent);
				WfPrint(node->statement, indent, writer);
				writer.AfterPrint(node);
			}

			void Visit(WfCoProviderStatement* node)override
			{
				writer.BeforePrint(node);
				if (node->name.value == L"")
				{
					writer.WriteString(L"$");
				}
				else
				{
					writer.WriteString(node->name.value);
					writer.WriteString(L" ");
				}
				WfPrint(node->statement, indent, writer);
				writer.AfterPrint(node);
			}

			void Visit(WfCoroutineStatement* node)override
			{
				node->Accept((WfCoroutineStatement::IVisitor*)this);
			}

			void Visit(WfCoPauseStatement* node)override
			{
				writer.BeforePrint(node);
				if (node->statement)
				{
					writer.WriteLine(L"$pause");
					writer.WriteString(indent);
					WfPrint(node->statement, indent, writer);
				}
				else
				{
					writer.WriteString(L"$pause;");
				}
				writer.AfterPrint(node);
			}

			void Visit(WfCoOperatorStatement* node)override
			{
				if (node->varName.value != L"")
				{
					writer.WriteString(L"var ");
					writer.WriteString(node->varName.value);
					writer.WriteString(L" = ");
				}
				writer.WriteString(node->opName.value);

				for (auto [argument, index] : indexed(node->arguments))
				{
					writer.WriteString(index == 0 ? L" " : L", ");
					WfPrint(argument, indent, writer);
				}
				writer.WriteString(L";");
			}

			void Visit(WfStateMachineStatement* node)override
			{
				node->Accept((WfStateMachineStatement::IVisitor*)this);
			}

			void Visit(WfStateSwitchStatement* node)override
			{
				writer.BeforePrint(node);
				writer.WriteString(L"switch (");
				switch (node->type)
				{
				case WfStateSwitchType::Default:
					writer.WriteLine(L"raise)");
					break;
				case WfStateSwitchType::Pass:
					writer.WriteLine(L"continue)");
					break;
				case WfStateSwitchType::PassAndReturn:
					writer.WriteLine(L"continue, return)");
					break;
				case WfStateSwitchType::Ignore:
					writer.WriteLine(L")");
					break;
				case WfStateSwitchType::IgnoreAndReturn:
					writer.WriteLine(L"return)");
					break;
				default:
					CHECK_FAIL(L"Internal error: Unknown value.");
				}

				writer.WriteString(indent);
				writer.WriteLine(L"{");

				for (auto switchCase : node->caseBranches)
				{
					writer.BeforePrint(switchCase.Obj());
					writer.WriteString(indent);
					writer.WriteString(L"    case ");
					writer.WriteString(switchCase->name.value);
					writer.WriteString(L"(");
					for (auto [argument, index] : indexed(switchCase->arguments))
					{
						if (index != 0) writer.WriteString(L", ");
						writer.BeforePrint(argument.Obj());
						writer.WriteString(argument->name.value);
						writer.AfterPrint(argument.Obj());
					}
					writer.WriteLine(L"):");
					writer.WriteString(indent + L"    ");
					WfPrint(switchCase->statement, indent + L"    ", writer);
					writer.AfterPrint(switchCase.Obj());
					writer.WriteLine(L"");
				}

				writer.WriteString(indent);
				writer.WriteString(L"}");
				writer.AfterPrint(node);
			}

			void Visit(WfStateInvokeStatement* node)override
			{
				writer.BeforePrint(node);
				switch (node->type)
				{
				case WfStateInvokeType::Goto:
					writer.WriteString(L"$goto_state ");
					break;
				case WfStateInvokeType::Push:
					writer.WriteString(L"push_state ");
					break;
				default:
					CHECK_FAIL(L"Internal error: Unknown value.");
				}

				writer.WriteString(node->name.value);
				writer.WriteString(L"(");
				for (auto [argument, index] : indexed(node->arguments))
				{
					if (index != 0) writer.WriteString(L", ");
					WfPrint(argument, indent, writer);
				}
				writer.WriteString(L");");
				writer.AfterPrint(node);
			}
		};

/***********************************************************************
Print (Declaration)
***********************************************************************/

		class PrintDeclarationVisitor
			: public Object
			, public WfDeclaration::IVisitor
			, public WfVirtualCfeDeclaration::IVisitor
			, public WfVirtualCseDeclaration::IVisitor
		{
		public:
			WString								indent;
			ParsingWriter&						writer;

			PrintDeclarationVisitor(const WString& _indent, ParsingWriter& _writer)
				:indent(_indent), writer(_writer)
			{
			}

			void Visit(WfNamespaceDeclaration* node)override
			{
				writer.BeforePrint(node);
				writer.WriteLine(L"namespace " + node->name.value);
				writer.WriteString(indent);
				writer.WriteLine(L"{");
				for (auto [decl, index] : indexed(node->declarations))
				{
					if (index != 0)
					{
						writer.WriteLine(L"");
					}
					writer.WriteString(indent + L"    ");
					WfPrint(decl, indent + L"    ", writer);
					writer.WriteLine(L"");
				}
				writer.WriteString(indent);
				writer.WriteString(L"}");
				writer.AfterPrint(node);
			}

			void Visit(WfFunctionDeclaration* node)override
			{
				switch (node->functionKind)
				{
				case WfFunctionKind::Normal:
					break;
				case WfFunctionKind::Static:
					writer.WriteString(L"static ");
					break;
				case WfFunctionKind::Override:
					writer.WriteString(L"override ");
					break;
				default:
					CHECK_FAIL(L"Internal error: Unknown value.");
				}

				writer.BeforePrint(node);
				writer.WriteString(L"func ");
				switch (node->anonymity)
				{
				case WfFunctionAnonymity::Named:
					writer.WriteString(node->name.value);
					break;
				case WfFunctionAnonymity::Anonymous:
					break;
				default:
					CHECK_FAIL(L"Internal error: Unknown value.");
				}

				writer.WriteString(L"(");
				for (auto [argument, index] : indexed(node->arguments))
				{
					if (index > 0)
					{
						writer.WriteString(L", ");
					}
					writer.BeforePrint(argument.Obj());
					for (auto attribute : argument->attributes)
					{
						WfPrint(attribute, indent, writer);
						writer.WriteString(L" ");
					}
					writer.WriteString(argument->name.value);
					writer.WriteString(L" : ");
					WfPrint(argument->type, indent, writer);
					writer.AfterPrint(argument.Obj());
				}
				writer.WriteString(L")");

				writer.WriteString(L" : (");
				WfPrint(node->returnType, indent, writer);
				writer.WriteString(L")");
				if (node->statement)
				{
					writer.WriteLine(L"");

					writer.WriteString(indent);
					WfPrint(node->statement, indent, writer);
				}
				else
				{
					writer.WriteString(L";");
				}
				writer.AfterPrint(node);
			}

			void Visit(WfVariableDeclaration* node)override
			{
				writer.BeforePrint(node);
				writer.WriteString(L"var ");
				writer.WriteString(node->name.value);
				if (node->type)
				{
					writer.WriteString(L" : ");
					WfPrint(node->type, indent, writer);
				}
				writer.WriteString(L" = ");
				WfPrint(node->expression, indent, writer);
				writer.WriteString(L";");
				writer.AfterPrint(node);
			}

			void Visit(WfEventDeclaration* node)override
			{
				writer.BeforePrint(node);
				writer.WriteString(L"event ");
				writer.WriteString(node->name.value);
				writer.WriteString(L"(");
				for (auto [type, index] : indexed(node->arguments))
				{
					if (index != 0)
					{
						writer.WriteString(L", ");
					}
					WfPrint(type, indent, writer);
				}
				writer.WriteString(L");");
				writer.AfterPrint(node);
			}

			void Visit(WfPropertyDeclaration* node)override
			{
				writer.BeforePrint(node);
				writer.WriteString(L"prop ");
				writer.WriteString(node->name.value);
				writer.WriteString(L" : ");
				WfPrint(node->type, indent, writer);
				writer.WriteString(L"{");
				writer.WriteString(node->getter.value);
				if (node->setter.value != L"")
				{
					writer.WriteString(L", ");
					writer.WriteString(node->setter.value);
				}
				if (node->valueChangedEvent.value != L"")
				{
					writer.WriteString(L" : ");
					writer.WriteString(node->valueChangedEvent.value);
				}
				writer.WriteString(L"}");
				writer.AfterPrint(node);
			}

			void Visit(WfStaticInitDeclaration* node)override
			{
				writer.BeforePrint(node);
				writer.WriteLine(L"static");
				writer.WriteString(indent);
				WfPrint(node->statement, indent, writer);
				writer.AfterPrint(node);
			}

			void Visit(WfConstructorDeclaration* node)override
			{
				writer.BeforePrint(node);
				switch (node->constructorType)
				{
				case WfConstructorType::RawPtr:
					writer.WriteString(L"new* ");
					break;
				case WfConstructorType::SharedPtr:
					writer.WriteString(L"new ");
					break;
				default:
					CHECK_FAIL(L"Internal error: Unknown value.");
				}
				
				writer.WriteString(L"(");
				for (auto [argument, index] : indexed(node->arguments))
				{
					if (index > 0)
					{
						writer.WriteString(L", ");
					}
					writer.BeforePrint(argument.Obj());
					writer.WriteString(argument->name.value);
					writer.WriteString(L" : ");
					WfPrint(argument->type, indent, writer);
					writer.AfterPrint(argument.Obj());
				}
				writer.WriteString(L")");
				for (auto [call, callIndex] : indexed(node->baseConstructorCalls))
				{
					writer.WriteLine(L"");
					writer.WriteString(indent + L"    ");
					if (callIndex == 0)
					{
						writer.WriteString(L":");
					}
					else
					{
						writer.WriteString(L",");
					}
					writer.BeforePrint(call.Obj());
					WfPrint(call->type, indent + L"    ", writer);
					writer.WriteString(L"(");
					for (auto [argument, argumentIndex] : indexed(call->arguments))
					{
						if (argumentIndex != 0)
						{
							writer.WriteString(L", ");
						}
						WfPrint(argument, indent + L"    ", writer);
					}
					writer.WriteString(L")");
					writer.AfterPrint(call.Obj());
				}

				writer.WriteLine(L"");
				writer.WriteString(indent);
				WfPrint(node->statement, indent, writer);
				writer.AfterPrint(node);
			}

			void Visit(WfDestructorDeclaration* node)override
			{
				writer.BeforePrint(node);
				writer.WriteLine(L"delete");
				writer.WriteString(indent);
				WfPrint(node->statement, indent, writer);
				writer.AfterPrint(node);
			}

			void Visit(WfClassDeclaration* node)override
			{
				writer.BeforePrint(node);
				switch (node->kind)
				{
				case WfClassKind::Class:
					writer.WriteString(L"class ");
					break;
				case WfClassKind::Interface:
					writer.WriteString(L"interface ");
					break;
				default:
					CHECK_FAIL(L"Internal error: Unknown value.");
				}
				writer.WriteString(node->name.value);
				switch (node->kind)
				{
				case WfClassKind::Class:
					switch (node->constructorType)
					{
					case WfConstructorType::Undefined:
						break;
					default:
						CHECK_FAIL(L"Internal error: Unknown value.");
					}
					break;
				case WfClassKind::Interface:
					switch (node->constructorType)
					{
					case WfConstructorType::RawPtr:
						writer.WriteString(L"*");
					case WfConstructorType::SharedPtr:
						break;
					default:
						CHECK_FAIL(L"Internal error: Unknown value.");
					}
					break;
				default:;
				}

				for (auto [type, index] : indexed(node->baseTypes))
				{
					if (index == 0)
					{
						writer.WriteString(L" : ");
					}
					else
					{
						writer.WriteString(L", ");
					}
					WfPrint(type, indent, writer);
				}

				writer.WriteLine(L"");
				writer.WriteLine(indent + L"{");

				for (auto [decl, index] : indexed(node->declarations))
				{
					if (index > 0)
					{
						writer.WriteLine(L"");
					}

					writer.WriteString(indent + L"    ");
					WfPrint(decl, indent + L"    ", writer);
					writer.WriteLine(L"");
				}

				writer.WriteString(indent + L"}");
				writer.AfterPrint(node);
			}

			void Visit(WfEnumDeclaration* node)override
			{
				writer.BeforePrint(node);
				switch (node->kind)
				{
				case WfEnumKind::Normal:
					writer.WriteString(L"enum ");
					break;
				case WfEnumKind::Flag:
					writer.WriteString(L"flagenum ");
					break;
				default:
					CHECK_FAIL(L"Internal error: Unknown value.");
				}
				writer.WriteLine(node->name.value);
				writer.WriteLine(indent + L"{");

				auto newIndent = indent + L"    ";
				for (auto item : node->items)
				{
					writer.BeforePrint(item.Obj());
					for (auto attribute : item->attributes)
					{
						writer.WriteString(newIndent);
						WfPrint(attribute, newIndent, writer);
						writer.WriteLine(L"");
					}
					writer.WriteString(newIndent);
					writer.WriteString(item->name.value);
					writer.WriteString(L" = ");
					switch (item->kind)
					{
					case WfEnumItemKind::Constant:
						writer.WriteString(item->number.value);
						break;
					case WfEnumItemKind::Intersection:
						for (auto [itemInt, index] : indexed(item->intersections))
						{
							if (index != 0)writer.WriteString(L" | ");
							writer.WriteString(itemInt->name.value);
						}
						break;
					default:
						CHECK_FAIL(L"Internal error: Unknown value.");
					}
					writer.AfterPrint(item.Obj());
					writer.WriteLine(L",");
				}

				writer.WriteString(indent + L"}");
				writer.AfterPrint(node);
			}

			void Visit(WfStructDeclaration* node)override
			{
				writer.BeforePrint(node);
				writer.WriteString(L"struct ");
				writer.WriteLine(node->name.value);
				writer.WriteLine(indent + L"{");

				auto newIndent = indent + L"    ";
				for (auto member : node->members)
				{
					writer.BeforePrint(member.Obj());
					for (auto attribute : member->attributes)
					{
						writer.WriteString(newIndent);
						WfPrint(attribute, newIndent, writer);
						writer.WriteLine(L"");
					}
					writer.WriteString(newIndent);
					writer.WriteString(member->name.value);
					writer.WriteString(L" : ");
					WfPrint(member->type, newIndent, writer);
					writer.WriteLine(L";");
					writer.AfterPrint(member.Obj());
				}

				writer.WriteString(indent + L"}");
				writer.AfterPrint(node);
			}

			void PrintExpandedDeclarations(List<Ptr<WfDeclaration>>& decls)
			{
				for (auto [decl, index] : indexed(decls))
				{
					if (index > 0)
					{
						writer.WriteLine(L"");
						writer.WriteString(indent);
					}
					WfPrint(decl, indent, writer);
					if (index < decls.Count() - 1)
					{
						writer.WriteLine(L"");
					}
				}
			}

			void Visit(WfVirtualCfeDeclaration* node)override
			{
				if (node->expandedDeclarations.Count() > 0)
				{
					writer.BeforePrint(node);
					PrintExpandedDeclarations(node->expandedDeclarations);
					writer.AfterPrint(node);
				}
				else
				{
					node->Accept(static_cast<WfVirtualCfeDeclaration::IVisitor*>(this));
				}
			}

			void Visit(WfAutoPropertyDeclaration* node)override
			{
				switch (node->functionKind)
				{
				case WfFunctionKind::Normal:
					break;
				case WfFunctionKind::Static:
					writer.WriteString(L"static ");
					break;
				case WfFunctionKind::Override:
					writer.WriteString(L"override ");
					break;
				default:
					CHECK_FAIL(L"Internal error: Unknown value.");
				}

				writer.BeforePrint(node);
				writer.WriteString(L"prop ");
				writer.WriteString(node->name.value);
				writer.WriteString(L" : ");
				WfPrint(node->type, indent, writer);
				if (node->expression)
				{
					writer.WriteString(L" = ");
					WfPrint(node->expression, indent, writer);
				}
				writer.WriteString(L" {");
				switch (node->configConst)
				{
				case WfAPConst::Readonly:
					switch (node->configObserve)
					{
					case WfAPObserve::NotObservable:
						writer.WriteString(L"const, not observe");
						break;
					case WfAPObserve::Observable:
						writer.WriteString(L"const");
						break;
					default:
						CHECK_FAIL(L"Internal error: Unknown value.");
					}
					break;
				case WfAPConst::Writable:
					switch (node->configObserve)
					{
					case WfAPObserve::NotObservable:
						writer.WriteString(L"not observe");
						break;
					case WfAPObserve::Observable:
						break;
					default:
						CHECK_FAIL(L"Internal error: Unknown value.");
					}
					break;
				default:
					CHECK_FAIL(L"Internal error: Unknown value.");
				}
				writer.WriteString(L"}");
				writer.AfterPrint(node);
			}

			void Visit(WfCastResultInterfaceDeclaration* node)override
			{
				writer.BeforePrint(node);
				writer.WriteString(L"$interface ");
				writer.WriteString(node->name.value);
				writer.WriteString(L" : ");
				WfPrint(node->baseType, indent, writer);
				writer.WriteString(L"<");
				WfPrint(node->elementType, indent, writer);
				writer.WriteString(L">;");
				writer.AfterPrint(node);
			}

			void Visit(WfVirtualCseDeclaration* node)override
			{
				if (node->expandedDeclarations.Count() > 0)
				{
					writer.BeforePrint(node);
					PrintExpandedDeclarations(node->expandedDeclarations);
					writer.AfterPrint(node);
				}
				else
				{
					node->Accept(static_cast<WfVirtualCseDeclaration::IVisitor*>(this));
				}
			}

			void Visit(WfStateMachineDeclaration* node)override
			{
				writer.BeforePrint(node);
				writer.WriteLine(L"$state_machine");
				writer.WriteLine(indent + L"{");

				for (auto [input, index] : indexed(node->inputs))
				{
					if (index != 0) writer.WriteLine(L"");

					writer.BeforePrint(input.Obj());
					writer.WriteString(indent + L"    $state_input ");
					writer.WriteString(input->name.value);
					writer.WriteString(L"(");
					for (auto [argument, index] : indexed(input->arguments))
					{
						if (index > 0)
						{
							writer.WriteString(L", ");
						}
						writer.BeforePrint(argument.Obj());
						writer.WriteString(argument->name.value);
						writer.WriteString(L" : ");
						WfPrint(argument->type, indent, writer);
						writer.AfterPrint(argument.Obj());
					}
					writer.WriteLine(L");");
					writer.AfterPrint(input.Obj());
				}

				for (auto [state, index] : indexed(node->states))
				{
					if (index != 0 || node->inputs.Count() > 0) writer.WriteLine(L"");

					writer.BeforePrint(state.Obj());
					writer.WriteString(indent + L"    $state ");
					if (state->name.value == L"")
					{
						writer.WriteString(L"default");
					}
					else
					{
						writer.WriteString(state->name.value);
					}
					writer.WriteString(L"(");
					for (auto [argument, index] : indexed(state->arguments))
					{
						if (index > 0)
						{
							writer.WriteString(L", ");
						}
						writer.BeforePrint(argument.Obj());
						writer.WriteString(argument->name.value);
						writer.WriteString(L" : ");
						WfPrint(argument->type, indent, writer);
						writer.AfterPrint(argument.Obj());
					}
					writer.WriteLine(L")");
					writer.WriteString(indent + L"    ");
					WfPrint(state->statement, indent + L"    ", writer);
					writer.AfterPrint(state.Obj());
					writer.WriteLine(L"");
				}

				writer.WriteString(indent + L"}");
				writer.AfterPrint(node);
			}
		};

/***********************************************************************
Print (Module)
***********************************************************************/

		void WfPrint(Ptr<WfAttribute> node, const WString& indent, glr::ParsingWriter& writer)
		{
			writer.BeforePrint(node.Obj());
			writer.WriteString(L"@");
			writer.WriteString(node->category.value);
			writer.WriteString(L":");
			writer.WriteString(node->name.value);
			if (node->value)
			{
				writer.WriteString(L"(");
				WfPrint(node->value, indent, writer);
				writer.WriteString(L")");
			}
			writer.AfterPrint(node.Obj());
		}

		void WfPrint(Ptr<WfType> node, const WString& indent, glr::ParsingWriter& writer)
		{
			PrintTypeVisitor visitor(indent, writer);
			node->Accept(&visitor);
		}

		void WfPrint(Ptr<WfExpression> node, const WString& indent, glr::ParsingWriter& writer)
		{
			PrintExpressionVisitor visitor(indent, writer);
			node->Accept(&visitor);
		}

		void WfPrint(Ptr<WfStatement> node, const WString& indent, glr::ParsingWriter& writer)
		{
			PrintStatementVisitor visitor(indent, writer);
			node->Accept(&visitor);
		}

		void WfPrint(Ptr<WfDeclaration> node, const WString& indent, glr::ParsingWriter& writer)
		{
			for (auto attribute : node->attributes)
			{
				WfPrint(attribute, indent, writer);
				writer.WriteLine(L"");
				writer.WriteString(indent);
			}

			PrintDeclarationVisitor visitor(indent, writer);
			node->Accept(&visitor);
		}

		void WfPrint(Ptr<WfModule> node, const WString& indent, glr::ParsingWriter& writer)
		{
			writer.WriteString(indent);
			switch (node->moduleType)
			{
			case WfModuleType::Module:
				writer.WriteLine(L"module " + node->name.value + L";");
				break;
			case WfModuleType::Unit:
				writer.WriteLine(L"unit " + node->name.value + L";");
				break;
			default:
				CHECK_FAIL(L"Internal error: Unknown value.");
			}

			for (auto path : node->paths)
			{
				writer.WriteString(indent);
				writer.WriteString(L"using ");
				for (auto [item, index] : indexed(path->items))
				{
					if (index > 0)
					{
						writer.WriteString(L"::");
					}
					for (auto fragment : item->fragments)
					{
						if (auto name = fragment.Cast<WfModuleUsingNameFragment>())
						{
							writer.WriteString(name->name.value);
						}
						else
						{
							writer.WriteString(L"*");
						}
					}
				}
				writer.WriteLine(L";");
			}

			for (auto decl : node->declarations)
			{
				writer.WriteLine(L"");
				writer.WriteString(indent);
				WfPrint(decl, indent, writer);
				writer.WriteLine(L"");
			}
		}

/***********************************************************************
Print (Module)
***********************************************************************/

		void WfPrint(Ptr<WfAttribute> node, const WString& indent, stream::TextWriter& writer)
		{
			ParsingWriter parsingWriter(writer);
			WfPrint(node, indent, parsingWriter);
		}

		void WfPrint(Ptr<WfType> node, const WString& indent, stream::TextWriter& writer)
		{
			ParsingWriter parsingWriter(writer);
			WfPrint(node, indent, parsingWriter);
		}

		void WfPrint(Ptr<WfExpression> node, const WString& indent, stream::TextWriter& writer)
		{
			ParsingWriter parsingWriter(writer);
			WfPrint(node, indent, parsingWriter);
		}

		void WfPrint(Ptr<WfStatement> node, const WString& indent, stream::TextWriter& writer)
		{
			ParsingWriter parsingWriter(writer);
			WfPrint(node, indent, parsingWriter);
		}

		void WfPrint(Ptr<WfDeclaration> node, const WString& indent, stream::TextWriter& writer)
		{
			ParsingWriter parsingWriter(writer);
			WfPrint(node, indent, parsingWriter);
		}

		void WfPrint(Ptr<WfModule> node, const WString& indent, stream::TextWriter& writer)
		{
			ParsingWriter parsingWriter(writer);
			WfPrint(node, indent, parsingWriter);
		}
	}
}
