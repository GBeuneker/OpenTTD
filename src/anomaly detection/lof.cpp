#include "lof.h"

#if USE_LOF

/// <summary>Constructor for Local Outlier Factor.</summary>
/// <param name='k'>The amount of neighbours we want to use.</param>
LOF::LOF(uint16_t k)
{
	this->k = k;
}

void LOF::SetData(std::vector<DataChart*> _datacharts)
{
	Detector::SetData(_datacharts);

	lofValues = std::vector<float[WINDOW_SIZE]>(datacharts.size());
	lofIndices = std::vector<int>(datacharts.size());
}

/// <summary>Classifies whether a datapoint is anomalous.</summary>
/// <param name='d'>The collection of data we want to use for our classification.</param>
/// <param name='p'>The datapoint we would like to classify.</param>
Classification LOF::Classify(DataChart * d, Datapoint* lof_p)
{
	Classification result;

	if (d->GetValues()->size() <= k)
		return result;

	// Get k-neighbours of p
	SetKNeighbours(d, lof_p);
	SetKDistance(d, lof_p);

	// Update the k-neighbours of every k-neighbour of p
	for (int i = 0; i < lof_p->neighbours.size(); ++i)
	{
		// If the k-distance has not yet been reached for the point
		if (!lof_p->neighbours.at(i)->kDistanceReached)
			SetKNeighbours(d, lof_p->neighbours.at(i));
		// If k-distance has already been reached, just update the neighbourhood with this new point
		else
			UpdateKNeighbours(d, lof_p->neighbours.at(i), lof_p);

		SetKDistance(d, lof_p->neighbours.at(i));
	}

	// Calculate lrd(p) = |k-neighbours(p)| / SUM(reach-distances)
	SetLRD(d, lof_p);
	// Set the LRD for all the k-neighbours of p
	for (int i = 0; i < lof_p->neighbours.size(); ++i)
		SetLRD(d, lof_p->neighbours.at(i));

	// Calculate LOF(p) = SUM(lrd(o) / lrd(p)) / |k-neighbours(p)|
	float lofValue = GetLOF(d, lof_p);

	// Find the index of the chart
	int chartIndex = std::distance(datacharts.begin(), std::find(datacharts.begin(), datacharts.end(), d));

	// Get the maximum index of our lof-values (first k steps are skipped, plus the first time there are no lof-values yet)
	int maxIndex = fmin(WINDOW_SIZE, d->GetValues()->size() - k - 1);

	// Calculate the average distance of this window
	float averageValue = 0;
	for (int i = 0; i < maxIndex; ++i)
		averageValue += lofValues.at(chartIndex)[i];
	averageValue /= maxIndex;

	// Calculate the standard deviation
	float stDev = 0;
	for (int i = 0; i < maxIndex; ++i)
		stDev += pow(lofValues.at(chartIndex)[i] - averageValue, 2);
	stDev = sqrtf(stDev / maxIndex);


	// We flag it as outlier if the distance is at least one stDev removed from the average
	bool isOutlier = lofValue > (averageValue + stDev);
	isOutlier = ApplyCooldown(chartIndex, isOutlier);

	float threshold = averageValue + stDev;
	result.isAnomaly = lofValue > threshold;
	// The certainty is the amount of standard deviations removed from the average (maxes out at 4 standard deviations)
	if (stDev <= 0)
		result.certainty = 1;
	else
		result.certainty = fmin(abs(lofValue - threshold) / (4 * stDev), 1);

	// Add the average to the list
	int kIndex = lofIndices[chartIndex];
	lofValues.at(chartIndex)[kIndex] = lofValue;
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
	float dist = 0;
	int nbrSize = lofDatapoints.size(), distSteps = 0;
	for (int i = 0; i < lofDatapoints.size(); ++i)
	{
		// If we find a step in distance
		if (lofDatapoints.at(i)->distance > dist)
		{
			// Remember the last observed distance
			dist = lofDatapoints.at(i)->distance;
			// If we haven't reached k-th nearest neighbour yet, take a step
			if (distSteps < k)
				distSteps++;
			// We have reached the k-th nearest neighbour and found the next step	
			else
			{
				// Size is equal to i
				nbrSize = i;
				p->kDistanceReached = true;
				break;
			}
		}
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
}

LOF::~LOF()
{
}

#endif
