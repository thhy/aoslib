#pragma once
#include <atomic>
#include <memory>
#include <iostream>

using namespace std;
class ThreadSafeBase
{
public:
	ThreadSafeBase() :ref(0) {};
	~ThreadSafeBase() {};
	void addRef() {
		ref++;
	}
	bool release()
	{
		this->ref--;
		return ref == 0;
	}

private:
	volatile atomic<int> ref;
};

template<typename T>
class ThreadSafe:public ThreadSafeBase
{
public:
	ThreadSafe();
	~ThreadSafe() noexcept;
	bool release() {
		if (ThreadSafeBase::release()) {
			if (mMem) {
				cout << "release mMem" << endl;
				delete mMem;
			}
			return true;
		}
		return false;
	}
private:
	T *mMem;
};

template<typename T> ThreadSafe<T>::ThreadSafe()
{
	mMem = new T;
};

template<typename T> ThreadSafe<T>::~ThreadSafe() noexcept
{
	release();
};
