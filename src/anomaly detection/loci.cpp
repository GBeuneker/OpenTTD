#include "loci.h"

#if USE_LOCI

LOCI::LOCI()
{
}

/// <summary>Classifies whether a datapoint is anomalous.</summary>
/// <param name='d'>The collection of data we want to use for our classification.</param>
/// <param name='p'>The datapoint we would like to classify.</param>
Classification LOCI::Classify(DataChart * d, Datapoint* loci_p)
{
	Classification result;

	// Determine rmin, rmax and the stepsize
	int steps = 10;
	float maxRange = Distance(d->GetMinValue(), d->GetMaxValue());
	int rMin = maxRange / 100, rMax = maxRange;
	float stepSize = (rMax - rMin) / (float)steps;

	// Find the index of the chart
	int chartIndex = std::distance(datacharts.begin(), std::find(datacharts.begin(), datacharts.end(), d));

	// Set the neighbours in range k of p
	SetRNeighbours(d, loci_p, &loci_p->kNeighbours, k);
	for (float r = rMax; r > rMin; r -= stepSize)
	{
		if (r == rMax)
		{
			// Set the neighbours and neighbourhood of p
			SetRNeighbours(d, loci_p, &loci_p->neighbours, r);
			SetRNeighbours(d, loci_p, &loci_p->krNeighbours, k*r);
			SetRNeighbourhood(d, loci_p, r, k);
		}
		else
		{
			UpdateRNeighbours(loci_p, &loci_p->neighbours, r);
			UpdateRNeighbours(loci_p, &loci_p->krNeighbours, k*r);
			UpdateRNeighbourhood(d, loci_p, r, k);
		}

		// Get the MDEF value for p for this range r
		float mdef = GetMDEF(d, loci_p, r, k);

		// Get the standard deviation for this range r
		float s_mdef = GetStandardDeviationMDEF(d, loci_p, r, k);

		// Point is flagged as anomalous if the mdef is greater than the standard deviation
		float threshold = l * s_mdef;
		bool outlier = mdef > threshold;
		result.isAnomaly = mdef > threshold;
		// The certainty is the amount of standard deviations removed (maxes out at 4 standard deviations)
		if (s_mdef <= 0)
			result.certainty = 1;
		else
			result.certainty = fmin((mdef - threshold) / (4 * threshold), 1);

		// Break if for any range we have found an anomaly
		if (result.isAnomaly)
			break;
	}

	// Apply a cooldown to the result
	result.isAnomaly = ApplyCooldown(chartIndex, result.isAnomaly);

	return result;
}

/// <summary>Calculates and returns the MDEF value of a datapoint.</summary>
/// <param name='d'>The collection of data we want to use for our calculation.</param>
/// <param name='p'>The reference datapoint we would like to use.</param>
/// <param name='r'>The range we want to use.</param>
/// <param name='k'>The neighbourhood size.</param>
float LOCI::GetMDEF(DataChart * d, Datapoint * p, float r, float k)
{
	float prk_neighbourhood = p->prkNeighbourhood;
	float pkr_neighbourCount = p->krNeighbours.size();
	float pkr_neighbourhood = GetRNeighbourhood(&p->kNeighbours);

	return (prk_neighbourhood - pkr_neighbourCount) / pkr_neighbourhood;
}

/// <summary>Calculates and returns the standard deviation of the MDEF.</summary>
/// <param name='d'>The collection of data we want to use for our calculation.</param>
/// <param name='p'>The reference datapoint we would like to use.</param>
/// <param name='r'>The range we want to use.</param>
/// <param name='k'>The neighbourhood size.</param>
float LOCI::GetStandardDeviationMDEF(DataChart * d, Datapoint * p, float r, float k)
{
	return GetSigma(d, p, r, k) / p->prkNeighbourhood;
}

/// <summary>Calculates and returns the sigma values for the standard deviations of the MDEF.</summary>
/// <param name='d'>The collection of data we want to use for our calculation.</param>
/// <param name='p'>The reference datapoint we would like to use.</param>
/// <param name='r'>The range we want to use.</param>
/// <param name='k'>The neighbourhood size.</param>
float LOCI::GetSigma(DataChart * d, Datapoint * p, float r, float k)
{
	float stdevSum = 0;

	int pkNeighbourCount = 0;
	for (int i = 0; i < p->neighbours.size(); ++i)
	{
		pkNeighbourCount = p->neighbours[i]->krNeighbours.size();
		stdevSum += powf((pkNeighbourCount - p->prkNeighbourhood), 2);
	}

	return sqrtf(stdevSum / p->neighbours.size());
}

#pragma region R-Neighbours

/// <summary>Sets the r-neighbours(neighbours within range r) of a datapoint.</summary>
/// <param name='d'>The collection of data we want to use for our calculation.</param>
/// <param name='p'>The reference datapoint we would like to use.</param>
/// <param name='r'>The range we want to use.</param>
void LOCI::SetRNeighbours(DataChart * d, Datapoint * p, std::vector<Datapoint*>* neighbours, float r)
{
	std::vector<Datapoint*> datapoints = *(d->GetValues());

	int startIndex = datapoints.size() > WINDOW_SIZE ? datapoints.size() - WINDOW_SIZE : 0;
	int endIndex = datapoints.size() > WINDOW_SIZE ? startIndex + WINDOW_SIZE : datapoints.size();

	neighbours->clear();
	// Calculate the distance to p for all the values
	for (int i = startIndex; i < endIndex; ++i)
	{
		// Get the position from the Datapoint
		float dist = Distance(datapoints[i]->position, p->position);
		// Add a neighbour to p
		if (dist <= r)
			neighbours->push_back(datapoints[i]);
	}
}

/// <summary>Sets the r-neighbours(neighbours within range r) of a datapoint.</summary>
/// <param name='d'>The collection of data we want to use for our calculation.</param>
/// <param name='p'>The reference datapoint we would like to use.</param>
/// <param name='r'>The range we want to use.</param>
void LOCI::UpdateRNeighbours(Datapoint * p, std::vector<Datapoint*>* neighbours, float r)
{
	// Remove all elements outside the radius r
	neighbours->erase(
		std::remove_if(neighbours->begin(),
			neighbours->end(),
			[p, r](const Datapoint * a) -> bool { return Distance(a->position, p->position) > r; }),
		neighbours->end());
}

#pragma endregion

#pragma region R-Neighbourhood

/// <summary>Sets the r-neighbourhood(average amount of r-neighbours in its neighbourhood) of a datapoint.</summary>
/// <param name='d'>The collection of data we want to use for our calculation.</param>
/// <param name='p'>The reference datapoint we would like to use.</param>
/// <param name='r'>The range we want to use.</param>
void LOCI::SetRNeighbourhood(DataChart * d, Datapoint * p, float r, float k)
{
	float sum_rNeighbours = 0;

	// Sum the r-neighbour count for all the points in the neighbourhood of p
	for (int i = 0; i < p->neighbours.size(); ++i)
	{
		// Make sure we do not edit the r-neighbours of p itself
		if (p->neighbours[i] == p)
			sum_rNeighbours += p->krNeighbours.size();
		else
		{
			Datapoint* o = p->neighbours[i];
			SetRNeighbours(d, o, &o->krNeighbours, k*r);
			sum_rNeighbours += o->krNeighbours.size();
		}
	}

	// Calculate the average over all the neighbours
	p->prkNeighbourhood = sum_rNeighbours / p->neighbours.size();
}

/// <summary>Sets the r-neighbourhood(average amount of r-neighbours in its neighbourhood) of a datapoint.</summary>
/// <param name='d'>The collection of data we want to use for our calculation.</param>
/// <param name='p'>The reference datapoint we would like to use.</param>
/// <param name='r'>The range we want to use.</param>
void LOCI::UpdateRNeighbourhood(DataChart * d, Datapoint * p, float r, float k)
{
	float sum_rNeighbours = 0;

	// Sum the r-neighbour count for all the points in the neighbourhood of p
	for (int i = 0; i < p->neighbours.size(); ++i)
	{
		// Make sure we do not edit the r-neighbours of p itself
		if (p->neighbours[i] == p)
			sum_rNeighbours += p->krNeighbours.size();
		else
		{
			Datapoint* o = p->neighbours[i];
			UpdateRNeighbours(o, &o->krNeighbours, k*r);
			sum_rNeighbours += o->krNeighbours.size();
		}
	}

	// Calculate the average over all the neighbours
	p->prkNeighbourhood = sum_rNeighbours / p->neighbours.size();
}

/// <summary>Calculates and returns the r-neighbourhood(average amount of r-neighbours in its neighbourhood) of a datapoint.</summary>
/// <param name='d'>The collection of data we want to use for our calculation.</param>
/// <param name='p'>The reference datapoint we would like to use.</param>
/// <param name='r'>The range we want to use.</param>
float LOCI::GetRNeighbourhood(std::vector<Datapoint*>* neighbours)
{
	float sum_rNeighbours = 0;

	// Sum the r-neighbour count for all the points in the neighbourhood of p
	for (int i = 0; i < neighbours->size(); ++i)
		sum_rNeighbours += neighbours->at(i)->krNeighbours.size();

	// Calculate the average over all the neighbours
	return sum_rNeighbours / neighbours->size();
}

#pragma endregion

LOCI::~LOCI()
{
}

#endif
