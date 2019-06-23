#pragma once
#include <mutex>
#include <iostream>
#include <memory>
#include "NonCopyable.h"

template <class T>
class SingletonImp: public noncopyable
{
public:
	static T * getInstance() {
		static T *mInstance = new T;
		return mInstance;
	}
protected:
	SingletonImp() {};
	~SingletonImp() {
	};

};
