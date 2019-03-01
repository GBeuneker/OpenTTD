#include "knn.h"

#if USE_KNN

/// <summary>Constructor for K-Nearest Neighbours.</summary>
/// <param name='k'>The amount of neighbours we want to use.</param>
KNN::KNN(uint16_t k_values[])
{
	this->k_values = k_values;
	for (int i = 0; i < WINDOW_SIZE; ++i)
		distances[i] = FLT_MAX;
}

void KNN::SetData(std::vector<DataChart*> _datacharts)
{
	Detector::SetData(_datacharts);
	// Assign all the kDistances
	kDistances = std::vector<float[WINDOW_SIZE]>(datacharts.size());
	kIndices = std::vector<int>(datacharts.size());
}

/// <summary>Classifies whether a datapoint is anomalous.</summary>
/// <param name='d'>The collection of data we want to use for our classification.</param>
/// <param name='p'>The datapoint we would like to classify.</param>
Classification KNN::Classify(DataChart* d, Datapoint* p)
{
	Classification result;

	// Find the index of the chart
	int chartIndex = std::distance(datacharts.begin(), std::find(datacharts.begin(), datacharts.end(), d));
	// Get a k-value from the pre-configured list
	uint16_t current_k = k_values[chartIndex];

	int valuesSize = d->GetValues()->size();
	// If there aren't enough points, return empty result
	if (valuesSize <= current_k)
		return result;

	int offset = valuesSize > WINDOW_SIZE ? valuesSize - WINDOW_SIZE : 0;
	// Fill the list of distances
	for (int i = 0; i < WINDOW_SIZE && i < valuesSize; ++i)
		distances[i] = Distance(p->position, d->GetValues()->at(offset + i)->position);

	// Sort distances
	std::sort(std::begin(distances), std::end(distances));

	// Get the k-distance
	float kDistance = distances[current_k];

	// Get the maximum index of our k-distances (first k steps are skipped, plus the first time there are no kDistances yet)
	int maxkDistanceIndex = fmin(WINDOW_SIZE, valuesSize - current_k - 1);
	// Only calculate anomaly score if we have observed more than one point
	if (maxkDistanceIndex > 1)
	{
		// Calculate the average distance of this window
		float averageDist = 0;
		for (int i = 0; i < maxkDistanceIndex; ++i)
			averageDist += kDistances.at(chartIndex)[i] / maxkDistanceIndex;

		// Calculate the standard deviation
		float stDev = 0;
		for (int i = 0; i < maxkDistanceIndex; ++i)
			stDev += pow(kDistances.at(chartIndex)[i] - averageDist, 2) / maxkDistanceIndex;
		stDev = sqrtf(stDev);

		// We flag it is outlier if the distance is at least one stDev removed from the average
		float deviation = kDistance - averageDist;
		bool isOutlier = deviation > stDev;

		// The certainty is the amount of standard deviations removed from the average (maxes out at 3 standard deviations)
		result.certainty = stDev > 0 ? std::clamp(deviation / (3 * stDev), 0.0f, 1.0f) : 1;
		// Result is anomalous if there is an outlier and we were still cooling down
		result.isAnomaly = ApplyCooldown(chartIndex, isOutlier);
	}

	// Add the average to the list
	int kIndex = kIndices[chartIndex];
	kDistances.at(chartIndex)[kIndex] = kDistance;
	// Increase the index
	kIndices[chartIndex] = (kIndex + 1) % WINDOW_SIZE;

	return result;
}

KNN::~KNN()
{
}

#endif
