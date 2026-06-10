#include "ChatBotApp.h"
#include "Shared/ChatBotJsonDispatcher.h"
#include <VlppOS.Windows.h>
#include <stdio.h>

using namespace vl;
using namespace vl::collections;
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

class ChatServerService : public Object, public virtual chatapi::IChatServer
{
private:
	SortedList<WString> users;

public:
	bool AddUser(const WString& name) override
	{
		if (users.Contains(name))
		{
			return false;
		}

		users.Add(name);
		OnUserAdded(name);
		return true;
	}

	bool RemoveUser(const WString& name) override
	{
		if (!users.Contains(name))
		{
			return false;
		}

		users.Remove(name);
		OnUserRemoved(name);
		return true;
	}

	void Speak(const WString& speakerName, const WString& message) override
	{
		if (users.Contains(speakerName))
		{
			OnSpoken(speakerName, message);
		}
	}
};

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

class ChatBotHttpChannelServer : public ChatBotChannelServer, public HttpServer
{
public:
	ChatBotHttpChannelServer(Ptr<Parser> parser)
		: ChatBotChannelServer(parser)
		, HttpServer(WString::Unmanaged(ChatBotHttpBaseUrl), ChatBotHttpPort)
	{
	}

	WaitForClientResult OnClientConnected(INetworkProtocolConnection* connection) override
	{
		return JsonNetworkChannelServer::OnClientConnected(connection);
	}

	void Start() override
	{
		ChatBotChannelServer::Start();
		HttpServer::Start();
	}

	void Stop() override
	{
		HttpServer::Stop();
		ChatBotChannelServer::Stop();
	}

	bool IsStopped() override
	{
		return HttpServer::IsStopped() || ChatBotChannelServer::IsStopped();
	}
};

int main()
{
	auto parser = CreateChatBotJsonParser();
	auto channelServer = Ptr(new ChatBotHttpChannelServer(parser));
	auto service = Ptr(new ChatServerService);
	service->OnUserAdded.Add(Func<void(WString)>([](WString speakerName)
	{
		Console::WriteLine(speakerName + WString::Unmanaged(L" joined"));
	}));
	service->OnUserRemoved.Add(Func<void(WString)>([](WString speakerName)
	{
		Console::WriteLine(speakerName + WString::Unmanaged(L" left"));
	}));
	service->OnSpoken.Add(Func<void(WString, WString)>([](WString speakerName, WString message)
	{
		Console::WriteLine(speakerName + WString::Unmanaged(L"> ") + message);
	}));

	auto dispatcher = Ptr(new ChatBotJsonDispatcherServer(Ptr<JsonChannelServer>(channelServer), Ptr<chatapi::IChatServer>(service)));
	channelServer->SetDispatcher(dispatcher.Obj());
	dispatcher->Start();

	auto taskThread = Ptr(new ChatBotTaskThread(Ptr<ChatBotJsonDispatcherBase>(dispatcher)));
	taskThread->Start();

	Console::SetTitle(L"ChatBotServer (localhost:" + itow(ChatBotHttpPort) + ChatBotHttpBaseUrl + L")");
	Console::WriteLine(L"Ready to start ChatBotClient.");
	WString line;
	while (TryReadLine(line) && line != WString::Unmanaged(L"exit"))
	{
	}

	dispatcher->QueueExitTask();
	taskThread->Wait();
	service->OnServerShutdown();
	dispatcher->FinalizeRpc();
	channelServer->Stop();
	return 0;
}
