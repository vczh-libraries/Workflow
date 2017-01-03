#include "WfCpp.h"

namespace vl
{
	namespace workflow
	{
		namespace cppcodegen
		{
			using namespace collections;
			using namespace analyzer;

			class WfGenerateExpressionVisitor : public Object, public WfExpression::IVisitor
			{
			public:
				WfCppConfig*				config;
				stream::StreamWriter&		writer;
				WString						prefix;

				WfGenerateExpressionVisitor(WfCppConfig* _config, stream::StreamWriter& _writer, const WString& _prefix)
					:config(_config)
					, writer(_writer)
					, prefix(_prefix)
				{
				}

				void Visit(WfThisExpression* node)override
				{
					throw 0;
				}

				void Visit(WfTopQualifiedExpression* node)override
				{
					throw 0;
				}

				void Visit(WfReferenceExpression* node)override
				{
					throw 0;
				}

				void Visit(WfOrderedNameExpression* node)override
				{
					throw 0;
				}

				void Visit(WfOrderedLambdaExpression* node)override
				{
					throw 0;
				}

				void Visit(WfMemberExpression* node)override
				{
					throw 0;
				}

				void Visit(WfChildExpression* node)override
				{
					throw 0;
				}

				void Visit(WfLiteralExpression* node)override
				{
					throw 0;
				}

				void Visit(WfFloatingExpression* node)override
				{
					throw 0;
				}

				void Visit(WfIntegerExpression* node)override
				{
					throw 0;
				}

				void Visit(WfStringExpression* node)override
				{
					throw 0;
				}

				void Visit(WfFormatExpression* node)override
				{
					throw 0;
				}

				void Visit(WfUnaryExpression* node)override
				{
					throw 0;
				}

				void Visit(WfBinaryExpression* node)override
				{
					throw 0;
				}

				void Visit(WfLetExpression* node)override
				{
					throw 0;
				}

				void Visit(WfIfExpression* node)override
				{
					throw 0;
				}

				void Visit(WfRangeExpression* node)override
				{
					throw 0;
				}

				void Visit(WfSetTestingExpression* node)override
				{
					throw 0;
				}

				void Visit(WfConstructorExpression* node)override
				{
					throw 0;
				}

				void Visit(WfInferExpression* node)override
				{
					throw 0;
				}

				void Visit(WfTypeCastingExpression* node)override
				{
					throw 0;
				}

				void Visit(WfTypeTestingExpression* node)override
				{
					throw 0;
				}

				void Visit(WfTypeOfTypeExpression* node)override
				{
					throw 0;
				}

				void Visit(WfTypeOfExpressionExpression* node)override
				{
					throw 0;
				}

				void Visit(WfAttachEventExpression* node)override
				{
					throw 0;
				}

				void Visit(WfDetachEventExpression* node)override
				{
					throw 0;
				}

				void Visit(WfBindExpression* node)override
				{
					throw 0;
				}

				void Visit(WfObserveExpression* node)override
				{
					throw 0;
				}

				void Visit(WfCallExpression* node)override
				{
					throw 0;
				}

				void Visit(WfFunctionExpression* node)override
				{
					throw 0;
				}

				void Visit(WfNewClassExpression* node)override
				{
					throw 0;
				}

				void Visit(WfNewInterfaceExpression* node)override
				{
					throw 0;
				}
			};

			void GenerateExpression(WfCppConfig* config, stream::StreamWriter& writer, Ptr<WfExpression> node, const WString& prefix)
			{
				WfGenerateExpressionVisitor visitor(config, writer, prefix);
				node->Accept(&visitor);
			}
		}
	}
}