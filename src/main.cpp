#include <iostream>
#include <list>
#include <vector>
#include <queue>
#include <thread>
#include <string>
#include <future>
#include <condition_variable>
#include <mutex>
#include <Windows.h>
#include "JobSystem.h"

using namespace std;

// 额外用于控制台输出的互斥锁
std::mutex printMtx;

void func1() {
	Sleep(1000);
	printMtx.lock();
	cout << "【JOB1】Update Translation" << endl;
	std::cout << "work thread:[" << std::this_thread::get_id() << "]" << std::endl;
	printMtx.unlock();
}
void func2() {
	Sleep(1000);
	printMtx.lock();
	cout << "【JOB2】Update Rotation" << endl;
	std::cout << "work thread:[" << std::this_thread::get_id() << "]" << std::endl;
	printMtx.unlock();
}
void func3() {
	Sleep(1000);
	printMtx.lock();
	cout << "【JOB3】Update Scale" << endl;
	std::cout << "work thread:[" << std::this_thread::get_id() << "]" << std::endl;
	printMtx.unlock();
}
void func4() {
	Sleep(100);
	printMtx.lock();
	cout << "【JOB4】Transform \t 依赖 【JOB1】 【JOB2】 【JOB3】" << endl;
	std::cout << "work thread:[" << std::this_thread::get_id() << "]" << std::endl;
	printMtx.unlock();
}
void func5() {
	Sleep(100);
	printMtx.lock();
	cout << "【JOB5】Physic \t \t 依赖 【JOB4】" << endl;
	std::cout << "work thread:[" << std::this_thread::get_id() << "]" << std::endl;
	printMtx.unlock();
}
void func6() {
	Sleep(100);
	printMtx.lock();
	cout << "【JOB6】Render \t \t 依赖 【JOB4】 【JOB5】" << endl;
	std::cout << "work thread:[" << std::this_thread::get_id() << "]" << std::endl;
	printMtx.unlock();
}
void func7() {
	Sleep(100);
	printMtx.lock();
	cout << "【JOB7】Particle \t 依赖 【JOB4】" << endl;
	std::cout << "work thread:[" << std::this_thread::get_id() << "]" << std::endl;
	printMtx.unlock();
}
void func8() {
	Sleep(100);
	printMtx.lock();
	cout << "【JOB8】Post \t \t 依赖 【JOB4】" << endl;
	std::cout << "work thread:[" << std::this_thread::get_id() << "]" << std::endl;
	printMtx.unlock();
}

int main()
{
	JobSystem s(4);
	s.Init();

	int job1 = s.SumbitJob(func1);
	int job2 = s.SumbitJob(func2);
	int job3 = s.SumbitJob(func3);
	int job4 = s.SumbitJob(func4, { job1,job2,job3 });
	int job5 = s.SumbitJob(func5, { job4 });
	int job6 = s.SumbitJob(func6, { job4,job5 });
	int job7 = s.SumbitJob(func7, { job4 });
	int job8 = s.SumbitJob(func8, { job4 });

	s.StartJobs();

	s.WaitJobs();

	system("pause");

	return 0;
}