#pragma once
#include <mutex>
#include <iostream>
#include <memory>
#include "NonCopyable.h"

template <class T>
class SingletonImp: public noncopyable
{
public:
	static std::shared_ptr<T>& getInstance() {
		static std::once_flag initDataFlag;
		std::call_once(initDataFlag, [&] {  std::cout << "construct" << std::endl;  mInstance = std::make_shared<T>(); });
		//std::cout << std::hex << mInstance << std::endl;

		return mInstance;
	}
protected:
	SingletonImp() {};
	~SingletonImp() {
	};
private:
	static std::shared_ptr<T> mInstance;

};
template<class T> std::shared_ptr<T> SingletonImp<T>::mInstance = nullptr;
