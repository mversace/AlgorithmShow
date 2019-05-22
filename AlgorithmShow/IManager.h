#pragma once
#include "IAlgorithm.h"

class IManager
{
public:
	IManager()
	{
		_hBrushBlack = CreateSolidBrush(RGB(0, 0, 0));
		_hBrushRed = CreateSolidBrush(RGB(255, 0, 0));
		_hBrushGreen = CreateSolidBrush(RGB(0, 255, 0));
		_hBrushBlue = CreateSolidBrush(RGB(0, 0, 255));
	}
	virtual ~IManager()
	{
		DeleteObject(_hBrushBlack);
		DeleteObject(_hBrushRed);
		DeleteObject(_hBrushGreen);
		DeleteObject(_hBrushBlue);
	}

public:
	virtual void startAlgorithm()
	{
		// 结束算法线程的sleep
		stopAlgorithm();

		// 初始化算法数据
		init();

		// 开始算法
		startAlgorithmInner();
	}

    virtual void stopAlgorithm()
	{
		for (auto p : _vAlgorithm)
			p->stopAlgorithmItem();

        // 等待所有线程结束
        bool isDone = false;
        while (!isDone)
        {
            isDone = true;
            for (auto p : _vAlgorithm)
            {
                if (p->isRun())
                {
                    isDone = false;
                    break;
                }
            }
        }
	}

	virtual void updateSpeed()
	{
		for (auto p : _vAlgorithm)
			p->updateSpeed(g_speed);
	}

	virtual void drawV(HDC hdc)
	{
		for (auto p : _vAlgorithm)
			p->drawV(hdc, _hBrushGreen, _hBrushRed, _hBrushBlue, _hBrushBlack);
	}

	virtual void keyDown(WPARAM wParam)
	{
		// do nothing
	};

protected:
	virtual void init() = 0;

	virtual void startAlgorithmInner()
	{
		for (auto p : _vAlgorithm)
			p->startAlgorithm(g_speed);
	}

protected:
	HBRUSH _hBrushBlack = nullptr;
	HBRUSH _hBrushRed = nullptr;
	HBRUSH _hBrushGreen = nullptr;
	HBRUSH _hBrushBlue = nullptr;

	std::vector<IAlgorithm*> _vAlgorithm;
};