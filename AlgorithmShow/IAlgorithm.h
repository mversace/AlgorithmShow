#pragma once

#include <thread>
#include <mutex>

__declspec(selectany) int g_speed = 30;

class IAlgorithm
{
public:
	IAlgorithm() = default;
	virtual ~IAlgorithm()
	{
		stopAlgorithm();
	}

public:
	virtual void updateSpeed(int speed)
	{
		_speed = speed;
	}

	virtual void startAlgorithm(int speed)
	{
		clearData();

		_speed = speed;
		_running = true;
		// 启动多线程排序
		_t = std::thread(std::bind(&IAlgorithm::threadAlgorithm, this));
		_t.detach();
	}

	virtual void stopAlgorithm()
	{
		// 主线程退出，此时线程还没有执行完，但类已经析构数据为空，此时线程会崩溃
		// TRICK 关闭限速 等线程结束后再析构
		_speed = 0;
		while (_running)
		{
			/* do nothing */
		}
	}

	virtual void setNeedData(void* p, int left, int top) = 0;
	virtual void clearData() = 0;
	virtual void drawV(HDC hdc, HBRUSH hBrushGreen, HBRUSH hBrushRed, 
					   HBRUSH hBrushBlue, HBRUSH hBrushBlack) = 0;

protected:
	virtual void threadAlgorithm()
	{
		_running = false;
	};

protected:
	std::thread _t;
	std::mutex _mutex;

	bool _running = false;
	int _speed = g_speed;
};