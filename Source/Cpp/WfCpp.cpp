#include "WfCpp.h"

namespace vl
{
	namespace workflow
	{
		namespace cppcodegen
		{
			using namespace collections;
			using namespace analyzer;

/***********************************************************************
WfCppConfig
***********************************************************************/

			void WfCppConfig::Collect()
			{
				FOREACH(Ptr<WfModule>, module, manager->GetModules())
				{
					FOREACH(Ptr<WfDeclaration>, decl, module->declarations)
					{
						CollectDeclaration(this, decl);
					}
				}
			}

			WfCppConfig::WfCppConfig(analyzer::WfLexicalScopeManager* _manager)
				:manager(_manager)
			{
				Collect();
			}

			WfCppConfig::~WfCppConfig()
			{
			}

			void WfCppConfig::WriteEnd(stream::StreamWriter& writer, collections::List<WString>& nss)
			{
			}

/***********************************************************************
WfCppConfig::WriteHeader
***********************************************************************/

			void WfCppConfig::WriteHeader_Global(stream::StreamWriter& writer)
			{
			}

			void WfCppConfig::WriteHeader_Enum(stream::StreamWriter& writer, Ptr<WfEnumDeclaration> decl, collections::List<WString>& nss)
			{
			}

			void WfCppConfig::WriteHeader_Struct(stream::StreamWriter& writer, Ptr<WfStructDeclaration> decl, collections::List<WString>& nss)
			{
			}

			void WfCppConfig::WriteHeader_ClassPreDecls(stream::StreamWriter& writer, collections::List<WString>& nss)
			{
			}

			void WfCppConfig::WriteHeader_Class(stream::StreamWriter& writer, Ptr<WfClassDeclaration> decl, collections::List<WString>& nss)
			{
			}

/***********************************************************************
WfCppConfig::WriteCpp
***********************************************************************/

			void WfCppConfig::WriteCpp_Global(stream::StreamWriter& writer)
			{
			}

			void WfCppConfig::WriteCpp_LambdaExprDecl(stream::StreamWriter& writer, Ptr<WfExpression> lambda, collections::List<WString>& nss)
			{
			}

			void WfCppConfig::WriteCpp_LambdaExprImpl(stream::StreamWriter& writer, Ptr<WfExpression> lambda, collections::List<WString>& nss)
			{
			}

			void WfCppConfig::WriteCpp_ClassExprDecl(stream::StreamWriter& writer, Ptr<WfNewInterfaceExpression> lambda, collections::List<WString>& nss)
			{
			}

			void WfCppConfig::WriteCpp_ClassExprImpl(stream::StreamWriter& writer, Ptr<WfNewInterfaceExpression> lambda, collections::List<WString>& nss)
			{
			}

			void WfCppConfig::GetClassMembers(Ptr<WfClassDeclaration> decl, collections::List<Ptr<WfClassMember>>& members)
			{
			}

			void WfCppConfig::WriteCpp_ClassMember(stream::StreamWriter& writer, Ptr<WfClassDeclaration> decl, Ptr<WfClassMember> member, collections::List<WString>& nss)
			{
			}
		}
	}
}