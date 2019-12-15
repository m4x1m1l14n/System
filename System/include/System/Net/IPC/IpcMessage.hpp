#ifndef __IPC_MESSAGE_HPP__
#define __IPC_MESSAGE_HPP__

#include <System/Timeout.hpp>
#include <System/TimeSpan.hpp>
#include <System/TimeoutException.hpp>

#include <future>

// TODO Remove JSON dependency, use std::string for payload

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

				const json11::Json& Data() const
				{
					return m_data;
				}

				std::uint64_t Id() const
				{
					return m_id;
				}

			protected:
				IpcMessage(const std::uint64_t id, const json11::Json& data, const TimeSpan& timeout)
					: m_id(id)
					, m_data(data)
					, m_timeout((timeout == TimeSpan::MaxValue()) ? Timeout::CreateInfinite() : Timeout::ElapseAfter(timeout))
				{

				}

			protected:
				std::uint64_t m_id;
				json11::Json m_data;
				System::Timeout m_timeout;
			};
		}
	}
}

#endif // __IPC_MESSAGE_HPP__
