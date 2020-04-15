#include <System/Net/Http/HttpClient.hpp>

#include <System/Net/Sockets/TlsSocket.hpp>

#include <assert.h>

namespace net = System::Net;
namespace sockets = System::Net::Sockets;

namespace System
{
	namespace Net
	{
		namespace Http
		{
			namespace details
			{
				static std::shared_ptr<SSL_CTX> CreateTlsContext()
				{
					auto ctx = SSL_CTX_new(TLSv1_2_client_method());
					if (ctx == nullptr)
					{
						throw std::runtime_error("SSL_CTX_new() returned nullptr"); // TODO Add OpenSSL error
					}

					auto result = SSL_CTX_set_options(ctx, SSL_OP_NO_SSLv2);
					// TODO Check result and throw on error

					return std::shared_ptr<SSL_CTX>(ctx, [=](SSL_CTX* ctx)
					{
						SSL_CTX_free(ctx);
					});
				}
			}

			HttpClient::HttpClient()
			{

			}

			HttpResponse HttpClient::Post(const System::Net::Url & url, const HttpContent& content) const
			{
				// TODO How to check if url is secure or not?

				std::shared_ptr<sockets::ISocket> socket = (url.getScheme() == "https")
					? std::make_shared<sockets::TlsSocket>(details::CreateTlsContext())
					: std::make_shared<sockets::Socket>();

				socket->Connect(url.getHost(), url.getPort());
	
				HttpRequest request(HttpMethod::Post, url.Path());
		
				request.Headers()
					.Add("Host", url.getHost())
					.Add("Content-Type", "application/json")
					.Add("Accept", "application/json");

				const auto& requestData = request.ToString();

				socket->Write(requestData);

				std::string responseData;

				size_t pos = 0;
				size_t offset = 0;

				do
				{
					const auto& data = socket->Read();

					// TODO Check when TlsSocket::Read can return empty data!
					assert(!data.empty());

					// It is enough to start search 4 characters at end of previous data block received
					if (responseData.length() > HttpHeaders::HeadersTerminator.length())
					{
						offset = responseData.length() - HttpHeaders::HeadersTerminator.length();
					}

					responseData.append(data);

				} while ((pos = responseData.find(HttpHeaders::HeadersTerminator, offset)) == std::string::npos);

				return HttpResponse(responseData, socket);
			}
		}
	}
}
