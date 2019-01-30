#pragma once
#include <vector>
#include "Vector2.h"

struct Datapoint
{
	Datapoint() {};
	Datapoint(float x, float y) { this->position = Vector2(x, y); }

	Vector2 position;
	float distance = -1;

	bool operator < (const Datapoint& dp) const
	{
		return (distance < dp.distance);
	}

	virtual ~Datapoint() {};
};

struct LOF_Datapoint : public Datapoint
{
	LOF_Datapoint() {};
	LOF_Datapoint(float x, float y) { this->position = Vector2(x, y); }

	float reachDistance = -1, kDistance = -1;
	float lrd = -1, lof = -1;
	std::vector<LOF_Datapoint> kNeighbours;
};

struct LOCI_Datapoint : public Datapoint
{
	LOCI_Datapoint() {};
	LOCI_Datapoint(float x, float y) { this->position = Vector2(x, y); }

	std::vector<LOCI_Datapoint> rNeighbours;
	float rNeighbourhood = -1;
};

