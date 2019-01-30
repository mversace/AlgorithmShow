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

	virtual void drawJumpPointLine(HDC hdc, HBRUSH hBrushGreen, HBRUSH hBrushRed, HBRUSH hBrushBlue, HBRUSH hBrushBlack) {};
	virtual void drawJumpPoints(HDC hdc, HBRUSH hBrushGreen, HBRUSH hBrushRed, HBRUSH hBrushBlue, HBRUSH hBrushBlack) {}

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

		drawJumpPointLine(hdc, hBrushGreen, hBrushRed, hBrushBlue, hBrushBlack);

		// ��������·��
		for (auto &item : _vecPath)
		{
			RECT rc = { item % _hashKeyDef * _cellW + _x, item / _hashKeyDef * _cellW + _y, item % _hashKeyDef * _cellW + _x + _cellW, item / _hashKeyDef * _cellW + _y + _cellW };
			FillRect(hdc, &rc, hBrushGreen);
			FrameRect(hdc, &rc, hBrushBlack);
		}

		drawJumpPoints(hdc, hBrushGreen, hBrushRed, hBrushBlue, hBrushBlack);

		// �����ұߵ�_tipsShow
		RECT rc = { 1250, 300, 1500, 800 };
		DrawText(hdc, _tipsShow.c_str(), _tipsShow.length(), &rc, DT_LEFT | DT_TOP);
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
	std::wstring _tipsShow;
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
		_tipsShow = L"��������㷨DFS:\n"
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
		_tipsShow = L"�����������BFS:\n"
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
protected:
	// ���cost
	struct tBestCost
	{
		tBestCost() : cost(INT_MAX) {}

		int hashKey; // �ý���hashkey
		std::vector<int> v; // ����ýڵ�����·��
		int cost; // ����ýڵ��cost
	};

	int _openSetTotal = 0;

	using pqItemType = std::pair<int, int>;
	using pqCmpType = std::function<bool(const pqItemType&, const pqItemType&)>;
	using pqType = std::priority_queue<pqItemType, std::vector<pqItemType>, pqCmpType>;

	virtual void InitStartData(pqType& s)
	{
		s.emplace(0, _start);
	}

	virtual void CheckAndAddNodes(pqType& s, std::unordered_map<int, tBestCost>& mapCost, int curKey)
	{
		auto itCur = mapCost.find(curKey);
		if (itCur == mapCost.end())
		{
			// �������ȼ������еĽڵ�ض��Ǽ�����ģ�û��������������
			return;
		}

		for (eDir e = E_ED_UP; e <= E_ED_LEFTUP; e = eDir(e + 1))
		{
			int keyTemp = chgDir(curKey, e);
			if (!isValid(keyTemp)) continue;

			int costTemp = getDijkstraCost(e, itCur->second.cost) + getGreedyCost(keyTemp);

			// ������Χ8����Ľڵ�cost
			auto it = mapCost.find(keyTemp);

			// ������Ҷ���
			if (it == mapCost.end())
			{
				s.emplace(costTemp, keyTemp);
				++_openSetTotal;
			}

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

	void clearData() override
	{
		BasePath::clearData();
		_openSetTotal = 0;
	}

	virtual int getDijkstraCost(eDir dir, int curCost) = 0;
	virtual int getGreedyCost(int curKey) = 0;

public:
	void threadAlgorithm()
	{
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
		pqCmpType cmp = [](const std::pair<int, int> &v1, const std::pair<int, int> &v2)->bool {
			return v1.first > v2.first;
		};
		pqType s1(cmp);
		
		InitStartData(s1);

		while (!s1.empty())
		{
			auto tData = std::move(s1.top());
			s1.pop();

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

			CheckAndAddNodes(s1, mapCost, p);

			_tipsShow = _tips + L"\n�����ڵ���:" + std::to_wstring(mapCost.size())
						+ L"\nopenset��ǰ:" + std::to_wstring(s1.size())
						+ L"\nopenset�ܼ�:" + std::to_wstring(_openSetTotal);
		}

		_vecPath = std::move(mapCost[_end].v);

		BasePath::threadAlgorithm();
	}

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

/**
 * JPS�㷨������A*���Ż�����Ҫ���Ż�openset
 */
class JPSPath : public AStarPath
{
private:
	// ��¼ÿ�������Ҫ���ķ��� key=hash value=dir1|dir2|...
	std::unordered_map<int, std::unordered_set<eDir>> _umapChecked;
	std::unordered_set<int> _usetJP; // ����
	HPEN _hPen = CreatePen(PS_SOLID, 3, RGB(0, 0, 255));

public:
	JPSPath()
	{
		_tips = L"JPS�㷨\n"
			L"����A*���Ż�\n"
			L"��Ҫ���Ż�openset\n";

		// ����costֵ
		dirCost = 3; // ��ͳ4����cost
		diagonalCost = 5; // б����cost
	}

	void clearData() override
	{
		AStarPath::clearData();

		_umapChecked.clear();
		_usetJP.clear();
	}

	#define CalcNewDir(a, b) eDir((a + E_ED_NONE + b) % E_ED_NONE)

	// ��JPS��mapCost�д��cost������DijkstraCost
	bool searchJumpPoint(pqType& s, std::unordered_map<int, tBestCost>& mapCost, int curKey, eDir dir)
	{
		auto itCur = mapCost.find(curKey);
		auto target = chgDir(curKey, dir);
		while (isValid(target))
		{
			if (itCur == mapCost.end())
			{
				// �����˴���
				return false;
			}

			bool bFind = false;

			// ����Ͳ���¼�����
			// _vv[target / _hashKeyDef][target % _hashKeyDef] = E_EPT_CHECKED;

			// ���openset
			int cost = getDijkstraCost(dir, itCur->second.cost);

			// ���mapcost
			auto itTemp = mapCost.find(target);
			if (itTemp == mapCost.end() || cost < itTemp->second.cost)
			{
				mapCost[target].hashKey = target;
				mapCost[target].v = itCur->second.v;
				mapCost[target].v.emplace_back(target);
				mapCost[target].cost = cost;
			}

			// б����
			if (dir % 2 == 1)
			{
				// �Ե�ǰ����Ϊ'ǰ'������'����''����'����
				bFind |= searchJumpPoint(s, mapCost, target, CalcNewDir(dir, -1));
				bFind |= searchJumpPoint(s, mapCost, target, CalcNewDir(dir, 1));
			}
			else
			{
				// �Ե�ǰ����Ϊ'ǰ'����block�����Ͽ��ߣ���ǰ��Ϊ����
				// ������Ѱ��Ϊ �Ϻ�����
				// ͬ����Ի��� �Ϻ�����
				auto left = chgDir(target, CalcNewDir(dir, -2));
				auto leftUp = chgDir(target, CalcNewDir(dir, -1));
				auto right = chgDir(target, CalcNewDir(dir, 2));
				auto rightUp = chgDir(target, CalcNewDir(dir, 1));

				// ��ǰ��������
				if (target == _end)
				{
					bFind = true;
				}
				else 
				{
					if (!isValid(left) && isValid(leftUp))
					{
						_umapChecked[target].insert(CalcNewDir(dir, -1));
						bFind = true;
					}
					if (!isValid(right) && isValid(rightUp))
					{
						_umapChecked[target].insert(CalcNewDir(dir, 1));
						bFind = true;
					}
				}
			}

			// �����Ѱmap
			_umapChecked[target].insert(dir);

			// �ѵ�ǰ����ǰ����ڵ�����������
			if (bFind)
			{
				s.emplace(cost + getGreedyCost(target), target);
				++_openSetTotal;
				_usetJP.insert(target);

				return true;
			}
			else
			{
				itCur = mapCost.find(target);
				target = chgDir(target, dir);
			}

			Sleep(_speed);
		}

		return false;
	}

	virtual void InitStartData(pqType& s) override
	{
		AStarPath::InitStartData(s);
		
		// ��ʼ����������8����
		_umapChecked[_start] = { E_ED_UP, E_ED_RIGHT, E_ED_DOWN, E_ED_LEFT, E_ED_LEFTUP, E_ED_RIGHTUP, E_ED_RIGHTDOWN, E_ED_LEFTDOWN };

	}

	virtual void CheckAndAddNodes(pqType& s, std::unordered_map<int, tBestCost>& mapCost, int curKey) override
	{
		auto itCur = mapCost.find(curKey);
		if (itCur == mapCost.end())
		{
			// �������ȼ������еĽڵ�ض��Ǽ�����ģ�û��������������
			return;
		}

		// �õ���Ҫ������Щ����
		auto dirSet = _umapChecked[curKey];
		if (dirSet.empty())
		{
			// �˴�û��ֵ�������ݳ�����
			return;
		}

		for (eDir e = E_ED_UP; e <= E_ED_LEFTUP; e = eDir(e + 1))
		{
			if (dirSet.count(e))
				searchJumpPoint(s, mapCost, curKey, e);
		}
	}

	virtual void drawJumpPointLine(HDC hdc, HBRUSH hBrushGreen, HBRUSH hBrushRed, HBRUSH hBrushBlue, HBRUSH hBrushBlack) override
	{
		// GDI����Ч�ʺò����Ͳ����Ż���
		HPEN hOldPen = (HPEN)SelectObject(hdc, _hPen);

		for (auto &item : _umapChecked)
		{
			int left = item.first % _hashKeyDef * _cellW + _x;
			int right = left + _cellW;
			int top = item.first / _hashKeyDef * _cellW + _y;
			int bottom = top + _cellW;

			// ��
			if (item.second.count(E_ED_LEFT) || item.second.count(E_ED_RIGHT))
			{
				MoveToEx(hdc, left, (top + bottom) / 2, nullptr);
				LineTo(hdc, right, (top + bottom) / 2);
			}

			// ��
			if (item.second.count(E_ED_UP) || item.second.count(E_ED_DOWN))
			{
				MoveToEx(hdc, (left + right) / 2, top, nullptr);
				LineTo(hdc, (left + right) / 2, bottom);
			}

			// ���µ�����б
			if (item.second.count(E_ED_RIGHTUP) || item.second.count(E_ED_LEFTDOWN))
			{
				MoveToEx(hdc, left, bottom, nullptr);
				LineTo(hdc, right, top);
			}

			// ���µ�����б
			if (item.second.count(E_ED_LEFTUP) || item.second.count(E_ED_RIGHTDOWN))
			{
				MoveToEx(hdc, left, top, nullptr);
				LineTo(hdc, right, bottom);
			}
		}
		SelectObject(hdc, hOldPen);
	}

	virtual void drawJumpPoints(HDC hdc, HBRUSH hBrushGreen, HBRUSH hBrushRed, HBRUSH hBrushBlue, HBRUSH hBrushBlack)
	{
		for (auto &item : _usetJP)
		{
			RECT rc = { item % _hashKeyDef * _cellW + _x, item / _hashKeyDef * _cellW + _y, item % _hashKeyDef * _cellW + _x + _cellW, item / _hashKeyDef * _cellW + _y + _cellW };
			FillRect(hdc, &rc, hBrushRed);
			FrameRect(hdc, &rc, hBrushRed);
		}
	}
};