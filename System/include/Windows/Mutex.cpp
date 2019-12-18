#include <Windows/Mutex.hpp>

Mutex::Mutex()
{
	InitializeSRWLock(&m_lock);
}

Mutex::~Mutex()
{
	
}

void Mutex::Lock()
{
	AcquireSRWLockExclusive(&m_lock);
}

void Mutex::Unlock()
{
	ReleaseSRWLockExclusive(&m_lock);
}

LockGuard::LockGuard(Mutex& mtx)
	: m_mtx(mtx)
{
	m_mtx.Lock();
}

LockGuard::~LockGuard()
{
	m_mtx.Unlock();
}
