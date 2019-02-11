#include "lof.h"

/// <summary>Constructor for Local Outlier Factor.</summary>
/// <param name='k'>The amount of neighbours we want to use.</param>
LOF::LOF(uint16_t k)
{
	this->k = k;
}

void LOF::SetData(std::vector<DataChart*> _datacharts)
{
	Detector::SetData(_datacharts);

	lofValues= std::vector<float[WINDOW_SIZE]>(datacharts.size());
	lofIndices = std::vector<int>(datacharts.size());
}

/// <summary>Classifies whether a datapoint is anomalous.</summary>
/// <param name='d'>The collection of data we want to use for our classification.</param>
/// <param name='p'>The datapoint we would like to classify.</param>
Classification LOF::Classify(DataChart * d, Datapoint p)
{
	Classification result;

	if (d->GetValues()->size() <= k)
		return result;

	// Convert datapoint to lof_datapoint
	LOF_Datapoint lof_p = LOF_Datapoint(p.position.X, p.position.Y);
	// Get k-neighbours of p
	SetKNeighbours(d, &lof_p);

	// Set the k-neighbours of every k-neighbour of p
	for (int i = 0; i < lof_p.kNeighbours.size(); ++i)
	{
		// Set the k-neighbours of o
		SetKNeighbours(d, &lof_p.kNeighbours[i]);
		// Set the k-distance of o
		SetKDistance(d, &lof_p.kNeighbours[i]);
	}

	// Calculate lrd(p) = |k-neighbours(p)| / SUM(reach-distances)
	SetLRD(d, &lof_p);
	// Set the LRD for all the k-neighbours of p
	for (int i = 0; i < lof_p.kNeighbours.size(); ++i)
		SetLRD(d, &lof_p.kNeighbours[i]);

	// Calculate LOF(p) = SUM(lrd(o) / lrd(p)) / |k-neighbours(p)|
	SetLOF(d, &lof_p);

	// Find the index of the chart
	int chartIndex = std::distance(datacharts.begin(), std::find(datacharts.begin(), datacharts.end(), d));

	float lofValue = lof_p.lof;
	lofValues.at(chartIndex)[lofIndices[chartIndex]] = lofValue;
	lofIndices[chartIndex] = (lofIndices[chartIndex] + 1) % WINDOW_SIZE;
	int maxIndex = fmin(WINDOW_SIZE, d->GetValues()->size() - k);

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

	// We flag it as anomalous if the distance is at least one stDev removed from the average
	result.isAnomaly = lofValue > (averageValue + stDev);
	// The certainty is the amount of standard deviations removed from the average (maxes out at 4 standard deviations)
	if (stDev <= 0)
		result.certainty = 1;
	else
		result.certainty = fmin(abs(lofValue - averageValue) / (4 * stDev), 1);

	return result;
}

/// <summary>Sets the LOF value for a datapoint.</summary>
/// <param name='d'>The collection of data we want to use for our classification.</param>
/// <param name='p'>The datapoint we would like to set the LOF value for.</param>
void LOF::SetLOF(DataChart *d, LOF_Datapoint *p)
{
	float lrdSum = 0;
	for (int i = 0; i < p->kNeighbours.size(); ++i)
	{
		LOF_Datapoint o = p->kNeighbours[i];
		lrdSum += (o.lrd / p->lrd);
	}

	float lof = lrdSum / p->kNeighbours.size();
	p->lof = lof;
}

/// <summary>Sets the LRD value for a datapoint.</summary>
/// <param name='d'>The collection of data we want to use for our classification.</param>
/// <param name='p'>The datapoint we would like to set the LRD value for.</param>
void LOF::SetLRD(DataChart *d, LOF_Datapoint *p)
{
	float reachDistSum = 0;
	for (int i = 0; i < p->kNeighbours.size(); ++i)
		reachDistSum += GetReachDistance(d, *p, p->kNeighbours[i]);

	// Ensure we don't divide by zero
	if (reachDistSum == 0)
		reachDistSum = 0.000001f;

	p->lrd = (p->kNeighbours.size()) / reachDistSum;
}

/// <summary>Calculates and returns the reach-distance from a point.</summary>
/// <param name='d'>The collection of data we want to use for our classification.</param>
/// <param name='p'>The point we want to calculate the reach-distance for.</param>
/// <param name='o'>The reference point used for calculating the reach distance.</param>
float LOF::GetReachDistance(DataChart *d, LOF_Datapoint p, LOF_Datapoint o)
{
	return fmax(Distance(p.position, o.position), o.kDistance);
}

/// <summary>Sets the k-distance(distance to the k-th negihbour) of a datapoint. </summary>
/// <param name='d'>The collection of data we want to use for our classification.</param>
/// <param name='p'>The datapoint we would like to set the k-distance for.</param>
void LOF::SetKDistance(DataChart *d, LOF_Datapoint *p)
{
	// Set the distance to the k-th nearest neighbour
	p->kDistance = Distance(p->kNeighbours.back().position, p->position);
}

/// <summary>Sets the k-neighbours(closest k neighbours) of a datapoint. </summary>
/// <param name='d'>The collection of data we want to use for our classification.</param>
/// <param name='p'>The datapoint we would like to set the k-neighbours for.</param>
void LOF::SetKNeighbours(DataChart * d, LOF_Datapoint* p)
{
	std::vector<Datapoint> datapoints = *(d->GetValues());

	int startIndex = datapoints.size() > WINDOW_SIZE ? datapoints.size() - WINDOW_SIZE : 0;
	int endIndex = datapoints.size() > WINDOW_SIZE ? startIndex + WINDOW_SIZE : datapoints.size();
	// Calculate the distance to p for all the values
	for (int i = startIndex; i < endIndex; ++i)
	{
		// Get the position from the Datapoint
		Vector2 pos = datapoints[i].position;
		float dist = Distance(datapoints[i].position, p->position);
		// Convert to a LOF_DataPoint
		LOF_Datapoint lofDatapoint = LOF_Datapoint(pos.X, pos.Y);
		lofDatapoint.distance = dist;

		// Add it to the list
		lofDatapoints[i - startIndex] = lofDatapoint;
	}

	// Sort the values from smallest to greatest distance
	std::sort(lofDatapoints, lofDatapoints + WINDOW_SIZE);

	// Get the closest k-neighbours
	std::vector<LOF_Datapoint> kNeighbours(k);
	std::copy_n(lofDatapoints, k, kNeighbours.begin());
	// Set the k-neighbours of the datapoint p
	p->kNeighbours = kNeighbours;
}

LOF::~LOF()
{
}
