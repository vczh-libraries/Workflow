#include "ChatBotApp.h"
#include "..\ChatBotServer\Shared\ChatBotJsonDispatcher.h"
#include <VlppOS.Windows.h>
#include <stdio.h>

using namespace vl;
using namespace vl::console;
using namespace vl::glr::json;
using namespace vl::inter_process;
using namespace chatbot;

bool TryReadLine(WString& line)
{
	DWORD mode = 0;
	if (GetConsoleMode(GetStdHandle(STD_INPUT_HANDLE), &mode))
	{
		line = Console::Read();
		return true;
	}

	wchar_t buffer[4096] = { 0 };
	if (!fgetws(buffer, sizeof(buffer) / sizeof(*buffer), stdin))
	{
		return false;
	}

	line = buffer;
	while (line.Length() > 0)
	{
		auto c = line[line.Length() - 1];
		if (c != L'\r' && c != L'\n')
		{
			break;
		}
		line = line.Left(line.Length() - 1);
	}
	return true;
}

class ChatBotTaskThread : public Thread
{
private:
	Ptr<ChatBotJsonDispatcherBase> dispatcher;

protected:
	void Run() override
	{
		dispatcher->RunTaskQueue();
	}

public:
	ChatBotTaskThread(Ptr<ChatBotJsonDispatcherBase> _dispatcher)
		: dispatcher(_dispatcher)
	{
	}
};

class ChatBotInputThread : public Thread
{
private:
	Ptr<ChatBotJsonDispatcherClient> dispatcher;
	Ptr<chatapi::IChatServer> chatServer;
	WString userName;

protected:
	void Run() override
	{
		WString line;
		while (TryReadLine(line))
		{
			if (line == WString::Unmanaged(L"exit"))
			{
				dispatcher->QueueTask(Func<void()>([server = chatServer, name = userName]()
				{
					server->RemoveUser(name);
				}));
				dispatcher->QueueExitTask();
				break;
			}
			else
			{
				dispatcher->QueueTask(Func<void()>([server = chatServer, name = userName, message = line]()
				{
					server->Speak(name, message);
				}));
			}
		}
	}

public:
	ChatBotInputThread(Ptr<ChatBotJsonDispatcherClient> _dispatcher, Ptr<chatapi::IChatServer> _chatServer, const WString& _userName)
		: dispatcher(_dispatcher)
		, chatServer(_chatServer)
		, userName(_userName)
	{
	}
};

int main()
{
	auto parser = CreateChatBotJsonParser();
	auto httpClient = Ptr(new HttpClient(WString::Unmanaged(ChatBotHttpBaseUrl), ChatBotHttpPort));
	auto channelClient = Ptr(new ChatBotChannelClient(Ptr<INetworkProtocolClient>(httpClient), parser));
	auto dispatcher = Ptr(new ChatBotJsonDispatcherClient);
	dispatcher->LoginClient(Ptr<JsonChannelClient>(channelClient));

	auto chatServer = dispatcher->GetChatServer();
	chatServer->OnUserAdded.Add(Func<void(WString)>([](WString speakerName)
	{
		Console::WriteLine(speakerName + WString::Unmanaged(L" joined"));
	}));
	chatServer->OnUserRemoved.Add(Func<void(WString)>([](WString speakerName)
	{
		Console::WriteLine(speakerName + WString::Unmanaged(L" left"));
	}));
	chatServer->OnSpoken.Add(Func<void(WString, WString)>([](WString speakerName, WString message)
	{
		Console::WriteLine(speakerName + WString::Unmanaged(L"> ") + message);
	}));
	chatServer->OnServerShutdown.Add(Func<void()>([dispatcher]()
	{
		dispatcher->QueueExitTask();
	}));

	WString userName;
	if (!TryReadLine(userName))
	{
		return 0;
	}
	auto taskThread = Ptr(new ChatBotTaskThread(Ptr<ChatBotJsonDispatcherBase>(dispatcher)));
	taskThread->Start();
	dispatcher->QueueTask(Func<void()>([server = chatServer, name = userName]()
	{
		server->AddUser(name);
	}));

	auto inputThread = Ptr(new ChatBotInputThread(dispatcher, chatServer, userName));
	inputThread->Start();
	taskThread->Wait();
	dispatcher->FinalizeRpc();
	httpClient->Stop();
	return 0;
}
