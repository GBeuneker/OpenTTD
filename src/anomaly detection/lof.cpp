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
		DataPoint p = d->GetLast();
		results.push_back(Classify(d, p));
	}
}

LOF::~LOF()
{
}

Classification LOF::Classify(DataChart * d, DataPoint p)
{
	Classification result;

	// Get k-neighbours of p
	std::vector<DataPoint> kNeighbours = GetNeighbours(d, p, k);

	// Get reach distance for every k-neighbour of p
	for (int i = 0; i < kNeighbours.size(); ++i)
	{

	}

	// Calculate lrd(p) = |k-neighbours(p)| / SUM(reach-distances)

	// Calculate LOF(p) = SUM(lrd(o) / lrd(p)) / |k-neighbours(p)|


	return result;
}

std::vector<DataPoint> LOF::GetNeighbours(DataChart * d, DataPoint p, uint16_t k)
{
	std::vector<DataPoint> values = *(d->GetValues());

	// Calculate the distance to p for all the values
	for (int i = 0; i < values.size(); ++i)
		values[i].distance = Distance(values[i].position, p.position);

	// Sort the values from smallest to greatest distance
	std::sort(values.begin(), values.end());

	// Get the first k values as the answer;
	std::vector<DataPoint> answer(values.begin(), values.begin() + k);

	return answer;
}
