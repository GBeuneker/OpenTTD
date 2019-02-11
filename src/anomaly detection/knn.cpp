#include "knn.h"

/// <summary>Constructor for K-Nearest Neighbours.</summary>
/// <param name='k'>The amount of neighbours we want to use.</param>
KNN::KNN(uint16_t k)
{
	this->k = k;
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

	// Find the index of the chart
	int chartIndex = std::distance(datacharts.begin(), std::find(datacharts.begin(), datacharts.end(), d));

	// Get the k-th distance and add it to the list
	float kDistance = distances[k];
	int kIndex = kIndices[chartIndex];
	kDistances.at(chartIndex)[kIndex] = kDistance;
	kIndices[chartIndex] = (kIndex + 1) % WINDOW_SIZE;
	int maxIndex = fmin(WINDOW_SIZE, valuesSize - k);

	// Calculate the average distance of this window
	float averageDist = 0;
	for (int i = 0; i < maxIndex; ++i)
		averageDist += kDistances.at(chartIndex)[i];
	averageDist /= maxIndex;

	// Calculate the standard deviation
	float stDev = 0;
	for (int i = 0; i < maxIndex; ++i)
		stDev += pow(kDistances.at(chartIndex)[i] - averageDist, 2);
	stDev = sqrtf(stDev / maxIndex);

	// We flag it is outlier if the distance is at least one stDev removed from the average
	bool isOutlier = kDistance > (averageDist + stDev);
	if (isOutlier)
	{
		// If we aren't cooling down, start the cooldown and don't flag as outlier
		if (cooldownSteps[chartIndex] <= 0)
		{
			cooldownSteps[chartIndex] = cooldownSize;
			isOutlier = false;
		}
		// If we were still cooling down, flag as outlier
		else if (cooldownSteps[chartIndex] > 0)
		{
			isOutlier = true;
			cooldownSteps[chartIndex] = 0;
		}
	}
	// If there is no outlier, decrease the cooldown
	else
		cooldownSteps[chartIndex]--;

	// Result is anomalous if there is an outlier and we were still cooling down
	result.isAnomaly = isOutlier;
	// The certainty is the amount of standard deviations removed from the average (maxes out at 4 standard deviations)
	result.certainty = stDev > 0 ? fmin(abs(kDistance - averageDist) / (4 * stDev), 1) : 1;

	return result;
}

KNN::~KNN()
{
}
