#pragma comment(lib, "System.lib")
#pragma comment(lib, "libssl.lib")
#pragma comment(lib, "libcrypto.lib")

#include <iostream>
#include <iomanip>
#include <sstream>

#include <System/String.hpp>

#include <System/Net/IPC/IpcServer.hpp>
#include <System/Net/IPC/IpcClient.hpp>

void InitializeSockets()
{
#ifdef _WIN32

	WORD wHighVersion = 2;
	WORD wVersion = 2;

	WSADATA wsa = { 0 };
	WORD wVersionRequested = MAKEWORD(wHighVersion, wVersion);

	int nResult = ::WSAStartup(wVersionRequested, &wsa);
	if (nResult != 0)
	{
		throw SocketException(nResult);
	}

#endif // _WIN32
}

namespace ipc = System::Net::IPC;

namespace Excalibur
{
	class ExcaliburIpcClient
		: public ipc::IpcClientDispatcher
	{
	public:

	};

	class ExcaliburIpcServer
		: public ipc::IpcServerDispatcher
	{
	public:

	};



	const int port = 65321;

	class ExcaliburClient
		: public ipc::IpcServerDispatcher
	{
	public:
		ExcaliburClient()
		{
			m_ipcServer = std::make_shared<ipc::IpcServer>(port, this);
			m_ipcServer->Start();
		}

		virtual void IpcServer_ClientConnected(const ipc::IpcClientId clientId) override
		{
			std::cout << "Client connected: " << clientId << std::endl;
		}

	private:
		ipc::IpcServer_ptr m_ipcServer;
	};



	class ExcaliburCredentialProviderUI
		: public ipc::IpcClientDispatcher
	{
	public:
		ExcaliburCredentialProviderUI()
		{
			m_ipcClient = std::make_shared<ipc::IpcClient>(port, this);
			m_ipcClient->Start();
		}

		virtual void IpcClient_OnConnected() override
		{
			concurrency::create_task([this]()
				{
					const auto request = m_ipcClient->CreateRequest("Hello world", System::Timeout::Infinite);

					const auto response = m_ipcClient->SendRequest(request);

					int a = 5;
				});
		}

	private:
		ipc::IpcClient_ptr m_ipcClient;
	};
}

using namespace Excalibur;

int main()
{
	InitializeSockets();

	auto excaliburClient = ExcaliburClient();
	auto excaliburCredProviderUI = ExcaliburCredentialProviderUI();

	do
	{
		std::this_thread::sleep_for(std::chrono::seconds(1));

	} while (1);

	return 0;
}
