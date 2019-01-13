#pragma once
#include "IAlgorithm.h"
#include <vector>
#include <stack>
#include <queue>
#include <unordered_set>
#include <algorithm>

enum ePointType
{
	E_EPT_SPACE = 0,
	E_EPT_BLOCK,
	E_EPT_START,
	E_EPT_END,
	E_EPT_CHECKED = 10,
};

enum eDir
{
	E_ED_UP = 0,
	E_ED_RIGHTUP,
	E_ED_RIGHT,
	E_ED_RIGHTDOWN,
	E_ED_DOWN,
	E_ED_LEFTDOWN,
	E_ED_LEFT,
	E_ED_LEFTUP,
	E_ED_NONE,
};

// 排序基类
class BasePath : public IAlgorithm
{
public:
	BasePath() :
		_vv(100, std::vector<int>(150, 0))
	{

	}
	virtual ~BasePath()
	{
		stopAlgorithm();
	}

public:
	virtual void setNeedData(void* p, int left, int top) override
	{
		_vv = *(std::vector<std::vector<int>>*)p;
		_x = left;
		_y = top;

		_height = (int)_vv.size();
		_width = (int)_vv[0].size();
		// 寻找起点、终点
		for (int i = 0; i < _height; ++i)
		{
			for (int j = 0; j < _width; ++j)
			{
				if (_vv[i][j] == E_EPT_START)
					_start = getHashKey(i, j);
				if (_vv[i][j] == E_EPT_END)
					_end = getHashKey(i, j);
			}
		}
	}

	void clearData() override
	{
		_keyChecking = -1;
		_vecPath.clear();
	}

	void drawV(HDC hdc, HBRUSH hBrushGreen, HBRUSH hBrushRed, HBRUSH hBrushBlue, HBRUSH hBrushBlack) override
	{
		for (int i = 0; i < _height; ++i)
		{
			for (int j = 0; j < _width; ++j)
			{
				RECT rc = { j * _cellW + _x, i * _cellW + _y, j * _cellW + _x + _cellW, i * _cellW + _y + _cellW };

				switch (_vv[i][j])
				{
				case E_EPT_SPACE: break;
				case E_EPT_BLOCK: FillRect(hdc, &rc, hBrushBlack); break; // block
				case E_EPT_START: FillRect(hdc, &rc, hBrushGreen); break; // start
				case E_EPT_END: FillRect(hdc, &rc, hBrushGreen); break; // end
				case E_EPT_CHECKED: FillRect(hdc, &rc, hBrushBlue); break; // checked
				default:
					break;
				}

				// is checking
				if (i * _hashKeyDef + j == _keyChecking)
					FillRect(hdc, &rc, hBrushRed);

				FrameRect(hdc, &rc, hBrushBlack);
			}
		}

		// 绘制最终路径
		do 
		{
			std::lock_guard<std::mutex> lg(_mutex);
			for (auto &item : _vecPath)
			{
				RECT rc = { item % _hashKeyDef * _cellW + _x, item / _hashKeyDef * _cellW + _y, item % _hashKeyDef * _cellW + _x + _cellW, item / _hashKeyDef * _cellW + _y + _cellW };
				FillRect(hdc, &rc, hBrushGreen);
				FrameRect(hdc, &rc, hBrushBlack);
			}
		} while (false);

		// 绘制右边的_tips
		RECT rc = { 1250, 300, 1500, 800 };
		DrawText(hdc, _tips.c_str(), _tips.length(), &rc, DT_LEFT | DT_TOP);
	}

protected:
	int getHashKey(int row, int col)
	{
		return row * _hashKeyDef + col;
	}

	virtual bool isCanCheck(int hashKey)
	{
		int i = hashKey / _hashKeyDef;
		int j = hashKey % _hashKeyDef;

		if (i < 0 || i >= (int)_vv.size()) return false;
		if (j < 0 || j >= (int)_vv[0].size()) return false;

		if (_vv[i][j] == E_EPT_SPACE || _vv[i][j] == E_EPT_END)
			return true;

		return false;
	}

	bool isValid(int hashKey)
	{
		int i = hashKey / _hashKeyDef;
		int j = hashKey % _hashKeyDef;

		if (i < 0 || i >= (int)_vv.size()) return false;
		if (j < 0 || j >= (int)_vv[0].size()) return false;

		if (_vv[i][j] == E_EPT_BLOCK)
			return false;

		return true;
	}

	// 根据传入的坐标hashKey获得对应方向的hashKey
	int chgDir(int hashKey, eDir dir)
	{
		switch (dir)
		{
		case E_ED_UP: return hashKey - _hashKeyDef; break;
		case E_ED_RIGHTUP: return hashKey - _hashKeyDef + 1; break;
		case E_ED_RIGHT: return hashKey + 1; break;
		case E_ED_RIGHTDOWN: return hashKey + 1 + _hashKeyDef; break;
		case E_ED_DOWN: return hashKey + _hashKeyDef; break;
		case E_ED_LEFTDOWN: return hashKey + _hashKeyDef - 1; break;
		case E_ED_LEFT: return hashKey - 1; break;
		case E_ED_LEFTUP: return hashKey - _hashKeyDef - 1; break;
		default:
			break;
		}

		return -1;
	}

	// 优化最终路径
	void calcFinalPath(std::vector<int> &st)
	{
		// st为到达终点时经过的点，根据终点反向寻找紧挨着的点
		// *非最优路径，但可以减少很多冗余
		std::lock_guard<std::mutex> lg(_mutex);
		auto t = _end;
		_vecPath.push_back(t);
		auto it = st.end();
		while ((it = find_if(st.begin(), it, [&](const int &value)->bool{
			if (t / _hashKeyDef == value / _hashKeyDef || t % _hashKeyDef == value % _hashKeyDef)
				return abs(t / _hashKeyDef - value / _hashKeyDef) + abs(t % _hashKeyDef - value % _hashKeyDef) == 1;
			else
				return abs(t / _hashKeyDef - value / _hashKeyDef) + abs(t % _hashKeyDef - value % _hashKeyDef) == 2;
			})
			) != st.end())
		{
			t = *it;
			_vecPath.push_back(t);
			if (it == st.begin())
				break;
			--it;
		}

		_keyChecking = -1;
	}

protected:
	int _x = 0;
	int _y = 0;
	std::wstring _tips;
	std::vector<std::vector<int>> _vv;
	std::vector<int> _vecPath;

	int _keyChecking = -1;
	int _hashKeyDef = 1000;	// 根据坐标计算 hashkey = row * _hashKeyDef + col;

	int _start;
	int _end;

	int _width = 0;
	int _height = 0;
	int _cellW = 8;
};

/*
	深度优先算法
	朝着一个分支走到底，回溯继续这个过程
*/
class DFSPath : public BasePath
{
public:
	DFSPath()
	{
		_tips = L"深度优先算法DFS:\n"
			   L"从起点朝某个方向走到底，回溯\n"
			   L"根据所有搜寻的点，查询最短路径\n";
	}

	void threadAlgorithm() override final
	{
		std::vector<int> st, s1;
		s1.push_back(_start);

		// 朝着一个方向
		while (!s1.empty())
		{
			auto p = s1.back();

			// 查看当前点
			// 检查当前点是否为终点
			if (p == _end)
				break;

			_keyChecking = p;
			st.push_back(p);

			// 如果回到了起点
			// trick 删掉无用点，对最终生成的路线图有影响
// 			if (p == _start && st.size() > 1)
// 			{
// 				st.clear();
// 				st.push_back(_start);
// 				continue;
// 			}

			// 开始探索
			if (p != _start)
				_vv[p / _hashKeyDef][p % _hashKeyDef] = E_EPT_CHECKED;

			// ↑→↓←顺序深度搜索8方向，因为是反序，所以最终是优先E_ED_LEFTUP
			bool bHas = false;
			for (eDir e = E_ED_UP; e <= E_ED_LEFTUP; e = eDir(e + 1))
			{
				int keyTemp = chgDir(p, e);
				if (!isCanCheck(keyTemp)) continue;

				s1.emplace_back(keyTemp);

				bHas = true;
				Sleep(_speed);
				break;
			}

			if (!bHas) s1.pop_back();

			Sleep(_speed);
		}

		calcFinalPath(st);

		BasePath::threadAlgorithm();
	}
};

/*
	广度优先搜索BFS
	就像二叉树的层排序，依次遍历当前所有点的紧挨着的节点
*/
class BFSPath : public BasePath
{
public:
	BFSPath()
	{
		_tips = L"广度优先搜索BFS:\n"
			L"依次遍历当前所有点的紧挨着的节点";
	}

	void threadAlgorithm()
	{
		std::vector<int> st, s1, s2;
		s1.push_back(_start);
		// 朝着一个方向
		while (!s1.empty())
		{
			s2 = std::move(s1);
			s1.clear();

			while (!s2.empty())
			{
				auto p = s2.back();
				s2.pop_back();
				_keyChecking = p;
				st.push_back(p);

				// 查看当前点
				// 检查当前点是否为终点
				if (p == _end)
				{
					s1.clear();
					break;
				}

				// 开始探索
				if (p != _start)
					_vv[p / _hashKeyDef][p % _hashKeyDef] = E_EPT_CHECKED;

				// →↓←↑顺序深度搜索8方向
				for (eDir e = E_ED_UP; e <= E_ED_LEFTUP; e = eDir(e + 1))
				{
					int keyTemp = chgDir(p, e);
					if (!isCanCheck(keyTemp)) continue;
					if (find(s1.begin(), s1.end(), keyTemp) != s1.end()) continue;

					s1.emplace_back(keyTemp);

					Sleep(_speed);
				}

				Sleep(_speed);
			}
		}

		calcFinalPath(st);
		BasePath::threadAlgorithm();
	}
};

/*
	寻路算法基类
*/
class DGAPathbase : public BasePath
{
public:
	void threadAlgorithm()
	{
		// 结点cost
		struct tBestCost
		{
			tBestCost() : cost(INT_MAX) {}

			int hashKey; // 该结点的hashkey
			std::vector<int> v; // 到达该节点的最短路径
			int cost; // 到达该节点的cost
		};

		// 存储已搜索格子的cost值
		// key = i * _hashKeyDef + j
		// tBestCost.cost 到目前节点的代价
		// tBestCost.v 到目前节点的最优路径
		std::unordered_map<int, tBestCost> mapCost;

		// 初始化起点
		int hashStart = _start;
		mapCost[hashStart].hashKey = hashStart;
		mapCost[hashStart].v.emplace_back(hashStart);
		mapCost[hashStart].cost = 0;

		// 查找优先级队列，按照结点cost升序
		auto cmp = [](const std::pair<int, int> &v1, const std::pair<int, int> &v2)->bool {
			return v1.first > v2.first;
		};
		std::priority_queue<std::pair<int, int>, std::vector<std::pair<int, int>>, decltype(cmp)> s1(cmp);
		s1.emplace(0, hashStart);

		while (!s1.empty())
		{
			auto tData = std::move(s1.top());
			s1.pop();

			auto itCur = mapCost.find(tData.second);
			if (itCur == mapCost.end())
			{
				// 加入优先级队列中的节点必定是计算过的，没计算则代表出错了
				break;
			}

			_keyChecking = tData.second;
			auto p = _keyChecking;

			// 查看当前点
			// 检查当前点是否为终点
			if (p == _end)
			{
				break;
			}

			if (p != _start)
				_vv[p / _hashKeyDef][p % _hashKeyDef] = E_EPT_CHECKED;

			for (eDir e = E_ED_UP; e <= E_ED_LEFTUP; e = eDir(e + 1))
			{
				int keyTemp = chgDir(p, e);
				if (!isValid(keyTemp)) continue;

				int costTemp = getDijkstraCost(e, itCur->second.cost) + getGreedyCost(keyTemp);

				// 更新周围8方向的节点cost
				auto it = mapCost.find(keyTemp);

				// 加入查找队列
				if (it == mapCost.end())
					s1.emplace(costTemp, keyTemp);

				if (it == mapCost.end() || it->second.cost > costTemp)
				{
					mapCost[keyTemp].hashKey = keyTemp;
					mapCost[keyTemp].v = itCur->second.v;
					mapCost[keyTemp].v.emplace_back(keyTemp);
					mapCost[keyTemp].cost = costTemp;
				}

				Sleep(_speed);
			}
		}

		_vecPath = std::move(mapCost[_end].v);

		BasePath::threadAlgorithm();
	}

protected:
	virtual int getDijkstraCost(eDir dir, int curCost) = 0;

	virtual int getGreedyCost(int curKey) = 0;

protected:
	int dirCost = 3; // 正统4方向cost
	int diagonalCost = 5; // 斜方向cost
};

/*
	Dijkstra算法 BFS的改进版
	遍历没有查找的节点，根据周围节点的最短路径
	计算当前节点的最短路径，DP
*/
class DijkstraPath : public DGAPathbase
{
public:
	DijkstraPath()
	{
		_tips = L"Dijkstra算法 宽度搜索的改进版\n"
			L"遍历没有查找的节点\n"
			L"更新周围8方向的节点cost";
	}

protected:
	virtual int getDijkstraCost(eDir dir, int curCost)
	{
		// Dijkstra利用DP思想，计算当前节点curKey的cost
		// 设置4正方向代价为a，斜向为b
		// 对于 (0,0) -> (0, 2)
		// 直线行走
		// (0, 0) -> (0, 1) -> (0, 2) 代价小于 (0, 0) -> (1, 1) -> (0, 2)
		// 则： a * 2 <= b * 2
		// 斜线行走
		// (0, 0) -> (1, 1) 代价小于 (0, 0) -> (0, 1) - > (1, 1)
		// 则： a * 2 > b
		// 于是  b < a * 2 < b * 2
		// a,b自由调整到适应游戏

		int cost = 0;
		switch (dir)
		{
		case E_ED_UP:
		case E_ED_RIGHT:
		case E_ED_DOWN:
		case E_ED_LEFT:
			cost = dirCost;
			break;
		case E_ED_RIGHTUP:
		case E_ED_RIGHTDOWN:
		case E_ED_LEFTDOWN:
		case E_ED_LEFTUP:
			cost = diagonalCost;
			break;
		default:
			break;
		}

		return curCost + cost;
	}

	virtual int getGreedyCost(int curKey)
	{
		return 0;
	}
};

/*
	贪婪最佳算法
	给需要遍历的定点排序，根据到达目标距离最近排序
*/
class GreedyPath : public DGAPathbase
{
public:
	GreedyPath()
	{
		_tips = L"贪婪最佳算法\n"
			L"给需要遍历的节点排序\n"
			L"根据到目标的距离\n";
	}

protected:
	virtual int getDijkstraCost(eDir dir, int curCost)
	{
		return 0;
	}

	virtual int getGreedyCost(int curKey)
	{
		// 贪心算法计算结点curKey到目标地点的cost

		int iDiff = abs(curKey / _hashKeyDef - _end / _hashKeyDef);
		int jDiff = abs(curKey % _hashKeyDef - _end % _hashKeyDef);

		// 因为可以斜着走
		return (max(iDiff, jDiff) - min(iDiff, jDiff)) * dirCost + min(iDiff, jDiff) * diagonalCost;
	}
};

/*
	A*算法,
	结合Dijkstra和贪婪算法实现
*/
class AStarPath : public DGAPathbase
{
public:
	AStarPath()
	{
		_tips = L"A*算法\n"
			L"结合Dijkstra和贪婪算法\n";

		// A*算法需要DijkstraCos占主要，这样看起来最终路径才更靠近与最优路径
		// 调整以下参数来保证getDijkstraCost计算出较大值
		dirCost = 3; // 正统4方向cost
		diagonalCost = 20; // 斜方向cost
	}

protected:
	virtual int getDijkstraCost(eDir dir, int curCost)
	{
		// Dijkstra利用DP思想，计算当前节点curKey的cost
		// 设置4正方向代价为a，斜向为b
		// 对于 (0,0) -> (0, 2)
		// 直线行走
		// (0, 0) -> (0, 1) -> (0, 2) 代价小于 (0, 0) -> (1, 1) -> (0, 2)
		// 则： a * 2 <= b * 2
		// 斜线行走
		// (0, 0) -> (1, 1) 代价小于 (0, 0) -> (0, 1) - > (1, 1)
		// 则： a * 2 > b
		// 于是  b < a * 2 < b * 2
		// a,b自由调整到适应游戏

		int cost = 0;
		switch (dir)
		{
		case E_ED_UP:
		case E_ED_RIGHT:
		case E_ED_DOWN:
		case E_ED_LEFT:
			cost = dirCost;
			break;
		case E_ED_RIGHTUP:
		case E_ED_RIGHTDOWN:
		case E_ED_LEFTDOWN:
		case E_ED_LEFTUP:
			cost = diagonalCost;
			break;
		default:
			break;
		}

		return curCost + cost;
	}

	virtual int getGreedyCost(int curKey)
	{
		// 贪心算法计算结点curKey到目标地点的cost

		int iDiff = abs(curKey / _hashKeyDef - _end / _hashKeyDef);
		int jDiff = abs(curKey % _hashKeyDef - _end % _hashKeyDef);

		// 因为可以斜着走
		return (max(iDiff, jDiff) - min(iDiff, jDiff)) * dirCost + min(iDiff, jDiff) * diagonalCost;
	}
};