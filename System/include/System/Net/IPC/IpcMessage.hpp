#ifndef __SYSTEM_NET_IPC_MESSAGE_HPP__
#define __SYSTEM_NET_IPC_MESSAGE_HPP__

#include <System/Timeout.hpp>
#include <System/TimeSpan.hpp>
#include <System/TimeoutException.hpp>

#include <System/Net/IPC/IpcDefines.hpp>

#include <future>

namespace System
{
	namespace Net
	{
		namespace IPC
		{
			class IpcMessage
			{
			public:
				virtual ~IpcMessage()
				{

				}

				virtual void SetResult(const std::exception& ex) = 0;
				virtual bool IsRequest() const = 0;
				virtual bool IsResponse() const = 0;

				bool IsExpired() const
				{
					return m_timeout.GetIsElapsed();
				}

				const std::string& Data() const
				{
					return m_data;
				}

				IpcMessageId Id() const
				{
					return m_id;
				}

			protected:
				IpcMessage(const IpcMessageId id, const std::string& data, const Timeout& timeout)
					: m_id(id)
					, m_data(data)
					, m_timeout(timeout)
				{

				}

			protected:
				IpcMessageId m_id;
				std::string m_data;
				System::Timeout m_timeout;
			};

			typedef std::shared_ptr<IpcMessage> IpcMessage_ptr;
		}
	}
}

#endif // __SYSTEM_NET_IPC_MESSAGE_HPP__
