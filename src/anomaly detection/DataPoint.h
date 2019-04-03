#pragma once
#include <vector>
#include "Vector2.h"
#include "DetectionSettings.h"

struct Datapoint
{
public:
	Datapoint() {};
	Datapoint(float x, float y, int tick)
	{
		this->position = Vector2(x, y);
		this->tick = tick;
	}
	Vector2 position;
	std::vector<Datapoint*> neighbours;
	float distance;
	int tick;

#if USE_SUBPOINTS
	bool isDirty = false;
#endif

	float kDistance = FLT_MAX;
	float lrd = -1;
	float prkNeighbourhood = -1;
	std::vector<Datapoint*> rkNeighbours;
	std::vector<Datapoint*> tmp_neighbours, tmp_rkNeighbours;

	virtual ~Datapoint() {};
};
