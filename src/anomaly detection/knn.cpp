#include "knn.h"

KNN::KNN(uint16_t k)
{
	this->k = k;
	for (int i = 0; i < MAXSIZE; ++i)
		distances[i] = FLT_MAX;
}

void KNN::Run()
{
	std::vector<Classification> results;

	for (int i = 0; i < datacharts.size(); ++i)
	{
		DataChart* d = datacharts[i];
		Vector2 v = d->GetLast();
		results.push_back(Classify(d, v));
	}
}

Classification KNN::Classify(DataChart* d, Vector2 v)
{
	Classification result;

	int valuesSize = d->GetValues()->size();
	// If there aren't enough points, return empty result
	if (valuesSize <= k)
		return result;

	int offset = valuesSize > MAXSIZE ? valuesSize - MAXSIZE : 0;
	// Fill the list of distances
	for (int i = 0; i < MAXSIZE && i < valuesSize; ++i)
		distances[i] = Distance(v, d->GetValues()->at(offset + i));

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
