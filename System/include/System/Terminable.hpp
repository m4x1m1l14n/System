#pragma once

#include <System/Threading/ManualResetEvent.hpp>

class Terminable
{

private:
	System::Threading::ManualResetEvent_ptr m_terminateEvent;
};
