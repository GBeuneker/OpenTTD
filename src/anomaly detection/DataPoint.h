#pragma once
#include <vector>
#include "Vector2.h"

struct DataPoint
{
	Vector2 position;
	float distance = -1;

	bool operator < (const DataPoint& dp) const
	{
		return (distance < dp.distance);
	}
};

struct LOF_DataPoint : public DataPoint
{
	float reachDistance = -1, kDistance = -1;
	float lrd = -1, lof = -1;
	std::vector<LOF_DataPoint> kNeighbours;
};

