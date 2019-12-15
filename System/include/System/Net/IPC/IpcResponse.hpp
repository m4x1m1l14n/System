#ifndef __IPC_RESPONSE_HPP__
#define __IPC_RESPONSE_HPP__

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
				IpcResponse(const std::uint64_t id, const json11::Json& data, const TimeSpan& timeout)
					: IpcMessage(id, data, timeout)
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
		}
	}
}

#endif // __IPC_RESPONSE_HPP__
