#include <iostream>
#include <string>
#include <algorithm>
#include <bitset>
#include <vector>
#include <set>
#include <thread>
#include <mutex>
#include <atomic>
#include "ThreadPoll.h"
#include "ThreadSafe.h"

using namespace std;

int func(int n) {
	std::cout << "n:" << n << endl;
	return n;
}

int main() {

	shared_ptr<ThreadPoll<4>> threadPool = move(SingletonImp<ThreadPoll<4>>::getInstance());
	for (int i = 0; i < 10; ++i) {
		//shared_ptr<ThreadPoll<4>> threadPool = move(SingletonImp<ThreadPoll<4>>::getInstance());
		cout << hex << SingletonImp<ThreadPoll<4>>::getInstance().use_count() << endl;
	}
	//cout << hex << threadPoll << endl;
	for (int i = 0; i < 10; ++i) {
		threadPool->add(func, i);
	}
	system("pause");
}