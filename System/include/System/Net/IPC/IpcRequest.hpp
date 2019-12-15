#ifndef __IPC_REQUEST_HPP__
#define __IPC_REQUEST_HPP__

#include <System/Net/IPC/IpcMessage.hpp>

namespace System
{
	namespace Net
	{
		namespace IPC
		{
			class IpcRequest final
				: public IpcMessage
			{
			public:
				IpcRequest(const std::uint64_t id, const json11::Json& data, const TimeSpan& timeout)
					: IpcMessage(id, data, timeout)
				{

				}

				void SetResult(const json11::Json& result)
				{
					m_promise.set_value(result);
				}

				json11::Json Wait()
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
					return true;
				}

				virtual bool IsResponse() const override
				{
					return false;
				}

			private:
				std::promise<json11::Json> m_promise;
			};
		}
	}
}

#endif // __IPC_REQUEST_HPP__
