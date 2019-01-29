#include "lof.h"

LOF::LOF()
{
}

void LOF::Run()
{
	std::vector<Classification> results;

	for (int i = 0; i < datacharts.size(); ++i)
	{
		DataChart* d = datacharts[i];
		LOF_DataPoint p = d->GetLast();
		results.push_back(Classify(d, p));
	}
}

LOF::~LOF()
{
}

Classification LOF::Classify(DataChart * d, LOF_DataPoint p)
{
	Classification result;

	// Get k-neighbours of p
	SetKNeighbours(d, &p);

	// Get reach distance for every k-neighbour of p
	for (int i = 0; i < p.kNeighbours.size(); ++i)
	{
		LOF_DataPoint o = p.kNeighbours[i];
		GetReachDistance(d, p, o);
	}

	// Calculate lrd(p) = |k-neighbours(p)| / SUM(reach-distances)
	SetLRD(d, &p);
	for (int i = 0; i < p.kNeighbours.size(); ++i)
	{
		LOF_DataPoint o = p.kNeighbours[i];
		SetLRD(d, &o);
	}

	// Calculate LOF(p) = SUM(lrd(o) / lrd(p)) / |k-neighbours(p)|
	SetLOF(d, &p);

	return result;
}

void LOF::SetLOF(DataChart *d, LOF_DataPoint *p)
{
	if (p->lrd == -1)
		SetLRD(d, p);

	float lrdSum = 0;
	for (int i = 0; i < p->kNeighbours.size(); ++i)
	{
		LOF_DataPoint o = p->kNeighbours[i];
		if (o.lrd == -1)
			SetLRD(d, &o);

		lrdSum += (o.lrd / p->lrd);
	}

	p->lof = lrdSum / p->kNeighbours.size();
}

void LOF::SetLRD(DataChart *d, LOF_DataPoint *p)
{
	float reachDistSum = 0;
	for (int i = 0; i < p->kNeighbours.size(); ++i)
		reachDistSum += GetReachDistance(d, *p, p->kNeighbours[i]);

	p->lrd = (p->kNeighbours.size()) / reachDistSum;
}

float LOF::GetReachDistance(DataChart *d, LOF_DataPoint p, LOF_DataPoint o)
{
	if (o.kDistance == -1)
		SetKDistance(d, &o);

	return fmax(Distance(p.position, o.position), o.kDistance);
}

void LOF::SetKDistance(DataChart *d, LOF_DataPoint *p)
{
	// If p doesn't know its neighbours yet, set the neighbours
	if (p->kNeighbours.size() == 0)
		SetKNeighbours(d, p);

	// Set the distance to the k-th nearest neighbour
	p->kDistance = Distance(p->kNeighbours.back().position, p->position);
}

void LOF::SetKNeighbours(DataChart * d, LOF_DataPoint* p)
{
	std::vector<LOF_DataPoint> values = *(d->GetValues());

	// Calculate the distance to p for all the values
	for (int i = 0; i < values.size(); ++i)
		values[i].distance = Distance(values[i].position, p.position);

	// Sort the values from smallest to greatest distance
	std::sort(values.begin(), values.end());

	// Get the first k values as the answer
	std::vector<LOF_DataPoint> answer(values.begin(), values.begin() + k);

	// Set the k-neighbours of the datapoint p
	p->kNeighbours = answer;
}
