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
		// �������߳�����
		_t = std::thread(std::bind(&IAlgorithm::threadAlgorithm, this));
		_t.detach();
	}

	virtual void stopAlgorithm()
	{
		// ���߳��˳�����ʱ�̻߳�û��ִ���꣬�����Ѿ���������Ϊ�գ���ʱ�̻߳����
		// TRICK �ر����� ���߳̽�����������
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