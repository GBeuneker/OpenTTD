#include "lof.h"

LOF::LOF(uint16_t k)
{
	this->k = k;
}

void LOF::Run()
{
	std::vector<Classification> results;

	for (int i = 0; i < datacharts.size(); ++i)
	{
		DataChart* d = datacharts[i];
		Datapoint datapoint = d->GetLast();

		LOF_Datapoint p = LOF_Datapoint(datapoint.position.X, datapoint.position.Y);
		Classify(d, p);

		//results.push_back(Classify(d, *p));
	}
}

Classification LOF::Classify(DataChart * d, LOF_Datapoint p)
{
	Classification result;

	// Get k-neighbours of p
	SetKNeighbours(d, &p);

	// Get reach distance for every k-neighbour of p
	for (int i = 0; i < p.kNeighbours.size(); ++i)
	{
		//// Set the k-neighbours of o
		//SetKNeighbours(d, &p.kNeighbours[i]);
		//// Set the k-distance of o
		//SetKDistance(d, &p.kNeighbours[i]);

		// Get the reach distance = MAX(dist(o,p), o.kDistance)
		GetReachDistance(d, p, p.kNeighbours[i]);
	}

	// Calculate lrd(p) = |k-neighbours(p)| / SUM(reach-distances)
	SetLRD(d, &p);
	for (int i = 0; i < p.kNeighbours.size(); ++i)
		SetLRD(d, &p.kNeighbours[i]);

	// Calculate LOF(p) = SUM(lrd(o) / lrd(p)) / |k-neighbours(p)|
	SetLOF(d, &p);

	result.answer = false;
	result.certainty = p.lof;

	return result;
}

void LOF::SetLOF(DataChart *d, LOF_Datapoint *p)
{
	float lrdSum = 0;
	for (int i = 0; i < p->kNeighbours.size(); ++i)
	{
		LOF_Datapoint o = p->kNeighbours[i];
		lrdSum += (o.lrd / p->lrd);
	}

	p->lof = lrdSum / p->kNeighbours.size();
}

void LOF::SetLRD(DataChart *d, LOF_Datapoint *p)
{
	float reachDistSum = 0;
	for (int i = 0; i < p->kNeighbours.size(); ++i)
		reachDistSum += GetReachDistance(d, *p, p->kNeighbours[i]);

	// Ensure we don't divide by zero
	if (reachDistSum == 0)
		reachDistSum = 0.000001f;

	p->lrd = (p->kNeighbours.size()) / reachDistSum;
}

float LOF::GetReachDistance(DataChart *d, LOF_Datapoint p, LOF_Datapoint o)
{
	return fmax(Distance(p.position, o.position), o.kDistance);
}

void LOF::SetKDistance(DataChart *d, LOF_Datapoint *p)
{
	// Set the distance to the k-th nearest neighbour
	p->kDistance = Distance(p->kNeighbours.back().position, p->position);
}

void LOF::SetKNeighbours(DataChart * d, LOF_Datapoint* p)
{
	std::vector<Datapoint> datapoints = *(d->GetValues());

	int startIndex = datapoints.size() > WINDOW_SIZE ? datapoints.size() - WINDOW_SIZE : 0;
	int endIndex = datapoints.size() > WINDOW_SIZE ? startIndex + WINDOW_SIZE : datapoints.size();
	// Calculate the distance to p for all the values
	for (int i = startIndex; i < endIndex; ++i)
	{
		// Get the position from the Datapoint
		Vector2 pos = datapoints[i].position;
		float dist = Distance(datapoints[i].position, p->position);
		// Convert to a LOF_DataPoint
		LOF_Datapoint lofDatapoint = LOF_Datapoint(pos.X, pos.Y);
		lofDatapoint.distance = dist;

		// Add it to the list
		lofDatapoints[i - startIndex] = lofDatapoint;
	}

	// Sort the values from smallest to greatest distance
	std::sort(lofDatapoints, lofDatapoints + WINDOW_SIZE);

	// Get the closest k-neighbours
	std::vector<LOF_Datapoint> kNeighbours(k);
	std::copy_n(lofDatapoints, k, kNeighbours.begin());
	// Set the k-neighbours of the datapoint p
	p->kNeighbours = kNeighbours;
}

LOF::~LOF()
{
}
