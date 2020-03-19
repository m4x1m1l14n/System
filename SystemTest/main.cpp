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

using namespace System;
namespace ipc = System::Net::IPC;

const int port = 65321;

static void encryptDecryptPayload(std::string& payload)
{
	for (auto iter = payload.begin(); iter != payload.end(); ++iter)
	{
		*iter ^= 'K';
	}
}

class IpcServerTest
	: public ipc::IpcServerDispatcher
{
public:
	IpcServerTest()
	{
		m_ipcServer = std::make_shared<ipc::IpcServer>(port, this);
		m_ipcServer->Start();
	}

	virtual void IpcServer_ClientConnected(const ipc::IpcClientId clientId) override
	{
		std::cout << "Client connected: " << clientId << std::endl;
	}

	virtual void IpcServer_OnMessage(const ipc::IpcClientId clientId, const ipc::IpcMessage_ptr message) override
	{
		std::cout << "Client " << clientId << " said " << message->Payload() << std::endl;

		auto ignore = concurrency::create_task([this, clientId, message]()
		{
			const auto response = m_ipcServer->CreateResponse(message, message->Payload());

			m_ipcServer->SendResponse(clientId, response);
		});
	}

	virtual void IpcServer_EncryptPayload(std::string& payload) override
	{
		//std:: cout << "Server encrypting: " << payload << std::endl;

		encryptDecryptPayload(payload);
	}

	virtual void IpcServer_DecryptPayload(std::string& payload) override
	{
		//std:: cout << "Server decrypting: " << payload << std::endl;

		encryptDecryptPayload(payload);
	}

private:
	ipc::IpcServer_ptr m_ipcServer;
};



class IpcClientTest
	: public ipc::IpcClientDispatcher
{
public:
	IpcClientTest()
	{
		m_ipcClient = std::make_shared<ipc::IpcClient>(port, this);
		m_ipcClient->Start();
	}

	virtual void IpcClient_EncryptPayload(std::string& payload) override
	{
		//std:: cout << "Client encrypting: " << payload << std::endl;

		encryptDecryptPayload(payload);
	}

	virtual void IpcClient_DecryptPayload(std::string& payload) override
	{
		//std:: cout << "Client decrypting: " << payload << std::endl;

		encryptDecryptPayload(payload);
	}

	virtual void IpcClient_OnConnected() override
	{
		concurrency::create_task([this]()
		{
			const auto request = m_ipcClient->CreateRequest("Hello world");

			auto timeout = Timeout::ElapseAfter(TimeSpan::FromSeconds(5));

			try
			{
				const auto response = m_ipcClient->SendRequest(request, timeout);

				std::cout << "Server responded: " << response->Payload() << std::endl;
			}
			catch (const std::exception & ex)
			{
				std::cout << ex.what() << std::endl;
			}
		});
	}

	virtual void IpcClient_OnError(const std::exception& ex) override
	{
		std::cout << "Client error: " << ex.what() << std::endl;
	}

private:
	ipc::IpcClient_ptr m_ipcClient;
};


int main()
{
	InitializeSockets();

	auto ipcServerTest = IpcServerTest();
	auto ipcClientTest1 = IpcClientTest();
	auto ipcClientTest2 = IpcClientTest();
	auto ipcClientTest3 = IpcClientTest();
	auto ipcClientTest4 = IpcClientTest();

	do
	{
		std::this_thread::sleep_for(std::chrono::seconds(1));

	} while (1);

	return 0;
}
