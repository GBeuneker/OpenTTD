#include "knn.h"

KNN::KNN(uint16_t k)
{
	this->k = k;
	for (int i = 0; i < WINDOW_SIZE; ++i)
		distances[i] = FLT_MAX;
}

void KNN::Run()
{
	std::vector<Classification> results;

	for (int i = 0; i < datacharts.size(); ++i)
	{
		DataChart* d = datacharts[i];
		DataPoint p = d->GetLast();
		results.push_back(Classify(d, p));
	}
}

Classification KNN::Classify(DataChart* dc, DataPoint p)
{
	Classification result;

	int valuesSize = dc->GetValues()->size();
	// If there aren't enough points, return empty result
	if (valuesSize <= k)
		return result;

	int offset = valuesSize > WINDOW_SIZE ? valuesSize - WINDOW_SIZE : 0;
	// Fill the list of distances
	for (int i = 0; i < WINDOW_SIZE && i < valuesSize; ++i)
		distances[i] = Distance(p.position, dc->GetValues()->at(offset + i).position);

	// Sort distances
	std::sort(std::begin(distances), std::end(distances));

	// Get the k-th distance
	float maxDist = distances[k];

	// Determine the result
	result.certainty = 1;
	result.answer = maxDist > 4;

	return result;
}

KNN::~KNN()
{
}
