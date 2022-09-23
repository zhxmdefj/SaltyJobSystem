# SaltyJobSystem

## 构建方法

需要xmake

`xmake project -k vsxmake`


## 简单说明

系统内部实现一个线程池，主线程一次性将所有任务以闭包的functor的形式准备好，并在准备结束之后可通过 StartJobs() 触发。触发后提供等待函数 WaitJobs() ，允许主线程等待所有线程执行完毕。

实现了一个功能类似的FunctionObj，可传入如class，struct，lambda或普通的函数指针，也可使用std::function替代。