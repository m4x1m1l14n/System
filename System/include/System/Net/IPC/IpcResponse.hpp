#ifndef __SYSTEM_NET_IPC_RESPONSE_HPP__
#define __SYSTEM_NET_IPC_RESPONSE_HPP__


#include <System/Net/IPC/IpcMessage.hpp>


namespace System
{
	namespace Net
	{
		namespace IPC
		{
			class IpcResponse
				: public IpcMessage
			{
			public:
				IpcResponse(const IpcMessageId id, const std::string& data)
					: IpcMessage(id, data)
				{

				}

				void SetResult()
				{
					m_promise.set_value();
				}

				void Wait()
				{
					auto future = m_promise.get_future();

					return future.get();
				}

				virtual void SetResult(const std::exception& ex) override
				{
					m_promise.set_exception(std::make_exception_ptr(ex));
				}

				virtual bool IsRequest() const override
				{
					return false;
				}

				virtual bool IsResponse() const override
				{
					return true;
				}

			private:
				std::promise<void> m_promise;
			};


			typedef std::shared_ptr<IpcResponse> IpcResponse_ptr;
		}
	}
}

#endif // __SYSTEM_NET_IPC_RESPONSE_HPP__
