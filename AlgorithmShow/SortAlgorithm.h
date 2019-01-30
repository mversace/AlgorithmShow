/*
	10大排序算法实现

	_tips = 排序名 时间复杂度 空间复杂度 稳定性
	稳定性：如果一个数组中含有相同的值A,B，排序后如果没有改变A,B的相对位置，即为稳定
*/
#pragma once
#include <random>
#include <vector>
#include <unordered_set>
#include <string>
#include "IAlgorithm.h"

// 排序基类
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
			// STL 线程不安全，这里先拷贝一下
			c = _compareIdxSet;
			f = _sortedIdxSet;
			s = _swapIdxSet;
		} while (false);

		int tangleW = 10;
		// 绘制背景图
		RECT rcBG = { _x, _y, _x + tangleW * 50, _y + 200 };
		FrameRect(hdc, &rcBG, hBrushBlack);

		// 绘制排序名字
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
				// 已完成
				FillRect(hdc, &rc, hBrushGreen);
			}
			else if (s.find(i) != s.end())
			{
				// 比较
				FillRect(hdc, &rc, hBrushRed);
			}
			else if (c.find(i) != c.end())
			{
				// 查找
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
	冒泡排序:
	从左到右一次对比相邻的俩值
	如果v[i] > v[i + 1],交换位置
	第一次遍历后，最大值就在队列末尾了
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
	选择排序
	一次遍历，找到最小值放在当前的初始位置
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
	插入排序
	1.默认左边是已经排序好的，从第二个值开始判断
	2.从右向左遍历，找到合适的插入位置
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
	希尔排序(缩小增量排序)
	选择一些列增量，例如5
	拿出下标增量为5的： 0,5,10..5x 进行len/5次插入排序
	缩小增量，直到为1，则排序结束
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
	归并排序
	把队列平均分为2部分，对每一部分进行归并排序，然后合并2部分
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
	快速排序
	找一个基准值，把比它小的放到左边，递归分好组的两部分
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
	堆排序
	构建最小堆，把剩下的继续构建成最小堆
	最大堆就是二叉树的数组表示，按照层序遍历存储，父节点一定大于子节点
	最小堆反之
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
		// 构建堆时是从叶子开始构建的，所以俩子节点找大(小)的那一个，继续往下找即可
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
	计数排序
	建立一个额外数组，下标代表待排序数组中的值
	一次遍历放入数组，顺序读取排序
*/
class CountingSort : public BaseSort
{
public:
	CountingSort()
	{
		_tips = _T("Counting Sort  O(n + k)  O(K)  out-place  stable  耍赖排序随便演示..");
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
	桶排序
	计数排序改良版，设计一个算法，将带排序数据均匀放入桶内，桶内排序，然后合并
	对于分布不均匀的数据，需要设计独特算法
	设计不好反而不如计数排序
*/

class BucketSort : public BaseSort
{
public:
	BucketSort()
	{
		_tips = _T("Bucket Sort  O(n + k)  O(n + K)  out-place stable  耍赖排序随便演示..");
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
			// 小于5个时采用插入排序
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
	基数排序
	先根据个位数0-9放到不同的桶，再根据十位数，桶数取决于最大数的位数
*/
class RadixSort : public BaseSort
{
public:
	RadixSort()
	{
		_tips = _T("Radix Sort  O(n * k)  O(n + K)  out-place stable  耍赖排序随便演示..");
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
			
			// 分桶
			for (auto i : _v)
			{
				vv[i / r % 10].push_back(i);
				Sleep(_speed);
			}
			
			// 拷贝回去
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