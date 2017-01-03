#include "WfCpp.h"

namespace vl
{
	namespace workflow
	{
		namespace cppcodegen
		{
			using namespace collections;
			using namespace reflection;
			using namespace reflection::description;
			using namespace analyzer;

			class WfGenerateStatementVisitor : public Object, public WfStatement::IVisitor
			{
			public:
				WfCppConfig*				config;
				stream::StreamWriter&		writer;
				WString						prefix;

				WfGenerateStatementVisitor(WfCppConfig* _config, stream::StreamWriter& _writer, const WString& _prefix)
					:config(_config)
					, writer(_writer)
					, prefix(_prefix)
				{
				}

				void Visit(WfBreakStatement* node)override
				{
					throw 0;
				}

				void Visit(WfContinueStatement* node)override
				{
					throw 0;
				}

				void Visit(WfReturnStatement* node)override
				{
					throw 0;
				}

				void Visit(WfDeleteStatement* node)override
				{
					throw 0;
				}

				void Visit(WfRaiseExceptionStatement* node)override
				{
					throw 0;
				}

				void Visit(WfIfStatement* node)override
				{
					throw 0;
				}

				void Visit(WfSwitchStatement* node)override
				{
					throw 0;
				}

				void Visit(WfWhileStatement* node)override
				{
					throw 0;
				}

				void Visit(WfForEachStatement* node)override
				{
					throw 0;
				}

				void Visit(WfTryStatement* node)override
				{
					throw 0;
				}

				void Visit(WfBlockStatement* node)override
				{
					throw 0;
				}

				void Visit(WfExpressionStatement* node)override
				{
					throw 0;
				}

				void Visit(WfVariableStatement* node)override
				{
					throw 0;
				}
			};

			void GenerateStatement(WfCppConfig* config, stream::StreamWriter& writer, Ptr<WfStatement> node, const WString& prefix)
			{
				WfGenerateStatementVisitor visitor(config, writer, prefix);
				node->Accept(&visitor);
			}
		}
	}
}