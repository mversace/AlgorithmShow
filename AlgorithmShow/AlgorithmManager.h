#pragma once

#include <unordered_map>
#include "SortManager.h"
#include "PathManager.h"

enum eAlgorithmType
{
	E_EAT_SORT = 1,
	E_EAT_PATH
};

class AlgorithmManager
{
public:
	AlgorithmManager()
	{
		_uMap[E_EAT_SORT] = SortManager::getInstance();
		_uMap[E_EAT_PATH] = PathManager::getInstance();
	}

public:
	static AlgorithmManager* getInstance()
	{
		static AlgorithmManager g;
		return &g;
	}

	void startAlgorithm(eAlgorithmType eType)
	{
		if (_uMap.find(_eType) != _uMap.end())
			_uMap[_eType]->stopAlgorithm();

		_eType = eType;

		if (_uMap.find(_eType) != _uMap.end())
			_uMap[_eType]->startAlgorithm();
	}

	void updateSpeed()
	{
		if (_uMap.find(_eType) != _uMap.end())
			_uMap[_eType]->updateSpeed();
	}

	void drawV(HDC hdc)
	{
		if (_uMap.find(_eType) != _uMap.end())
			_uMap[_eType]->drawV(hdc);
	}

	void keyDown(WPARAM wParam)
	{
		switch (wParam)
		{
		case VK_F1:
			startAlgorithm(E_EAT_SORT);
			break;
		case VK_F2:
			startAlgorithm(E_EAT_PATH);
			break;
		case VK_OEM_PLUS:
			g_speed = max(0, g_speed - 5);
			updateSpeed();
			break;
		case VK_OEM_MINUS:
			g_speed = min(3000, g_speed + 5);
			updateSpeed();
			break;
		default:
			break;
		}

		if (_uMap.find(_eType) != _uMap.end())
			_uMap[_eType]->keyDown(wParam);
	}

private:
	eAlgorithmType _eType;
	std::unordered_map<eAlgorithmType, IManager*> _uMap;
};
