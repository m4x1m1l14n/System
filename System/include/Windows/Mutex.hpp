#pragma once

#include <windows.h>

class Mutex final
{
private:
	SRWLOCK m_lock;

public:
	Mutex();
	~Mutex();

	Mutex(const Mutex& obj) = delete;
	Mutex operator=(const Mutex& obj) = delete;

	void Lock();
	void Unlock();
};

class LockGuard final
{
public:
	LockGuard(Mutex& mtx);
	~LockGuard();

	LockGuard(const LockGuard& obj) = delete;
	LockGuard operator=(const LockGuard& obj) = delete;

	Mutex& m_mtx;
};
