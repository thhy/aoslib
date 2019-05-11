#pragma once
#include "SingletonImp.h"
#include <thread>
#include <mutex>
#include <future>
#include <queue>
#include <functional>
#include <condition_variable>
#include <atomic>
#include <vector>

//namespace base {
	using task = std::function<void()>;
	template<int n>
	class ThreadPoll
	{
	public:
		ThreadPoll() :threadNum_(n), stop_(false) {
			std::cout << "init " << n << " thread" << std::endl;
			for (int i = 0; i < threadNum_; ++i) {
				workers_.emplace_back([this, i]() {
					while (!stop_.load(std::memory_order_acquire)) {
						task tk;
						{
							std::unique_lock<std::mutex> lg(lock_);
							condition_.wait(lg, [this] {return stop_.load(std::memory_order_acquire) || !tasks_.empty(); });
							if (stop_.load(std::memory_order_acquire))
								return;
							tk = move(this->tasks_.front());
							this->tasks_.pop();
							std::cout << "thread:" << i << " get task" << std::endl;
						}
						tk();
					}
				});
			}
		};

		void stop() {
			stop_.store(true, std::memory_order_release);
		}


		//template < class Function, class... Args>
		//std::future < typename std::result_of<Function(Args...)>::type>
		//	add(Function&& fcn, Args&&... args) {

		//}

		template<class Func, class... Args> 
		std::future<typename std::result_of<Func(Args...)>::type>
			add(Func &&func, Args&&...args)
		{
			using returnType = typename std::result_of<Func(Args...)>::type ;
			using taskType = std::packaged_task<returnType()>;
			
			auto taskFunc = std::make_shared<taskType>(std::bind(std::forward<Func>(func), std::forward<Args>(args)...));

			auto ret = taskFunc->get_future();
			{
				std::lock_guard<std::mutex> lg(this->lock_);
				if (this->stop_.load(std::memory_order_acquire))
					throw std::runtime_error("add stop thread pool");
				this->tasks_.emplace([taskFunc]() {
					(*taskFunc)();
				});
			}
			this->condition_.notify_one();
			return ret;
		}

		~ThreadPoll() {
			stop();
			this->condition_.notify_all();
			for (auto &&t : workers_) {
				if (t.joinable())
					t.join();
			}

		};

	private:
		int threadNum_;
		std::atomic<bool> stop_;
		std::vector<std::thread> workers_;
		std::queue<task> tasks_;
		std::condition_variable condition_;
		std::mutex lock_;
	};

//};
