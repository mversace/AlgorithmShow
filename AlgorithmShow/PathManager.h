#pragma once
#include "IManager.h"
#include "PathAlgorithm.h"
#include <unordered_map>

enum ePathAlgorithm
{
	E_EPA_DFS = 1,
	E_EPA_BFS,
	E_EPA_Dijkstra,
	E_EPA_GREEDY,
	E_EPA_ASTAR,
	E_EPA_JPS,
	E_EPA_MAX,
};

class PathManager : public IManager
{
public:
	PathManager() :
		_eType(E_EPA_DFS)
	{
		_vAlgorithm.push_back(&_dfs);
		_vAlgorithm.push_back(&_bfs);
		_vAlgorithm.push_back(&_Dijkstra);
		_vAlgorithm.push_back(&_greedy);
		_vAlgorithm.push_back(&_AStar);
		_vAlgorithm.push_back(&_JPS);

		_uMap[E_EPA_DFS] = &_dfs;
		_uMap[E_EPA_BFS] = &_bfs;
		_uMap[E_EPA_Dijkstra] = &_Dijkstra;
		_uMap[E_EPA_GREEDY] = &_greedy;
		_uMap[E_EPA_ASTAR] = &_AStar;
		_uMap[E_EPA_JPS] = &_JPS;

		init();
	}

	virtual ~PathManager()
	{

	}

public:
	static PathManager* getInstance()
	{
		static PathManager g;
		return &g;
	}

	virtual void drawV(HDC hdc) override
	{
		if (_uMap.find(_eType) != _uMap.end())
			_uMap[_eType]->drawV(hdc, _hBrushGreen, _hBrushRed, _hBrushBlue, _hBrushBlack);

		RECT rc = { 1250, 10, 1500, 300 };
		TCHAR cTips[200] = { 0 };
		int len = swprintf_s(cTips, L"+:加速  -:减速\n"
			L"当前速度(越小越快): %d\n"
			L"按键F1:排序图示\n"
			L"按键F2:寻路图示\n"
			L"   --按键1:深度优先算法DFS\n"
			L"   --按键2:广度优先算法BFS\n"
			L"   --按键3:Dijkstra算法\n"
			L"   --按键4:贪婪最佳算法\n"
			L"   --按键5:A*算法\n"
			L"   --按键6:JPS算法\n"
			,
			g_speed);
		DrawText(hdc, cTips, len, &rc, DT_LEFT | DT_TOP);
	}

	virtual void init() override
	{
		// 初始化给所有
		std::vector<std::vector<int>> vv(100, std::vector<int>(150, 0));
		initBlocks1(vv);

		for (auto &item : _uMap)
			item.second->setNeedData(&vv, 0, 0);
	}

	virtual void startAlgorithmInner() override
	{
		// 仅开始当前的
		if (_uMap.find(_eType) != _uMap.end())
			_uMap[_eType]->startAlgorithm(g_speed);
	}

	virtual void keyDown(WPARAM wParam) override
	{
		if ('1' <= wParam && wParam <= '9')
		{
			// 切换图
			_eType = (ePathAlgorithm)(wParam - '1' + 1);
			if (_eType >= E_EPA_MAX)
				_eType = E_EPA_DFS;

			startAlgorithm();
		}
	}

private:
	void initBlocks1(std::vector<std::vector<int>> &vv)
	{
		/*
			形如：
			   ------
			*	    |   #
			   ------
		*/
		// 墙
		for (int j = 50; j < 90; ++j)
		{
			for (int i = 20; i < 25; ++i)
				vv[i][j] = 1;
			for (int i = 75; i < 80; ++i)
				vv[i][j] = 1;
		}

		for (int i = 20; i < 80; ++i)
		{
			for (int j = 90; j < 95; ++j)
				vv[i][j] = 1;
		}

		// 起始位置
		vv[50][30] = E_EPT_START;

		// 终点
		vv[60][130] = E_EPT_END;
	}

	void initBlocks2(std::vector<std::vector<int>> &vv)
	{
		/*
		形如：
		------
		*	    |   #
		------
		*/
		// 墙
		for (int j = 50; j < 90; ++j)
		{
			for (int i = 20; i < 25; ++i)
				vv[i][j] = 1;
			for (int i = 75; i < 80; ++i)
				vv[i][j] = 1;
		}

		for (int i = 20; i < 80; ++i)
		{
			for (int j = 90; j < 95; ++j)
				vv[i][j] = 1;
		}

		for (int i = 20; i < 60; ++i)
		{
			vv[i][50] = 1;
		}

		for (int i = 75; i < 90; ++i)
		{
			vv[i][49] = 1;
		}

		// 起始位置
		vv[30][80] = E_EPT_END;

		// 终点
		vv[80][130] = E_EPT_START;
	}

private:
	ePathAlgorithm _eType;
	std::unordered_map<ePathAlgorithm, IAlgorithm*> _uMap;

	DFSPath _dfs;
	BFSPath _bfs;
	DijkstraPath _Dijkstra;
	GreedyPath _greedy;
	AStarPath _AStar;
	JPSPath _JPS;
};