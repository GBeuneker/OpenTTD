#include "knn.h"

#if USE_KNN

/// <summary>Constructor for K-Nearest Neighbours.</summary>
/// <param name='k'>The amount of neighbours we want to use.</param>
KNN::KNN(uint16_t k_values[])
{
	this->k_values = k_values;
}

void KNN::SetData(std::vector<DataChart*> _datacharts)
{
	Detector::SetData(_datacharts);
	// Assign all the kDistances
	kDistances = std::vector<std::vector<float>>(datacharts.size());
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

	int offset = valuesSize > WINDOW_SIZE ? valuesSize - WINDOW_SIZE : 0;
	// Fill the list of distances
	distances.clear();
	for (int i = 0; i < WINDOW_SIZE && i < valuesSize; ++i)
		distances.push_back(Distance(p->position, d->GetValues()->at(offset + i)->position));

	// Sort distances
	std::sort(std::begin(distances), std::end(distances));

	// Get the k-distance
	float kDistance = distances[(int)fmin(distances.size() - 1, current_k)];

	// Only calculate anomaly score if we have observed more than a few points
	if (kDistances.at(chartIndex).size() > 5)
	{
		// Calculate the average distance of this window
		float averageDist = 0;
		for (int i = 0; i < kDistances.at(chartIndex).size(); ++i)
			averageDist += kDistances.at(chartIndex)[i] / kDistances.at(chartIndex).size();

		// Calculate the standard deviation
		float stDev = 0;
		for (int i = 0; i < kDistances.at(chartIndex).size(); ++i)
			stDev += pow(kDistances.at(chartIndex)[i] - averageDist, 2) / kDistances.at(chartIndex).size();
		stDev = sqrtf(stDev);

		float deviation = kDistance - averageDist;
		// We flag it is outlier if the distance is at least one stDev removed from the average
		result.isAnomaly = deviation > stDev;
		// The certainty increases exponentially until a distance of 3 standard deviations
		float deviationDistance = deviation / (3 * stDev);
		result.certainty = stDev > 0 ? Sigmoid(deviationDistance) : 1;
	}

	return result;
}

void KNN::Train(DataChart * d, Datapoint * p)
{
	// Find the index of the chart
	int chartIndex = std::distance(datacharts.begin(), std::find(datacharts.begin(), datacharts.end(), d));
	// Get a k-value from the pre-configured list
	uint16_t current_k = k_values[chartIndex];

	int valuesSize = d->GetValues()->size();

	int offset = valuesSize > WINDOW_SIZE ? valuesSize - WINDOW_SIZE : 0;
	// Fill the list of distances
	distances.clear();
	for (int i = 0; i < WINDOW_SIZE && i < valuesSize; ++i)
		distances.push_back(Distance(p->position, d->GetValues()->at(offset + i)->position));

	// Sort distances
	std::sort(std::begin(distances), std::end(distances));

	// Get the k-distance
	float kDistance = distances[(int)fmin(distances.size() - 1, current_k)];

	// Add the average to the list
	int kIndex = kIndices[chartIndex];
	if (kDistances.at(chartIndex).size() < WINDOW_SIZE)
		kDistances.at(chartIndex).push_back(kDistance);
	else
		kDistances.at(chartIndex).at(kIndex) = kDistance;
	// Increase the index
	kIndices[chartIndex] = (kIndex + 1) % WINDOW_SIZE;
}

KNN::~KNN()
{
}

#endif
