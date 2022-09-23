#pragma once

template <typename T, typename ...Args>
class CFObjHandler
{
public:
	virtual T invoke(Args... args) const = 0;
};

template <typename U, typename T, typename ...Args>
class CFObjHandlerChild :public CFObjHandler<T, Args...>
{
public:
	CFObjHandlerChild(U&& tmpfuncobj) :
		functor(std::forward<U>(tmpfuncobj))
	{
	}
	virtual T invoke(Args... args) const
	{
		return functor(std::forward<Args>(args)...);
	};
private:
	U functor;
};

//泛化版本
template <typename T>
class FunctionObj;

//特化版本
template <typename T, typename... Args>
class FunctionObj<T(Args...)>
{
public:

	template <typename U>
	FunctionObj(U&& acobj) //可以接收各种可调用对象（函数对象，lambda表达式等)
	{
		handler = new CFObjHandlerChild<U, T, Args...>(std::forward<U>(acobj));
	}

	FunctionObj()
	{
		handler = nullptr;
	}

	FunctionObj(FunctionObj&& acobj)
	{
		handler = acobj.handler;
		acobj.handler = nullptr;
	}

	FunctionObj& operator =(FunctionObj&& acobj)
	{
		if (handler)
		{
			delete handler;
			handler = nullptr;
		}
		handler = acobj.handler;
		acobj.handler = nullptr;
		return *this;
	}

	~FunctionObj()
	{
		if (handler)
			delete handler;
	}

public:

	T operator()(Args... args) const
	{
		return handler->invoke(std::forward<Args>(args)...); //使用了std::forward实现参数的完美转发，保持原始实参的佐治或者右值性。
	}

	operator bool() const noexcept
	{
		return handler;
	}

private:
	CFObjHandler<T, Args...>* handler; //可调用对象处理器
};