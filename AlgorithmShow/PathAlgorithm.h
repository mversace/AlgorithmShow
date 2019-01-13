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

// �������
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
		// Ѱ����㡢�յ�
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

		// ��������·��
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

		// �����ұߵ�_tips
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

	// ���ݴ��������hashKey��ö�Ӧ�����hashKey
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

	// �Ż�����·��
	void calcFinalPath(std::vector<int> &st)
	{
		// stΪ�����յ�ʱ�����ĵ㣬�����յ㷴��Ѱ�ҽ����ŵĵ�
		// *������·���������Լ��ٺܶ�����
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
	int _hashKeyDef = 1000;	// ����������� hashkey = row * _hashKeyDef + col;

	int _start;
	int _end;

	int _width = 0;
	int _height = 0;
	int _cellW = 8;
};

/*
	��������㷨
	����һ����֧�ߵ��ף����ݼ����������
*/
class DFSPath : public BasePath
{
public:
	DFSPath()
	{
		_tips = L"��������㷨DFS:\n"
			   L"����㳯ĳ�������ߵ��ף�����\n"
			   L"����������Ѱ�ĵ㣬��ѯ���·��\n";
	}

	void threadAlgorithm() override final
	{
		std::vector<int> st, s1;
		s1.push_back(_start);

		// ����һ������
		while (!s1.empty())
		{
			auto p = s1.back();

			// �鿴��ǰ��
			// ��鵱ǰ���Ƿ�Ϊ�յ�
			if (p == _end)
				break;

			_keyChecking = p;
			st.push_back(p);

			// ����ص������
			// trick ɾ�����õ㣬���������ɵ�·��ͼ��Ӱ��
// 			if (p == _start && st.size() > 1)
// 			{
// 				st.clear();
// 				st.push_back(_start);
// 				continue;
// 			}

			// ��ʼ̽��
			if (p != _start)
				_vv[p / _hashKeyDef][p % _hashKeyDef] = E_EPT_CHECKED;

			// ��������˳���������8������Ϊ�Ƿ�����������������E_ED_LEFTUP
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
	�����������BFS
	����������Ĳ��������α�����ǰ���е�Ľ����ŵĽڵ�
*/
class BFSPath : public BasePath
{
public:
	BFSPath()
	{
		_tips = L"�����������BFS:\n"
			L"���α�����ǰ���е�Ľ����ŵĽڵ�";
	}

	void threadAlgorithm()
	{
		std::vector<int> st, s1, s2;
		s1.push_back(_start);
		// ����һ������
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

				// �鿴��ǰ��
				// ��鵱ǰ���Ƿ�Ϊ�յ�
				if (p == _end)
				{
					s1.clear();
					break;
				}

				// ��ʼ̽��
				if (p != _start)
					_vv[p / _hashKeyDef][p % _hashKeyDef] = E_EPT_CHECKED;

				// ��������˳���������8����
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
	Ѱ·�㷨����
*/
class DGAPathbase : public BasePath
{
public:
	void threadAlgorithm()
	{
		// ���cost
		struct tBestCost
		{
			tBestCost() : cost(INT_MAX) {}

			int hashKey; // �ý���hashkey
			std::vector<int> v; // ����ýڵ�����·��
			int cost; // ����ýڵ��cost
		};

		// �洢���������ӵ�costֵ
		// key = i * _hashKeyDef + j
		// tBestCost.cost ��Ŀǰ�ڵ�Ĵ���
		// tBestCost.v ��Ŀǰ�ڵ������·��
		std::unordered_map<int, tBestCost> mapCost;

		// ��ʼ�����
		int hashStart = _start;
		mapCost[hashStart].hashKey = hashStart;
		mapCost[hashStart].v.emplace_back(hashStart);
		mapCost[hashStart].cost = 0;

		// �������ȼ����У����ս��cost����
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
				// �������ȼ������еĽڵ�ض��Ǽ�����ģ�û��������������
				break;
			}

			_keyChecking = tData.second;
			auto p = _keyChecking;

			// �鿴��ǰ��
			// ��鵱ǰ���Ƿ�Ϊ�յ�
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

				// ������Χ8����Ľڵ�cost
				auto it = mapCost.find(keyTemp);

				// ������Ҷ���
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
	int dirCost = 3; // ��ͳ4����cost
	int diagonalCost = 5; // б����cost
};

/*
	Dijkstra�㷨 BFS�ĸĽ���
	����û�в��ҵĽڵ㣬������Χ�ڵ�����·��
	���㵱ǰ�ڵ�����·����DP
*/
class DijkstraPath : public DGAPathbase
{
public:
	DijkstraPath()
	{
		_tips = L"Dijkstra�㷨 ��������ĸĽ���\n"
			L"����û�в��ҵĽڵ�\n"
			L"������Χ8����Ľڵ�cost";
	}

protected:
	virtual int getDijkstraCost(eDir dir, int curCost)
	{
		// Dijkstra����DP˼�룬���㵱ǰ�ڵ�curKey��cost
		// ����4���������Ϊa��б��Ϊb
		// ���� (0,0) -> (0, 2)
		// ֱ������
		// (0, 0) -> (0, 1) -> (0, 2) ����С�� (0, 0) -> (1, 1) -> (0, 2)
		// �� a * 2 <= b * 2
		// б������
		// (0, 0) -> (1, 1) ����С�� (0, 0) -> (0, 1) - > (1, 1)
		// �� a * 2 > b
		// ����  b < a * 2 < b * 2
		// a,b���ɵ�������Ӧ��Ϸ

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
	̰������㷨
	����Ҫ�����Ķ������򣬸��ݵ���Ŀ������������
*/
class GreedyPath : public DGAPathbase
{
public:
	GreedyPath()
	{
		_tips = L"̰������㷨\n"
			L"����Ҫ�����Ľڵ�����\n"
			L"���ݵ�Ŀ��ľ���\n";
	}

protected:
	virtual int getDijkstraCost(eDir dir, int curCost)
	{
		return 0;
	}

	virtual int getGreedyCost(int curKey)
	{
		// ̰���㷨������curKey��Ŀ��ص��cost

		int iDiff = abs(curKey / _hashKeyDef - _end / _hashKeyDef);
		int jDiff = abs(curKey % _hashKeyDef - _end % _hashKeyDef);

		// ��Ϊ����б����
		return (max(iDiff, jDiff) - min(iDiff, jDiff)) * dirCost + min(iDiff, jDiff) * diagonalCost;
	}
};

/*
	A*�㷨,
	���Dijkstra��̰���㷨ʵ��
*/
class AStarPath : public DGAPathbase
{
public:
	AStarPath()
	{
		_tips = L"A*�㷨\n"
			L"���Dijkstra��̰���㷨\n";

		// A*�㷨��ҪDijkstraCosռ��Ҫ����������������·���Ÿ�����������·��
		// �������²�������֤getDijkstraCost������ϴ�ֵ
		dirCost = 3; // ��ͳ4����cost
		diagonalCost = 20; // б����cost
	}

protected:
	virtual int getDijkstraCost(eDir dir, int curCost)
	{
		// Dijkstra����DP˼�룬���㵱ǰ�ڵ�curKey��cost
		// ����4���������Ϊa��б��Ϊb
		// ���� (0,0) -> (0, 2)
		// ֱ������
		// (0, 0) -> (0, 1) -> (0, 2) ����С�� (0, 0) -> (1, 1) -> (0, 2)
		// �� a * 2 <= b * 2
		// б������
		// (0, 0) -> (1, 1) ����С�� (0, 0) -> (0, 1) - > (1, 1)
		// �� a * 2 > b
		// ����  b < a * 2 < b * 2
		// a,b���ɵ�������Ӧ��Ϸ

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
		// ̰���㷨������curKey��Ŀ��ص��cost

		int iDiff = abs(curKey / _hashKeyDef - _end / _hashKeyDef);
		int jDiff = abs(curKey % _hashKeyDef - _end % _hashKeyDef);

		// ��Ϊ����б����
		return (max(iDiff, jDiff) - min(iDiff, jDiff)) * dirCost + min(iDiff, jDiff) * diagonalCost;
	}
};