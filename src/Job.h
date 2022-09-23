#pragma once
#include <list>
#include <atomic>
#include "FunctionObj.h"

using namespace std;

struct Job
{
	Job() :
		func(),
		dependece(0)
	{}

	template<class U>
	Job(U&& _func) :
		func(std::forward<U>(_func)),
		dependece(0)
	{}

	Job(Job&& job) :
		func(std::move(job.func)),
		children(std::move(job.children)),
		dependece(job.dependece.load())
	{
	}

	FunctionObj<void()> func;
	std::list<int> children;
	atomic<int> dependece;
};