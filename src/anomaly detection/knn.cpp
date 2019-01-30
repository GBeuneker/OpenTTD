#include "knn.h"

/// <summary>Constructor for K-Nearest Neighbours.</summary>
/// <param name='k'>The amount of neighbours we want to use.</param>
KNN::KNN(uint16_t k)
{
	this->k = k;
	for (int i = 0; i < WINDOW_SIZE; ++i)
		distances[i] = FLT_MAX;
}

/// <summary>Runs the KNN algorithm on all datacharts to determine if there is an anomaly.</summary>
void KNN::Run()
{
	std::vector<Classification> results;

	for (int i = 0; i < datacharts.size(); ++i)
	{
		DataChart* d = datacharts[i];
		Datapoint p = d->GetLast();
		results.push_back(Classify(d, p));
	}
}

/// <summary>Classifies whether a datapoint is anomalous.</summary>
/// <param name='d'>The collection of data we want to use for our classification.</param>
/// <param name='p'>The datapoint we would like to classify.</param>
Classification KNN::Classify(DataChart* d, Datapoint p)
{
	Classification result;

	int valuesSize = d->GetValues()->size();
	// If there aren't enough points, return empty result
	if (valuesSize <= k)
		return result;

	int offset = valuesSize > WINDOW_SIZE ? valuesSize - WINDOW_SIZE : 0;
	// Fill the list of distances
	for (int i = 0; i < WINDOW_SIZE && i < valuesSize; ++i)
		distances[i] = Distance(p.position, d->GetValues()->at(offset + i).position);

	// Sort distances
	std::sort(std::begin(distances), std::end(distances));

	// Get the k-th distance
	float maxDist = distances[k];

	//TODO: Determine the result
	result.certainty = 1;
	result.isAnomaly = maxDist > 4;

	return result;
}

KNN::~KNN()
{
}
