#pragma once

#include <Windows.h>

namespace System
{
	namespace Timers
	{
		enum class TimerFlags : ULONG
		{
			/// <summary>
			/// By default, the callback function is queued to a non-I/O worker thread.
			/// </summary>
			ExecuteDefault = WT_EXECUTEDEFAULT,

			/// <summary>
			/// The callback function is invoked by the timer thread itself. This flag 
			/// should be used only for short tasks or it could affect other timer operations. 
			/// The callback function is queued as an APC. It should not perform alertable 
			/// wait operations.
			/// </summary>
			ExecuteInTimerThread = WT_EXECUTEINTIMERTHREAD,

			/// <summary>
			/// The callback function is queued to an I/O worker thread. This flag should 
			/// be used if the function should be executed in a thread that waits in an 
			/// alertable state. 
			/// The callback function is queued as an APC. Be sure to address reentrancy 
			/// issues if the function performs an alertable wait operation.
			/// </summary>
			ExecuteInIOThread = WT_EXECUTEINIOTHREAD,

			/// <summary>
			/// The callback function is queued to a thread that never terminates. It 
			/// does not guarantee that the same thread is used each time. This flag 
			/// should be used only for short tasks or it could affect other timer 
			/// operations. currently no worker thread is truly persistent, 
			/// although no worker thread will terminate if there are any pending I/O 
			/// requests.
			/// </summary>
			ExecuteInPersistentThread = WT_EXECUTEINPERSISTENTTHREAD,

			/// <summary>
			/// The callback function can perform a long wait. This flag helps the 
			/// system to decide if it should create a new thread.
			/// </summary>
			ExecuteLongFunction = WT_EXECUTELONGFUNCTION,

			/// <summary>
			/// The timer will be set to the signaled state only once. If this flag 
			/// is set, the Period parameter must be zero.
			/// </summary>
			ExecuteOnlyOnce = WT_EXECUTEONLYONCE,

			/// <summary>
			/// Callback functions will use the current access token, whether it 
			/// is a process or impersonation token. If this flag is not specified, 
			/// callback functions execute only with the process token.
			/// Windows XP/2000:  This flag is not supported until Windows XP with 
			/// SP2 and Windows Server 2003.
			/// </summary>
			TransferImpersonation = WT_TRANSFER_IMPERSONATION
		};
	}
}
