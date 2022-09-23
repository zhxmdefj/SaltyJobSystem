#pragma once

#include <vector>
#include <atomic>
#include "Job.h"
#include "ThreadPool.h"

using namespace std;

const int INVAILD_JOBID = -1;

class JobSystem {
public:
	// 应根据 CPU 核心数（num）来创建线程池
	JobSystem(int num) :
		mThreadPool(num),
		mCurrentJobsNum(0)
	{
		mThreadPool.setFinishFunc([this](Job* job) {this->FinishedOneJob(job); });
		mThreadPool.start();
	}

	~JobSystem() {
		mThreadPool.stop();
	}

	// 每帧都要初始化
	void Init()
	{
		mJobs.clear();
		mCurrentJobsNum.store(0);
		cout << "本帧的 Jobsystem 已初始化完成" << endl;
	}

	// 提交 Job（但不先开干）
	template <class U>
	int SumbitJob(U&& func, std::list<int>&& parents = {})
	{
		int id = mJobs.size();
		mJobs.emplace_back(std::forward<U>(func));
		for (int parent : parents)
		{
			mJobs[parent].children.emplace_back(id);
			// 当前任务依赖数+1
			mJobs[id].dependece++;
		}
		return id;
	}

	// 同时触发开干所有 Job
	void StartJobs()
	{
		cout << "本帧的 Jobsystem 已开始同时触发" << endl;
		mCurrentJobsNum.store(mJobs.size());
		for (auto& job : mJobs)
		{
			if (job.dependece == 0)
				mThreadPool.appendTask(&job);
		}
	}

	// 等待所有 Job 完成
	void WaitJobs()
	{
		std::unique_lock<std::mutex> lck(mMtx);
		mWait.wait(lck, [this]() {return mCurrentJobsNum == 0; });
		cout << "本帧的 Jobsystem 已完成所有 Job" << endl << endl;
	}
private:
	void FinishedOneJob(Job* job)
	{
		if (!job)return;
		for (auto child : job->children)
		{
			// 对每个 child Job 的依赖数减去一
			if (mJobs[child].dependece.fetch_sub(1) == 1)
			{
				mThreadPool.appendTask(&mJobs[child]);
			}
		}

		mCurrentJobsNum.fetch_sub(1);
		mWait.notify_one();
	}
private:
	// 线程池
	ThreadPool mThreadPool;
	// Jobs
	std::vector<Job> mJobs;
	// 当前剩余 Job 数量
	atomic<int> mCurrentJobsNum;

	std::mutex mMtx;
	condition_variable mWait;
};