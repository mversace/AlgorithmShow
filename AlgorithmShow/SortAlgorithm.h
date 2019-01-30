/*
	10�������㷨ʵ��

	_tips = ������ ʱ�临�Ӷ� �ռ临�Ӷ� �ȶ���
	�ȶ��ԣ����һ�������к�����ͬ��ֵA,B����������û�иı�A,B�����λ�ã���Ϊ�ȶ�
*/
#pragma once
#include <random>
#include <vector>
#include <unordered_set>
#include <string>
#include "IAlgorithm.h"

// �������
class BaseSort : public IAlgorithm
{
public:
	virtual ~BaseSort()
	{
		stopAlgorithm();
	}

public:
	virtual void setNeedData(void* p, int left, int top) override
	{
		_v = *(std::vector<int>*)p;
		_x = left;
		_y = top;
	}

	void clearData() override
	{
		_compareIdxSet.clear();
		_sortedIdxSet.clear();
		_swapIdxSet.clear();
	}

	void drawV(HDC hdc, HBRUSH hBrushGreen, HBRUSH hBrushRed, HBRUSH hBrushBlue, HBRUSH hBrushBlack)
	{
		std::unordered_set<int> c;
		std::unordered_set<int> f;
		std::unordered_set<int> s;

		do 
		{
			std::lock_guard<std::mutex> lg(_mutex);
			// STL �̲߳���ȫ�������ȿ���һ��
			c = _compareIdxSet;
			f = _sortedIdxSet;
			s = _swapIdxSet;
		} while (false);

		int tangleW = 10;
		// ���Ʊ���ͼ
		RECT rcBG = { _x, _y, _x + tangleW * 50, _y + 200 };
		FrameRect(hdc, &rcBG, hBrushBlack);

		// ������������
		rcBG.left += 10;
		rcBG.top += 5;
		DrawText(hdc, _tips.c_str(), _tips.length(), &rcBG, DT_TOP | DT_LEFT);

		int left = _x, bottom = _y + 200;
		RECT rc = { 0 };
		for (int i = 0; i < (int)_v.size(); ++i)
		{
			rc.left = left + i * tangleW;
			rc.top = bottom - _v[i];
			rc.right = rc.left + tangleW;
			rc.bottom = bottom;

			if (f.find(i) != f.end())
			{
				// �����
				FillRect(hdc, &rc, hBrushGreen);
			}
			else if (s.find(i) != s.end())
			{
				// �Ƚ�
				FillRect(hdc, &rc, hBrushRed);
			}
			else if (c.find(i) != c.end())
			{
				// ����
				FillRect(hdc, &rc, hBrushBlue);
			}

			FrameRect(hdc, &rc, hBrushBlack);
		}
	}

protected:
	int _x = 0;
	int _y = 0;
	std::wstring _tips;
	std::vector<int> _v;
	std::unordered_set<int> _compareIdxSet;
	std::unordered_set<int> _swapIdxSet;
	std::unordered_set<int> _sortedIdxSet;
};

/*
	ð������:
	������һ�ζԱ����ڵ���ֵ
	���v[i] > v[i + 1],����λ��
	��һ�α��������ֵ���ڶ���ĩβ��
*/
class BubbleSort : public BaseSort
{
public:
	BubbleSort()
	{
		_tips = _T("Bubble Sort  O(n^2)  O(1)  in-place  stable");
	}
protected:
	void threadAlgorithm() override final
	{
		int len = (int)_v.size();
		for (int i = 0; i < len; ++i)
		{
			bool noSwap = true;
			for (int j = 0; j < len - i - 1; ++j)
			{
				do 
				{
					std::lock_guard<std::mutex> lg(_mutex);
					_compareIdxSet.clear();
					_swapIdxSet.clear();
					_compareIdxSet.insert(j);
					_compareIdxSet.insert(j + 1);
				} while (false);

				Sleep(_speed);

				// compare v[i] v[j]
				if (_v[j] > _v[j + 1])
				{
					do 
					{
						std::lock_guard<std::mutex> lg(_mutex);
						_swapIdxSet.insert(j);
						_swapIdxSet.insert(j + 1);
					} while (false);

					std::swap(_v[j], _v[j + 1]);

					Sleep(_speed);
					noSwap = false;
				}
			}
			if (noSwap)
			{
				std::lock_guard<std::mutex> lg(_mutex);
				for (int i = 0; i < len; ++i)
					_sortedIdxSet.insert(i);
			}
			else
			{
				std::lock_guard<std::mutex> lg(_mutex);
				_sortedIdxSet.insert(len - i - 1);
			}
		}

		BaseSort::threadAlgorithm();
	}
};

/*
	ѡ������
	һ�α������ҵ���Сֵ���ڵ�ǰ�ĳ�ʼλ��
*/

class SelectionSort : public BaseSort
{
public:
	SelectionSort()
	{
		_tips = _T("Selection Sort  O(n^2)  O(1)  in-place  unstable");
	}

	void threadAlgorithm() override final
	{
		int len = (int)_v.size();
		for (int i = 0; i < len; ++i)
		{
			for (int j = i + 1; j < len; ++j)
			{
				do 
				{
					std::lock_guard<std::mutex> lg(_mutex);
					_compareIdxSet.clear();
					_swapIdxSet.clear();
					_compareIdxSet.insert(i);
					_compareIdxSet.insert(j);
				} while (false);

				Sleep(_speed);

				// compare v[i] v[j]
				if (_v[i] > _v[j])
				{
					do 
					{
						std::lock_guard<std::mutex> lg(_mutex);
						_swapIdxSet.insert(i);
						_swapIdxSet.insert(j);
					} while (false);

					std::swap(_v[i], _v[j]);

					Sleep(_speed);
				}
			}
			std::lock_guard<std::mutex> lg(_mutex);
			_sortedIdxSet.insert(i);
		}

		BaseSort::threadAlgorithm();
	}
};

/*
	��������
	1.Ĭ��������Ѿ�����õģ��ӵڶ���ֵ��ʼ�ж�
	2.��������������ҵ����ʵĲ���λ��
*/
class InsertionSort : public BaseSort
{
public:
	InsertionSort()
	{
		_tips = _T("Insertion Sort  O(n^2)  O(1)  in-place  stable");
	}

	void threadAlgorithm() override final
	{
		int len = (int)_v.size();
		for (int i = 1; i < len; ++i)
		{
			for (int j = i - 1, t = i; j >= 0; --j)
			{
				do 
				{
					std::lock_guard<std::mutex> lg(_mutex);
					_compareIdxSet.clear();
					_swapIdxSet.clear();
					_compareIdxSet.insert(i);
					_compareIdxSet.insert(j);
				} while (false);
				
				Sleep(_speed);

				// compare
				if (_v[t] < _v[j])
				{
					do 
					{
						std::lock_guard<std::mutex> lg(_mutex);
						_swapIdxSet.insert(t);
						_swapIdxSet.insert(j);
					} while (false);
					
					std::swap(_v[t], _v[j]);
					t = j;
					Sleep(_speed);
				}
				else
				{
					break;
				}
			}
		}
		std::lock_guard<std::mutex> lg(_mutex);
		for (int i = 0; i < len; ++i)
			_sortedIdxSet.insert(i);

		BaseSort::threadAlgorithm();
	}
};

/*
	ϣ������(��С��������)
	ѡ��һЩ������������5
	�ó��±�����Ϊ5�ģ� 0,5,10..5x ����len/5�β�������
	��С������ֱ��Ϊ1�����������
*/

class ShellSort : public BaseSort
{
public:
	ShellSort()
	{
		_tips = _T("Shell Sort  O(nlogn)  O(1)  in-place  unstable");
	}

	void threadAlgorithm() override final
	{
		int len = (int)_v.size();
		int add = len;
		while (add != 1)
		{
			add = add / 3 + 1;
			for (int i = /*0 + */add; i < len; ++i)
			{
				for (int j = i - add, t = i; j >= 0; j -= add)
				{
					do 
					{
						std::lock_guard<std::mutex> lg(_mutex);
						_compareIdxSet.clear();
						_swapIdxSet.clear();
						_compareIdxSet.insert(i);
						_compareIdxSet.insert(j);
					} while (false);
					
					Sleep(_speed);

					// compare
					if (_v[t] < _v[j])
					{
						do 
						{
							std::lock_guard<std::mutex> lg(_mutex);
							_swapIdxSet.insert(t);
							_swapIdxSet.insert(j);
						} while (false);
						
						std::swap(_v[t], _v[j]);
						t = j;
						Sleep(_speed);
					}
					else
					{
						break;
					}
				}
			}
		}

		std::lock_guard<std::mutex> lg(_mutex);
		for (int i = 0; i < len; ++i)
			_sortedIdxSet.insert(i);

		BaseSort::threadAlgorithm();
	}
};

/*
	�鲢����
	�Ѷ���ƽ����Ϊ2���֣���ÿһ���ֽ��й鲢����Ȼ��ϲ�2����
*/

class MergeSort : public BaseSort
{
public:
	MergeSort()
	{
		_tips = _T("Merge Sort  O(nlogn)  O(n)  out-place  stable");
	}

	void threadAlgorithm() override final
	{
		int len = (int)_v.size();
		mergeSortItem(_v, 0, len - 1);

		std::lock_guard<std::mutex> lg(_mutex);
		for (int i = 0; i < len; ++i)
			_sortedIdxSet.insert(i);

		BaseSort::threadAlgorithm();
	}

private:
	void mergeSortItem(std::vector<int> &v, int begin, int end)
	{
		if (begin >= end) return;
		if (end - begin == 1)
		{
			do 
			{
				std::lock_guard<std::mutex> lg(_mutex);
				_compareIdxSet.clear();
				_swapIdxSet.clear();
				_compareIdxSet.insert(begin);
				_compareIdxSet.insert(end);
			} while (false);
			
			Sleep(_speed);
			if (v[begin] > v[end])
			{
				do 
				{
					std::lock_guard<std::mutex> lg(_mutex);
					_swapIdxSet.insert(begin);
					_swapIdxSet.insert(end);
				} while (false);

				Sleep(_speed);
				std::swap(v[begin], v[end]);
			}
		}
		else
		{
			int itemLen = (end + begin) / 2;
			mergeSortItem(v, begin, itemLen);
			mergeSortItem(v, itemLen + 1, end);
			mergeVector(v, begin, itemLen, itemLen + 1, end);
		}
	}

	void mergeVector(std::vector<int> &v, int begin1, int end1, int begin2, int end2)
	{
		do 
		{
			std::lock_guard<std::mutex> lg(_mutex);
			_compareIdxSet.clear();
			_swapIdxSet.clear();
			for (int i = begin1; i <= end2; ++i)
				_compareIdxSet.insert(i);
		} while (false);

		std::vector<int> vv;
		int i1 = begin1, i2 = begin2;
		while (i1 <= end1 && i2 <= end2)
		{
			if (v[i1] <= v[i2])
				vv.push_back(v[i1++]);
			else
				vv.push_back(v[i2++]);

			Sleep(_speed);
		}
		while (i1 <= end1)
		{
			vv.push_back(v[i1++]);
			Sleep(_speed);
		}
		while (i2 <= end2)
		{
			vv.push_back(v[i2++]);
			Sleep(_speed);
		}

		for (int i = begin1, j = 0; i <= end2; ++i, ++j)
			v[i] = vv[j];
	}
};

/*
	��������
	��һ����׼ֵ���ѱ���С�ķŵ���ߣ��ݹ�ֺ����������
*/
class QuickSort : public BaseSort
{
public:
	QuickSort()
	{
		_tips = _T("Quick Sort  O(nlogn)  O(logn)  in-place  unstable");
	}

	void threadAlgorithm() override final
	{
		int len = (int)_v.size();
		sortPivotIdx(_v, 0, len - 1);

		std::lock_guard<std::mutex> lg(_mutex);
		for (int i = 0; i < len; ++i)
			_sortedIdxSet.insert(i);

		BaseSort::threadAlgorithm();
	}

private:
	void sortPivotIdx(std::vector<int> &v, int begin, int end)
	{
		if (begin >= end) return;
		if (end - begin == 1)
		{
			do 
			{
				std::lock_guard<std::mutex> lg(_mutex);
				_compareIdxSet.clear();
				_swapIdxSet.clear();
				_compareIdxSet.insert(begin);
				_compareIdxSet.insert(end);
			} while (false);
			
			Sleep(_speed);
			if (v[begin] > v[end])
			{
				do 
				{
					std::lock_guard<std::mutex> lg(_mutex);
					_swapIdxSet.insert(begin);
					_swapIdxSet.insert(end);
				} while (false);
				
				Sleep(_speed);
				std::swap(v[begin], v[end]);
			}
		}
		else
		{
			int pivot = begin;
			for (int i = pivot + 1; i <= end; ++i)
			{
				do 
				{
					std::lock_guard<std::mutex> lg(_mutex);
					_compareIdxSet.clear();
					_swapIdxSet.clear();
					_compareIdxSet.insert(i);
					_compareIdxSet.insert(pivot);
				} while (false);
				Sleep(_speed);
				
				if (v[i] < v[pivot])
				{
					do 
					{
						std::lock_guard<std::mutex> lg(_mutex);
						_swapIdxSet.insert(i);
						_swapIdxSet.insert(pivot);
					} while (false);
					
					std::swap(v[i], v[pivot]);

					Sleep(_speed);
					do 
					{
						std::lock_guard<std::mutex> lg(_mutex);
						_compareIdxSet.clear();
						_compareIdxSet.insert(i);
						_compareIdxSet.insert(pivot + 1);
					} while (false);
					Sleep(_speed);
					
					if (i > pivot + 1)
					{
						do 
						{
							_swapIdxSet.clear();
							_swapIdxSet.insert(i);
							_swapIdxSet.insert(pivot + 1);
						} while (false);
						Sleep(_speed);
						std::swap(v[i], v[pivot + 1]);
						pivot = pivot + 1;
					}
					else
					{
						pivot = i;
					}
				}
				Sleep(_speed);
			}

			sortPivotIdx(v, begin, pivot);
			sortPivotIdx(v, pivot + 1, end);
		}
	}
};

/*
	������
	������С�ѣ���ʣ�µļ�����������С��
	���Ѿ��Ƕ������������ʾ�����ղ�������洢�����ڵ�һ�������ӽڵ�
	��С�ѷ�֮
*/
class HeapSort : public BaseSort
{
public:
	HeapSort()
	{
		_tips = _T("Heap Sort  O(nlogn)  O(1)  in-place  unstable");
	}

	void threadAlgorithm() override final
	{
		makeDeap(_v);

		int len = (int)_v.size() - 1;
		while (len >= 0)
		{
			do
			{
				do 
				{
					std::lock_guard<std::mutex> lg(_mutex);
					_sortedIdxSet.insert(len);
				} while (false);
				
				Sleep(_speed);
			} while (false);

			if (len > 0)
			{
				std::swap(_v[0], _v[len]);
				adjustDeap(_v, 0, len - 1);
			}
			--len;
		}

		BaseSort::threadAlgorithm();
	}

private:
	void makeDeap(std::vector<int> &v)
	{
		int len = (int)v.size();
		for (int i = len / 2 - 1; i >= 0; --i)
			adjustDeap(v, i, len - 1);
	}

	void adjustDeap(std::vector<int> &v, int idx, int len)
	{
		// ������ʱ�Ǵ�Ҷ�ӿ�ʼ�����ģ��������ӽڵ��Ҵ�(С)����һ�������������Ҽ���
		int j = idx * 2 + 1;

		while (j <= len)
		{
			do
			{
				std::lock_guard<std::mutex> lg(_mutex);
				_compareIdxSet.clear();
				_compareIdxSet.insert(j);
				_compareIdxSet.insert(j + 1);
				_compareIdxSet.insert(idx);
			} while (false);
			Sleep(_speed);

			if (j + 1 <= len && v[j + 1] > v[j])
				++j;

			if (v[j] > v[idx])
			{
				do 
				{
					std::lock_guard<std::mutex> lg(_mutex);
					_swapIdxSet.clear();
					_swapIdxSet.insert(j);
					_swapIdxSet.insert(idx);
				} while (false);
				Sleep(_speed);

				std::swap(v[idx], v[j]);
			}
			else
				break;

			idx = j;
			j = j * 2 + 1;
		}
		Sleep(_speed);
	}
};

/*
	��������
	����һ���������飬�±��������������е�ֵ
	һ�α����������飬˳���ȡ����
*/
class CountingSort : public BaseSort
{
public:
	CountingSort()
	{
		_tips = _T("Counting Sort  O(n + k)  O(K)  out-place  stable  ˣ�����������ʾ..");
	}

	void threadAlgorithm() override final
	{
		int maxValue = INT_MIN;
		int minValue = INT_MAX;
		for (auto i : _v)
		{
			maxValue = max(maxValue, i);
			minValue = min(minValue, i);
		}
			
		std::vector<int> vv(maxValue - minValue + 1, 0);
		for (auto i : _v)
		{
			vv[i - minValue]++;
			Sleep(_speed);
		}

		int idx = 0;
		for (int i = 0; i < (int)vv.size(); ++i)
		{
			for (int j = 0; j < vv[i]; ++j)
			{
				do 
				{
					std::lock_guard<std::mutex> lg(_mutex);
					_sortedIdxSet.insert(idx);
				} while (false);
				
				Sleep(_speed);
				_v[idx++] = i;
			}
		}

		BaseSort::threadAlgorithm();
	}
};

/*
	Ͱ����
	������������棬���һ���㷨�������������ݾ��ȷ���Ͱ�ڣ�Ͱ������Ȼ��ϲ�
	���ڷֲ������ȵ����ݣ���Ҫ��ƶ����㷨
	��Ʋ��÷��������������
*/

class BucketSort : public BaseSort
{
public:
	BucketSort()
	{
		_tips = _T("Bucket Sort  O(n + k)  O(n + K)  out-place stable  ˣ�����������ʾ..");
	}

	void threadAlgorithm() override final
	{
		sortBucket(_v);
		std::lock_guard<std::mutex> lg(_mutex);
		for (int i = 0; i < (int)_v.size(); ++i)
			_sortedIdxSet.insert(i);

		BaseSort::threadAlgorithm();
	}

private:
	void sortBucket(std::vector<int> &v)
	{
		int len = (int)v.size();

		if (len < 2) return;
		if (len <= 10)
		{
			// С��5��ʱ���ò�������
			for (int i = 1; i < len; ++i)
			{
				for (int j = i - 1, t = i; j >= 0; --j)
				{
					if (v[j] > v[t])
					{
						Sleep(_speed);
						std::swap(v[t], v[j]);
						t = j;
					}
					else
						break;
				}
			}
			return;
		}

		int bucketCount = min(10, len / 5 + 1);
		std::vector<std::vector<int>> vv(bucketCount, std::initializer_list<int>());
		int maxValue = INT_MIN;
		int minValue = INT_MAX;
		for (auto i : v)
		{
			maxValue = max(maxValue, i);
			minValue = min(minValue, i);
		}
		
		int bucketCapacity = (maxValue - minValue) / bucketCount + 1;
		for (auto i : v)
		{
			vv[(i - minValue) / (bucketCapacity + 1)].push_back(i);
			Sleep(_speed);
		}

		for (auto &t : vv)
			sortBucket(t);

		int idx = 0;
		for (auto &t : vv)
		{
			for (auto i : t)
			{
				v[idx] = i;
				do 
				{
					std::lock_guard<std::mutex> lg(_mutex);
					_compareIdxSet.insert(idx);
				} while (false);
				
				Sleep(_speed);
				++idx;
			}
		}
	}
};

/*
	��������
	�ȸ��ݸ�λ��0-9�ŵ���ͬ��Ͱ���ٸ���ʮλ����Ͱ��ȡ�����������λ��
*/
class RadixSort : public BaseSort
{
public:
	RadixSort()
	{
		_tips = _T("Radix Sort  O(n * k)  O(n + K)  out-place stable  ˣ�����������ʾ..");
	}

	void threadAlgorithm() override final
	{
		int maxValue = INT_MIN;
		for (auto i : _v)
			maxValue = max(maxValue, i);
		int needTimes = 0;
		while (maxValue)
		{
			++needTimes;
			maxValue /= 10;
		}

		std::vector<std::vector<int>> vv(10, std::initializer_list<int>{});
		for (int i = 0; i < needTimes; ++i)
		{
			for (auto &c : vv)
				c.clear();

			int t = i, r = 1;
			while (t--) r *= 10;
			
			// ��Ͱ
			for (auto i : _v)
			{
				vv[i / r % 10].push_back(i);
				Sleep(_speed);
			}
			
			// ������ȥ
			int idx = 0;
			for (auto &k : vv)
			{
				for (auto l : k)
				{
					_v[idx] = l;
					++idx;
					Sleep(_speed);
				}
			}

			std::lock_guard<std::mutex> lg(_mutex);
			for (int m = 0; m < idx; ++m)
				_compareIdxSet.insert(m);
			Sleep(_speed);
		}

		std::lock_guard<std::mutex> lg(_mutex);
		for (int m = 0; m < (int)_v.size(); ++m)
			_sortedIdxSet.insert(m);

		BaseSort::threadAlgorithm();
	}
};