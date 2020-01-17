#ifndef __SYSTEM_NET_IPC_QUEUE_ITEM_HPP__
#define __SYSTEM_NET_IPC_QUEUE_ITEM_HPP__


#include <System/Net/IPC/IpcMessage.hpp>


namespace System
{
	namespace Net
	{
		namespace IPC
		{
			namespace details
			{
				class IpcQueueItem
				{
				public:
					IpcQueueItem(const IpcMessage_ptr message, const System::Timeout& timeout)
						: m_message(message)
						, m_timeout(timeout)
					{

					}

					virtual bool IsRequestItem() const = 0;

					virtual void SetResult(const std::exception& ex) = 0;

					bool IsResponseItem() const
					{
						return !this->IsRequestItem();
					}

					IpcMessage_ptr Message() const
					{
						return m_message;
					}

					System::Timeout Timeout() const
					{
						return m_timeout;
					}

					bool IsExpired() const
					{
						return m_timeout.GetIsElapsed();
					}

				protected:
					IpcMessage_ptr m_message;
					System::Timeout m_timeout;
				};



				class IpcQueueRequestItem final
					: public IpcQueueItem
				{
				public:
					IpcQueueRequestItem(const IpcMessage_ptr request, const System::Timeout& timeout)
						: IpcQueueItem(request, timeout)
					{

					}

					virtual void SetResult(const std::exception& ex)
					{
						const auto pex = std::make_exception_ptr(ex);
						m_message->Id();
						m_promise.set_exception(pex);
					}

					void SetResult(const IpcMessage_ptr response)
					{
						m_promise.set_value(response);
					}

					virtual bool IsRequestItem() const
					{
						return true;
					}

					IpcMessage_ptr Wait()
					{
						auto future = m_promise.get_future();

						return future.get();
					}

				private:
					std::promise<IpcMessage_ptr> m_promise;
				};



				class IpcQueueResponseItem final
					: public IpcQueueItem
				{
				public:
					IpcQueueResponseItem(const IpcMessage_ptr response, const System::Timeout& timeout)
						: IpcQueueItem(response, timeout)
					{

					}

					virtual void SetResult(const std::exception& ex)
					{
						const auto pex = std::make_exception_ptr(ex);

						m_promise.set_exception(pex);
					}

					void SetResult()
					{
						m_promise.set_value();
					}

					virtual bool IsRequestItem() const
					{
						return false;
					}

					void Wait()
					{
						auto future = m_promise.get_future();

						future.get();
					}

				private:
					std::promise<void> m_promise;
				};


				typedef std::shared_ptr<IpcQueueItem> IpcQueueItem_ptr;
				typedef std::shared_ptr<IpcQueueRequestItem> IpcQueueRequestItem_ptr;
				typedef std::shared_ptr<IpcQueueResponseItem> IpcQueueResponseItem_ptr;
			}
		}
	}
}

#endif // __SYSTEM_NET_IPC_QUEUE_ITEM_HPP__
