#pragma once
#include <thread>
#include <mutex>
#include <atomic>
#include <condition_variable>
#include <functional>
#include <vector>
#include <queue>
#include <iostream>

#include "FunctionObj.h"
#include "Job.h"

class ThreadPool
{
public:

	explicit ThreadPool(int num) : _thread_num(num), _is_running(false) {}

	~ThreadPool() {
		if (_is_running)
			stop();
	}

	void start() {
		_is_running = true;

		//开启线程
		for (int i = 0; i < _thread_num; ++i)
			_threads.emplace_back(std::thread(&ThreadPool::work, this));
	}

	void stop() {
		{
			//因为有的线程可能处于阻塞状态,所以在回收线程之前保证所有线程均处于运行状态
			std::unique_lock<std::mutex> lk(_mtx);
			_is_running = false;
			_cond.notify_all();
		}

		for (auto& thread : _threads) {
			if (thread.joinable())
				thread.join();
		}
	}

	//往任务队列添加任务
	void appendTask(Job* job) {
		if (_is_running) {
			std::unique_lock<std::mutex> lk(_mtx);
			_tasks.emplace(job);
			_cond.notify_one();
		}
	}

	template<class U>
	void setFinishFunc(U&& func) {
		_finishedOneJob = std::forward<U>(func);
	}

private:
	//工作接口
	void work() {
		while (_is_running) {
			Job* job = nullptr;
			{
				std::unique_lock<std::mutex> lk(_mtx);
				if (!_tasks.empty())
				{
					// if tasks not empty,
					// must finish the task whether thread pool is running or not
					job = _tasks.front();
					_tasks.pop(); // remove the task
				}
				else if (_is_running && _tasks.empty())
					_cond.wait(lk);
			}

			if (job && job->func)
				job->func(); // do the task

			if (_finishedOneJob)
				_finishedOneJob(job);
		}
	}
public:
	// disable copy and assign construct
	ThreadPool(const ThreadPool&) = delete;

	ThreadPool& operator=(const ThreadPool& other) = delete;

private:
	std::mutex _mtx;
	std::atomic_bool _is_running;
	std::condition_variable _cond;

	int _thread_num;
	std::vector<std::thread> _threads;
	std::queue<Job*> _tasks;

	// 用于调用上层
	FunctionObj<void(Job*)> _finishedOneJob;
};