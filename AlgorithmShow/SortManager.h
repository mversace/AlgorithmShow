#pragma once

#include "SortAlgorithm.h"
#include "IManager.h"

class SortManager : public IManager
{
public:
	SortManager() : 
		_v(50, 0)
	{
		_vAlgorithm.push_back(&m_bs);
		_vAlgorithm.push_back(&m_ss);
		_vAlgorithm.push_back(&m_is);
		_vAlgorithm.push_back(&m_ss1);
		_vAlgorithm.push_back(&m_ms);
		_vAlgorithm.push_back(&m_qs);
		_vAlgorithm.push_back(&m_hs);
		_vAlgorithm.push_back(&m_cs);
		_vAlgorithm.push_back(&m_bs1);
		_vAlgorithm.push_back(&m_rs);

		init();
	}

	virtual ~SortManager()
	{
		
	}

public:
	static SortManager* getInstance()
	{
		static SortManager g;
		return &g;
	}

	void drawV(HDC hdc) override
	{
		IManager::drawV(hdc);

		TCHAR tips[200] = { 0 };
		int len = swprintf_s(tips, L"按键F1:排序图示    +:加速  -:减速\n"
								   L"按键F2:寻路图示    当前速度(每次动作后暂停时间,越小越快): %d\n"
								   ,
								   g_speed);
		RECT rc = {700, 650, 1300, 800};
		DrawText(hdc, tips, len, &rc, DT_LEFT | DT_TOP);
	}

	void init() override
	{
		// 生成随机vector
		std::default_random_engine engine(static_cast<unsigned int>(time(nullptr)));
		std::uniform_int_distribution<> dis(1, 180);
		for (int i = 0; i < 50; ++i)
			_v[i] = dis(engine);

		// 划分区域
		int x = 0, y = 0, width = 500, height = 200, space = 5;
		for (auto p : _vAlgorithm)
		{
			p->setNeedData(&_v, x, y);

			x += width + space;
			if (x > 1200)
			{
				x = 0;
				y += height + space;
			}
		}
	}

private:
	BubbleSort m_bs;
	SelectionSort m_ss;
	InsertionSort m_is;
	ShellSort m_ss1;
	MergeSort m_ms;
	QuickSort m_qs;
	HeapSort m_hs;
	CountingSort m_cs;
	BucketSort m_bs1;
	RadixSort m_rs;

	std::vector<int> _v;
};