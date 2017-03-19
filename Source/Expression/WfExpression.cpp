#include "WfExpression.h"

namespace vl
{
	namespace workflow
	{
		using namespace stream;
		using namespace collections;
		using namespace parsing;

/***********************************************************************
Unescaping Functions
***********************************************************************/

		void SetDefaultClassMember(vl::collections::List<vl::Ptr<WfDeclaration>>& value, const vl::collections::List<vl::regex::RegexToken>& tokens)
		{
			FOREACH(Ptr<WfDeclaration>, decl, value)
			{
				if (!decl->classMember)
				{
					decl->classMember = MakePtr<WfClassMember>();
					decl->classMember->codeRange = decl->codeRange;
					decl->classMember->kind = WfClassMemberKind::Normal;
				}
			}
		}

		void UnescapeStringInternal(vl::parsing::ParsingToken& value, bool formatString)
		{
			MemoryStream memoryStream;
			{
				WString input = formatString ? value.value.Sub(2, value.value.Length() - 3) : value.value.Sub(1, value.value.Length() - 2);
				const wchar_t* reading = input.Buffer();
				StreamWriter writer(memoryStream);

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
			}

			memoryStream.SeekFromBegin(0);
			{
				StreamReader reader(memoryStream);
				value.value = reader.ReadToEnd();
			}
		}

		void UnescapeFormatString(vl::parsing::ParsingToken& value, const vl::collections::List<vl::regex::RegexToken>& tokens)
		{
			UnescapeStringInternal(value, true);
		}

		void UnescapeString(vl::parsing::ParsingToken& value, const vl::collections::List<vl::regex::RegexToken>& tokens)
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
				if (node->writability == WfMapWritability::Readonly)
				{
					writer.WriteString(L"const ");
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

			void Visit(WfFunctionType* node)override
			{
				writer.BeforePrint(node);
				writer.WriteString(L"(func ");
				writer.WriteString(L"(");
				FOREACH_INDEXER(Ptr<WfType>, type, index, node->arguments)
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
			, public WfVirtualExpression::IVisitor
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
					case WfBinaryOperator::Union:
						writer.WriteString(L" & ");
						break;
					case WfBinaryOperator::Intersect:
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
					default:;
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
				FOREACH_INDEXER(Ptr<WfLetVariable>, var, index, node->variables)
				{
					if (index > 0)
					{
						writer.WriteString(L", ");
					}
					writer.WriteString(var->name.value);
					writer.WriteString(L" = ");
					WfPrint(var->value, indent, writer);
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
				writer.WriteString(node->beginBoundary == WfRangeBoundary::Exclusive ? L"(" : L"[");
				WfPrint(node->begin, indent, writer);
				writer.WriteString(L", ");
				WfPrint(node->end, indent, writer);
				writer.WriteString(node->endBoundary == WfRangeBoundary::Exclusive ? L")" : L"]");
				writer.AfterPrint(node);
			}

			void Visit(WfSetTestingExpression* node)override
			{
				writer.BeforePrint(node);
				WfPrint(node->element, indent, writer);
				if (node->test == WfSetTesting::NotIn)
				{
					writer.WriteString(L" not");
				}
				writer.WriteString(L" in ");
				WfPrint(node->collection, indent, writer);
				writer.AfterPrint(node);
			}

			void Visit(WfConstructorExpression* node)override
			{
				writer.BeforePrint(node);
				writer.WriteString(L"{");
				FOREACH_INDEXER(Ptr<WfConstructorArgument>, argument, index, node->arguments)
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
				if (node->strategy == WfTypeCastingStrategy::Strong)
				{
					writer.WriteString(L"(cast (");
					WfPrint(node->type, indent, writer);
					writer.WriteString(L") ");
					WfPrint(node->expression, indent, writer);
					writer.WriteString(L")");
				}
				else
				{
					writer.WriteString(L"(");
					WfPrint(node->expression, indent, writer);
					writer.WriteString(L" as (");
					WfPrint(node->type, indent, writer);
					writer.WriteString(L"))");
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
				if (node->observeType == WfObserveType::ExtendedObserve)
				{
					writer.WriteString(L" as ");
					writer.WriteString(node->name.value);
				}
				writer.WriteString(L"(");
				WfPrint(node->expression, indent, writer);
				if (node->events.Count() > 0)
				{
					writer.WriteString(L" on ");
					FOREACH_INDEXER(Ptr<WfExpression>, argument, index, node->events)
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
				FOREACH_INDEXER(Ptr<WfExpression>, argument, index, node->arguments)
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
				FOREACH_INDEXER(Ptr<WfExpression>, argument, index, node->arguments)
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
				FOREACH_INDEXER(Ptr<WfDeclaration>, decl, index, node->declarations)
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

			void Visit(WfVirtualExpression* node)override
			{
				node->Accept((WfVirtualExpression::IVisitor*)this);
			}

			void Visit(WfBindExpression* node)override
			{
				writer.BeforePrint(node);
				if (node->expandedExpression)
				{
					WfPrint(node->expandedExpression, indent, writer);
				}
				else
				{
					writer.WriteString(L"bind(");
					WfPrint(node->expression, indent, writer);
					writer.WriteString(L")");
				}
				writer.AfterPrint(node);
			}

			void Visit(WfFormatExpression* node)override
			{
				writer.BeforePrint(node);
				if (node->expandedExpression)
				{
					WfPrint(node->expandedExpression, indent, writer);
				}
				else
				{
					writer.WriteString(L"$");
					EscapeString(node->value.value, writer);
				}
				writer.AfterPrint(node);
			}

			void Visit(WfNewCoroutineExpression* node)override
			{
				writer.BeforePrint(node);
				writer.WriteLine(L"$coroutine");
				writer.WriteString(indent);
				WfPrint(node->statement, indent, writer);
				writer.AfterPrint(node);
			}
		};

/***********************************************************************
Print (Statement)
***********************************************************************/

		class PrintStatementVisitor
			: public Object
			, public WfStatement::IVisitor
			, public WfVirtualStatement::IVisitor
			, public WfCoroutineStatement::IVisitor
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
				FOREACH(Ptr<WfStatement>, statement, node->statements)
				{
					writer.WriteString(indent + L"    ");
					WfPrint(statement, indent + L"    ", writer);
					writer.WriteLine(L"");
				}
				writer.WriteString(indent);
				writer.WriteString(L"}");
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

			void Visit(WfVirtualStatement* node)override
			{
				node->Accept((WfVirtualStatement::IVisitor*)this);
			}

			void Visit(WfSwitchStatement* node)override
			{
				writer.BeforePrint(node);
				writer.WriteString(L"switch (");
				WfPrint(node->expression, indent, writer);
				writer.WriteLine(L")");

				writer.WriteString(indent);
				writer.WriteLine(L"{");

				FOREACH(Ptr<WfSwitchCase>, switchCase, node->caseBranches)
				{
					writer.WriteString(indent);
					writer.WriteString(L"    case ");
					WfPrint(switchCase->expression, indent, writer);
					writer.WriteLine(L":");
					writer.WriteString(indent + L"    ");
					WfPrint(switchCase->statement, indent + L"    ", writer);
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
				if (node->direction == WfForEachDirection::Reversed)
				{
					writer.WriteString(L"reversed ");
				}
				WfPrint(node->collection, indent, writer);
				writer.WriteLine(L")");
				writer.WriteString(indent);
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
		};

/***********************************************************************
Print (Declaration)
***********************************************************************/

		class PrintDeclarationVisitor
			: public Object
			, public WfDeclaration::IVisitor
			, public WfVirtualDeclaration::IVisitor
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
				FOREACH_INDEXER(Ptr<WfDeclaration>, decl, index, node->declarations)
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
				writer.BeforePrint(node);
				writer.WriteString(L"func ");
				if (node->anonymity == WfFunctionAnonymity::Named)
				{
					writer.WriteString(node->name.value);
				}

				writer.WriteString(L"(");
				FOREACH_INDEXER(Ptr<WfFunctionArgument>, argument, index, node->arguments)
				{
					if (index > 0)
					{
						writer.WriteString(L", ");
					}
					FOREACH(Ptr<WfAttribute>, attribute, argument->attributes)
					{
						WfPrint(attribute, indent, writer);
						writer.WriteString(L" ");
					}
					writer.WriteString(argument->name.value);
					writer.WriteString(L" : ");
					WfPrint(argument->type, indent, writer);
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
				FOREACH_INDEXER(Ptr<WfType>, type, index, node->arguments)
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

			void Visit(WfConstructorDeclaration* node)override
			{
				writer.BeforePrint(node);
				if (node->constructorType == WfConstructorType::RawPtr)
				{
					writer.WriteString(L"new* ");
				}
				else
				{
					writer.WriteString(L"new ");
				}
				
				writer.WriteString(L"(");
				FOREACH_INDEXER(Ptr<WfFunctionArgument>, argument, index, node->arguments)
				{
					if (index > 0)
					{
						writer.WriteString(L", ");
					}
					writer.WriteString(argument->name.value);
					writer.WriteString(L" : ");
					WfPrint(argument->type, indent, writer);
				}
				writer.WriteString(L")");
				FOREACH_INDEXER(Ptr<WfBaseConstructorCall>, call, callIndex, node->baseConstructorCalls)
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
					WfPrint(call->type, indent + L"    ", writer);
					writer.WriteString(L"(");
					FOREACH_INDEXER(Ptr<WfExpression>, argument, argumentIndex, call->arguments)
					{
						if (argumentIndex != 0)
						{
							writer.WriteString(L", ");
						}
						WfPrint(argument, indent + L"    ", writer);
					}
					writer.WriteString(L")");
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
				}
				writer.WriteString(node->name.value);
				if (node->constructorType == WfConstructorType::RawPtr)
				{
					writer.WriteString(L"*");
				}

				FOREACH_INDEXER(Ptr<WfType>, type, index, node->baseTypes)
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

				FOREACH_INDEXER(Ptr<WfDeclaration>, decl, index, node->declarations)
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
				}
				writer.WriteLine(node->name.value);
				writer.WriteLine(indent + L"{");

				auto newIndent = indent + L"    ";
				FOREACH(Ptr<WfEnumItem>, item, node->items)
				{
					FOREACH(Ptr<WfAttribute>, attribute, item->attributes)
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
						FOREACH_INDEXER(Ptr<WfEnumItemIntersection>, itemInt, index, item->intersections)
						{
							if (index != 0)writer.WriteString(L" | ");
							writer.WriteString(itemInt->name.value);
						}
						break;
					}
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
				FOREACH(Ptr<WfStructMember>, member, node->members)
				{
					FOREACH(Ptr<WfAttribute>, attribute, member->attributes)
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
				}

				writer.WriteString(indent + L"}");
				writer.AfterPrint(node);
			}

			void Visit(WfVirtualDeclaration* node)override
			{
				if (node->expandedDeclarations.Count() > 0)
				{
					FOREACH_INDEXER(Ptr<WfDeclaration>, decl, index, node->expandedDeclarations)
					{
						if (index > 0)
						{
							writer.WriteLine(L"");
							writer.WriteString(indent);
						}
						WfPrint(decl, indent, writer);
						if (index < node->expandedDeclarations.Count() - 1)
						{
							writer.WriteLine(L"");
						}
					}
				}
				else
				{
					node->Accept(static_cast<WfVirtualDeclaration::IVisitor*>(this));
				}
			}

			void Visit(WfAutoPropertyDeclaration* node)override
			{
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
				if (node->configConst == WfAPConst::Readonly)
				{
					if (node->configObserve == WfAPObserve::NotObservable)
					{
						writer.WriteString(L"const, not observe");
					}
					else
					{
						writer.WriteString(L"const");
					}
				}
				else
				{
					if (node->configObserve == WfAPObserve::NotObservable)
					{
						writer.WriteString(L"not observe");
					}
				}
				writer.WriteString(L"}");
				writer.AfterPrint(node);
			}
		};

/***********************************************************************
Print (Module)
***********************************************************************/

		void WfPrint(Ptr<WfAttribute> node, const WString& indent, parsing::ParsingWriter& writer)
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

		void WfPrint(Ptr<WfType> node, const WString& indent, parsing::ParsingWriter& writer)
		{
			PrintTypeVisitor visitor(indent, writer);
			node->Accept(&visitor);
		}

		void WfPrint(Ptr<WfExpression> node, const WString& indent, parsing::ParsingWriter& writer)
		{
			PrintExpressionVisitor visitor(indent, writer);
			node->Accept(&visitor);
		}

		void WfPrint(Ptr<WfStatement> node, const WString& indent, parsing::ParsingWriter& writer)
		{
			PrintStatementVisitor visitor(indent, writer);
			node->Accept(&visitor);
		}

		void WfPrint(Ptr<WfDeclaration> node, const WString& indent, parsing::ParsingWriter& writer)
		{
			FOREACH(Ptr<WfAttribute>, attribute, node->attributes)
			{
				WfPrint(attribute, indent, writer);
				writer.WriteLine(L"");
				writer.WriteString(indent);
			}

			if (node->classMember)
			{
				switch (node->classMember->kind)
				{
				case WfClassMemberKind::Normal:
					break;
				case WfClassMemberKind::Static:
					writer.WriteString(L"static ");
					break;
				case WfClassMemberKind::Override:
					writer.WriteString(L"override ");
					break;
				}
			}

			PrintDeclarationVisitor visitor(indent, writer);
			node->Accept(&visitor);
		}

		void WfPrint(Ptr<WfModule> node, const WString& indent, parsing::ParsingWriter& writer)
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
			}

			FOREACH(Ptr<WfModuleUsingPath>, path, node->paths)
			{
				writer.WriteString(indent);
				writer.WriteString(L"using ");
				FOREACH_INDEXER(Ptr<WfModuleUsingItem>, item, index, path->items)
				{
					if (index > 0)
					{
						writer.WriteString(L"::");
					}
					FOREACH(Ptr<WfModuleUsingFragment>, fragment, item->fragments)
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

			FOREACH(Ptr<WfDeclaration>, decl, node->declarations)
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
