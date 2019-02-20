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
	// How many points are at this position
	int count = 1;
	std::vector<Datapoint*> neighbours;
	float distance;

#if USE_LOF
	float kDistance = FLT_MAX;
	float lrd = -1;
	bool kDistanceReached = false;
	int exclusiveCount = 0;
#elif USE_LOCI
	float prkNeighbourhood = -1;
	std::vector<Datapoint*> rkNeighbours;
	std::vector<Datapoint*> tmp_neighbours, tmp_rkNeighbours;
#endif

	virtual ~Datapoint() {};
};
