#pragma once
#include <vector>
#include "Vector2.h"
#include "DetectionSettings.h"

struct Datapoint
{
public:
	Datapoint() {};
	Datapoint(float x, float y) { this->position = Vector2(x, y); }
	Vector2 position;
	std::vector<Datapoint*> neighbours;
	float distance = FLT_MAX;

#if USE_LOF
	float reachDistance = FLT_MAX, kDistance = FLT_MAX;
	float lrd = -1;
#elif USE_LOCI
	float rNeighbourhood = -1;
#endif

	bool operator < (const Datapoint& dp) const
	{
		return (distance < dp.distance);
	}

	bool operator == (const Datapoint& dp) const
	{
		return (position.X == dp.position.X && position.Y == dp.position.Y);
	}

	bool operator < (const Datapoint* dp) const
	{
		return (distance < dp->distance);
	}

	bool operator == (const Datapoint* dp) const
	{
		return (position.X == dp->position.X && position.Y == dp->position.Y);
	}

	virtual ~Datapoint() {};
};
