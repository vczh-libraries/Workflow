#include "ChatBotHttp.h"

namespace chatbot
{
	using namespace vl;
	using namespace vl::console;
	using namespace vl::glr::json;

	vint mainThreadId = -1;

	Ptr<Parser> CreateChatBotJsonParser()
	{
		return Ptr(new Parser);
	}

	void AttachChatServerEventHandlers(chatapi::IChatServer* chatServer)
	{
#define ERROR_MESSAGE_PREFIX L"chatbot::AttachChatServerEventHandlers(chatapi::IChatServer*)#"
		CHECK_ERROR(chatServer, ERROR_MESSAGE_PREFIX L"Argument chatServer is null.");
		chatServer->OnUserAdded.Add(Func<void(WString)>([](WString speakerName)
		{
			CHECK_ERROR(mainThreadId == Thread::GetCurrentThreadId(), ERROR_MESSAGE_PREFIX L"OnUserAdded should be invoked on the main thread.");
			Console::WriteLine(speakerName + WString::Unmanaged(L" joined"));
		}));
		chatServer->OnUserRemoved.Add(Func<void(WString)>([](WString speakerName)
		{
			CHECK_ERROR(mainThreadId == Thread::GetCurrentThreadId(), ERROR_MESSAGE_PREFIX L"OnUserRemoved should be invoked on the main thread.");
			Console::WriteLine(speakerName + WString::Unmanaged(L" left"));
		}));
		chatServer->OnSpoken.Add(Func<void(WString, WString)>([](WString speakerName, WString message)
		{
			CHECK_ERROR(mainThreadId == Thread::GetCurrentThreadId(), ERROR_MESSAGE_PREFIX L"OnSpoken should be invoked on the main thread.");
			Console::WriteLine(speakerName + WString::Unmanaged(L"> ") + message);
		}));
#undef ERROR_MESSAGE_PREFIX
	}
}
