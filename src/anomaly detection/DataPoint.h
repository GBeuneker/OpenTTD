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

#if USE_LOF
	float reachDistance = FLT_MAX, kDistance = FLT_MAX;
	float lrd = -1;
#elif USE_LOCI
	float prkNeighbourhood = -1;
	std::vector<Datapoint*> krNeighbours, kNeighbours;
#endif

	virtual ~Datapoint() {};
};
