#include "lof.h"

#if USE_LOF

/// <summary>Constructor for Local Outlier Factor.</summary>
/// <param name='k'>The amount of neighbours we want to use.</param>
LOF::LOF(uint16_t k_values[])
{
	this->k_values = k_values;
}

void LOF::SetData(std::vector<DataChart*> _datacharts)
{
	Detector::SetData(_datacharts);

	lofValues = std::vector<std::vector<float>>(datacharts.size());
	lofIndices = std::vector<int>(datacharts.size());
}

/// <summary>Classifies whether a datapoint is anomalous.</summary>
/// <param name='d'>The collection of data we want to use for our classification.</param>
/// <param name='p'>The datapoint we would like to classify.</param>
Classification LOF::Classify(DataChart * d, Datapoint* lof_p)
{
	Classification result;

	// Find the index of the chart
	int chartIndex = std::distance(datacharts.begin(), std::find(datacharts.begin(), datacharts.end(), d));
	// Get a k-value from the pre-configured list
	current_k = k_values[chartIndex];

	// If there aren't enough values, return empty
	if (d->GetValues()->size() <= 1)
		return result;

	// Get k-neighbours of p
	SetKNeighbours(d, lof_p);
	SetKDistance(d, lof_p);

	// Update the k-neighbours of every k-neighbour of p
	for (int i = 0; i < lof_p->neighbours.size(); ++i)
	{
		SetKNeighbours(d, lof_p->neighbours.at(i));
		SetKDistance(d, lof_p->neighbours.at(i));
	}

	// Calculate lrd(p) = |k-neighbours(p)| / SUM(reach-distances)
	SetLRD(d, lof_p);
	// Set the LRD for all the k-neighbours of p
	for (int i = 0; i < lof_p->neighbours.size(); ++i)
		SetLRD(d, lof_p->neighbours.at(i));

	// Calculate LOF(p) = SUM(lrd(o) / lrd(p)) / |k-neighbours(p)|
	float lofValue = GetLOF(d, lof_p);

	// Only calculate anomaly score if we have observed more than a few points
	if (lofValues.at(chartIndex).size() > 5)
	{
		// Calculate the average distance of this window
		float averageValue = 0;
		for (int i = 0; i < lofValues.at(chartIndex).size(); ++i)
			averageValue += lofValues.at(chartIndex)[i] / lofValues.at(chartIndex).size();

		// Calculate the standard deviation
		float stDev = 0;
		for (int i = 0; i < lofValues.at(chartIndex).size(); ++i)
			stDev += pow(lofValues.at(chartIndex)[i] - averageValue, 2) / lofValues.at(chartIndex).size();
		stDev = sqrtf(stDev);

		float deviation = lofValue - averageValue;
		// We flag it as outlier if the deviation is at least one stDev removed from the average
		result.isAnomaly = deviation > stDev;
		// The certainty increases exponentially until a distance of 3 standard deviations
		float deviationDistance = 1 - deviation / (3 * stDev);
		result.certainty = stDev > 0 ? std::clamp(exp(-5 * deviationDistance), 0.0f, 1.0f) : 1;
	}

	// Add the average to the list
	int kIndex = lofIndices[chartIndex];
	if (lofValues.at(chartIndex).size() < WINDOW_SIZE)
		lofValues.at(chartIndex).push_back(lofValue);
	else
		lofValues.at(chartIndex).at(kIndex) = lofValue;
	// Increase the index
	lofIndices[chartIndex] = (lofIndices[chartIndex] + 1) % WINDOW_SIZE;

	return result;
}

/// <summary>Sets the LOF value for a datapoint.</summary>
/// <param name='d'>The collection of data we want to use for our classification.</param>
/// <param name='p'>The datapoint we would like to set the LOF value for.</param>
float LOF::GetLOF(DataChart *d, Datapoint *p)
{
	float lrdSum = 0;
	for (int i = 0; i < p->neighbours.size(); ++i)
	{
		Datapoint* o = p->neighbours.at(i);
		lrdSum += (o->lrd / p->lrd);
	}

	float lof = lrdSum / p->neighbours.size();
	return lof;
}

/// <summary>Sets the LRD value for a datapoint.</summary>
/// <param name='d'>The collection of data we want to use for our classification.</param>
/// <param name='p'>The datapoint we would like to set the LRD value for.</param>
void LOF::SetLRD(DataChart *d, Datapoint *p)
{
	float reachDistSum = 0;
	for (int i = 0; i < p->neighbours.size(); ++i)
		reachDistSum += GetReachDistance(d, p, p->neighbours.at(i));

	// Ensure we don't divide by zero
	if (reachDistSum == 0)
		reachDistSum = 0.001;

	p->lrd = (p->neighbours.size()) / reachDistSum;
}

/// <summary>Calculates and returns the reach-distance from a point.</summary>
/// <param name='d'>The collection of data we want to use for our classification.</param>
/// <param name='p'>The point we want to calculate the reach-distance for.</param>
/// <param name='o'>The reference point used for calculating the reach distance.</param>
float LOF::GetReachDistance(DataChart *d, Datapoint* p, Datapoint* o)
{
	return fmax(Distance(p->position, o->position), o->kDistance);
}

/// <summary>Sets the k-distance(distance to the k-th negihbour) of a datapoint. </summary>
/// <param name='d'>The collection of data we want to use for our classification.</param>
/// <param name='p'>The datapoint we would like to set the k-distance for.</param>
void LOF::SetKDistance(DataChart *d, Datapoint *p)
{
	// Set the distance to the k-th nearest neighbour
	p->kDistance = Distance(p->neighbours.back()->position, p->position);
}

/// <summary>Sets the k-neighbours(closest k neighbours) of a datapoint. </summary>
/// <param name='d'>The collection of data we want to use for our classification.</param>
/// <param name='p'>The datapoint we would like to set the k-neighbours for.</param>
void LOF::SetKNeighbours(DataChart * d, Datapoint* p)
{
	std::vector<Datapoint*>* datapoints = d->GetValues();

	int startIndex = datapoints->size() > WINDOW_SIZE ? datapoints->size() - WINDOW_SIZE : 0;
	int endIndex = datapoints->size() > WINDOW_SIZE ? startIndex + WINDOW_SIZE : datapoints->size();
	// Calculate the distance to p for all the values
	std::vector<Datapoint*> lofDatapoints;
	for (int i = startIndex; i < endIndex; ++i)
	{
		// Get the position from the Datapoint
		Datapoint* lofDatapoint = datapoints->at(i);

		// Exclude p itself
		if (lofDatapoint == p)
			continue;

		//Set the distance of the datapoint
		lofDatapoint->distance = Distance(lofDatapoint->position, p->position);

		// Add it to the list
		lofDatapoints.push_back(lofDatapoint);
	}

	// Sort the values from smallest to greatest distance
	std::sort(lofDatapoints.begin(), lofDatapoints.end(),
		[p](const Datapoint * a, const Datapoint * b) -> bool
	{
		return a->distance < b->distance;
	});

	// Search for the k-th distance
	// Start distance at the k-th neighbour, since they're sorted this ensures we have at most k-1 neighbours whose distance is smaller
	float dist = lofDatapoints.at((int)fmin(lofDatapoints.size() - 1, current_k - 1))->distance;
	int nbrSize = lofDatapoints.size();

	// Add all distances equal to the current distance
	for (int i = current_k - 1; i < lofDatapoints.size(); ++i)
	{
		// We find a step in distance, stop adding
		if (lofDatapoints.at(i)->distance > dist)
			break;

		nbrSize = i + 1;
	}

	// Get the closest k-neighbours
	p->neighbours.resize(nbrSize);
	std::copy_n(lofDatapoints.begin(), nbrSize, p->neighbours.begin());
}

void LOF::UpdateKNeighbours(DataChart *d, Datapoint * p, Datapoint* new_p)
{
	float newDistance = Distance(p->position, new_p->position);
	// Check if the new_p should be included in the neighbours
	if (newDistance > p->kDistance)
		return;

	// Insert the new point into the neighbours of p
	for (int i = 0; i < p->neighbours.size(); ++i)
	{
		float dist = Distance(p->neighbours.at(i)->position, p->position);
		// When we find a distance greater than the new distance
		if (dist >= newDistance)
		{
			// Insert at this position
			p->neighbours.insert(p->neighbours.begin() + i, new_p);
			break;
		}
	}

	// Remove any neighbours outside the neighbourhood
	// Search for the k-th distance
	// Start distance at the k-th neighbour, since they're sorted this ensures we have at most k-1 neighbours whose distance is smaller
	float dist = p->neighbours.at(current_k - 1)->distance;

	// Add all distances equal to the current distance
	for (int i = current_k - 1; i < p->neighbours.size(); ++i)
	{
		// We find a step in distance, stop adding
		if (p->neighbours.at(i)->distance > dist)
		{
			// Cut off any neighbours outside the neighbourhood
			p->neighbours.resize(i);
			break;
		}
	}
}

LOF::~LOF()
{
}

#endif
