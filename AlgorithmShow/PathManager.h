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
		int len = swprintf_s(cTips, L"+:����  -:����\n"
			L"��ǰ�ٶ�(ԽСԽ��): %d\n"
			L"����F1:����ͼʾ\n"
			L"����F2:Ѱ·ͼʾ\n"
			L"   --����1:��������㷨DFS\n"
			L"   --����2:��������㷨BFS\n"
			L"   --����3:Dijkstra�㷨\n"
			L"   --����4:̰������㷨\n"
			L"   --����5:A*�㷨\n"
			L"   --����6:JPS�㷨\n"
			,
			g_speed);
		DrawText(hdc, cTips, len, &rc, DT_LEFT | DT_TOP);
	}

	virtual void init() override
	{
		// ��ʼ��������
		std::vector<std::vector<int>> vv(100, std::vector<int>(150, 0));
		initBlocks1(vv);

		for (auto &item : _uMap)
			item.second->setNeedData(&vv, 0, 0);
	}

	virtual void startAlgorithmInner() override
	{
		// ����ʼ��ǰ��
		if (_uMap.find(_eType) != _uMap.end())
			_uMap[_eType]->startAlgorithm(g_speed);
	}

	virtual void keyDown(WPARAM wParam) override
	{
		if ('1' <= wParam && wParam <= '9')
		{
			// �л�ͼ
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
			���磺
			   ------
			*	    |   #
			   ------
		*/
		// ǽ
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

		// ��ʼλ��
		vv[50][30] = E_EPT_START;

		// �յ�
		vv[60][130] = E_EPT_END;
	}

	void initBlocks2(std::vector<std::vector<int>> &vv)
	{
		/*
		���磺
		------
		*	    |   #
		------
		*/
		// ǽ
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

		// ��ʼλ��
		vv[30][80] = E_EPT_END;

		// �յ�
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