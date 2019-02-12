#include "loci.h"

#if USE_LOCI

LOCI::LOCI()
{
}

/// <summary>Classifies whether a datapoint is anomalous.</summary>
/// <param name='d'>The collection of data we want to use for our classification.</param>
/// <param name='p'>The datapoint we would like to classify.</param>
Classification LOCI::Classify(DataChart * d, Datapoint p)
{
	Classification result;

	// Convert datapoint to Datapoint
	Datapoint loci_p = Datapoint(p.position.X, p.position.Y);
	// Determine rmin, rmax and the stepsize
	int steps = 10;
	float maxRange = Distance(d->GetMinValue(), d->GetMaxValue());
	int rMin = maxRange / 100, rMax = maxRange;
	float stepSize = (rMax - rMin) / steps;

	for (float r = rMin; r < rMax; r += stepSize)
	{
		// Set the neighbours and neighbourhood of p
		SetRNeighbours(d, &loci_p, r);
		SetRNeighbourhood(d, &loci_p, r, k);

		// Get the MDEF value for p for this range r
		float mdef = GetMDEF(d, &loci_p, r, k);

		// Get the standard deviation for this range r
		float s_mdef = GetStandardDeviationMDEF(d, &loci_p, r, k);

		// Point is flagged as anomalous if the mdef is greater than the standard deviation
		result.isAnomaly = mdef > l * s_mdef;
		// The certainty is the amount of standard deviations removed (maxes out at 4 standard deviations)
		if (s_mdef <= 0)
			result.certainty = 1;
		else
			result.certainty = fmin(mdef / (4 * l * s_mdef), 1);

		// Break if for any range we have found an anomaly
		if (result.isAnomaly)
			break;
	}

	return result;
}

/// <summary>Calculates and returns the MDEF value of a datapoint.</summary>
/// <param name='d'>The collection of data we want to use for our calculation.</param>
/// <param name='p'>The reference datapoint we would like to use.</param>
/// <param name='r'>The range we want to use.</param>
/// <param name='k'>The neighbourhood size.</param>
float LOCI::GetMDEF(DataChart * d, Datapoint * p, float r, float k)
{
	float prk_neighbourhood = p->rNeighbourhood;
	float pkr_neighbours = GetRNeighbourCount(d, p, k*r);
	float pkr_neighbourhood = GetRNeighbourhood(d, p, k, r);

	return (prk_neighbourhood - pkr_neighbours) / pkr_neighbourhood;
}

/// <summary>Calculates and returns the standard deviation of the MDEF.</summary>
/// <param name='d'>The collection of data we want to use for our calculation.</param>
/// <param name='p'>The reference datapoint we would like to use.</param>
/// <param name='r'>The range we want to use.</param>
/// <param name='k'>The neighbourhood size.</param>
float LOCI::GetStandardDeviationMDEF(DataChart * d, Datapoint * p, float r, float k)
{
	return GetSigma(d, p, r, k) / p->rNeighbourhood;
}

/// <summary>Calculates and returns the sigma values for the standard deviations of the MDEF.</summary>
/// <param name='d'>The collection of data we want to use for our calculation.</param>
/// <param name='p'>The reference datapoint we would like to use.</param>
/// <param name='r'>The range we want to use.</param>
/// <param name='k'>The neighbourhood size.</param>
float LOCI::GetSigma(DataChart * d, Datapoint * p, float r, float k)
{
	float stdevSum = 0;

	for (int i = 0; i < p->neighbours.size(); ++i)
	{
		Datapoint o = p->neighbours[i];
		stdevSum += powf((o.neighbours.size() - p->rNeighbourhood), 2);
	}

	return sqrtf(stdevSum / p->neighbours.size());
}

#pragma region R-Neighbours

/// <summary>Sets the r-neighbours(neighbours within range r) of a datapoint.</summary>
/// <param name='d'>The collection of data we want to use for our calculation.</param>
/// <param name='p'>The reference datapoint we would like to use.</param>
/// <param name='r'>The range we want to use.</param>
void LOCI::SetRNeighbours(DataChart * d, Datapoint * p, float r)
{
	std::vector<Datapoint> datapoints = *(d->GetValues());

	int startIndex = datapoints.size() > WINDOW_SIZE ? datapoints.size() - WINDOW_SIZE : 0;
	int endIndex = datapoints.size() > WINDOW_SIZE ? startIndex + WINDOW_SIZE : datapoints.size();

	std::vector<Datapoint> rNeighbours;
	// Calculate the distance to p for all the values
	for (int i = startIndex; i < endIndex; ++i)
	{
		// Get the position from the Datapoint
		Vector2 pos = datapoints[i].position;
		float dist = Distance(datapoints[i].position, p->position);
		if (dist <= r)
		{
			// Convert to a Datapoint
			Datapoint lociDatapoint = Datapoint(pos.X, pos.Y);
			rNeighbours.push_back(lociDatapoint);
		}
	}

	p->neighbours = rNeighbours;
}

/// <summary>Calculates and returns the r-neighbours(neighbours within range r) of a datapoint.</summary>
/// <param name='d'>The collection of data we want to use for our calculation.</param>
/// <param name='p'>The reference datapoint we would like to use.</param>
/// <param name='r'>The range we want to use.</param>
std::vector<Datapoint> LOCI::GetRNeighbours(DataChart * d, Datapoint * p, float r)
{
	std::vector<Datapoint> datapoints = *(d->GetValues());

	int startIndex = datapoints.size() > WINDOW_SIZE ? datapoints.size() - WINDOW_SIZE : 0;
	int endIndex = datapoints.size() > WINDOW_SIZE ? startIndex + WINDOW_SIZE : datapoints.size();

	std::vector<Datapoint> rNeighbours;
	// Calculate the distance to p for all the values
	for (int i = startIndex; i < endIndex; ++i)
	{
		// Get the position from the Datapoint
		Vector2 pos = datapoints[i].position;
		float dist = Distance(datapoints[i].position, p->position);
		if (dist <= r)
		{
			// Convert to a Datapoint
			Datapoint lociDatapoint = Datapoint(pos.X, pos.Y);
			rNeighbours.push_back(lociDatapoint);
		}
	}

	return rNeighbours;
}

/// <summary>Calculates and returns the amount of r-neighbours(neighbours within range r) of a datapoint.</summary>
/// <param name='d'>The collection of data we want to use for our calculation.</param>
/// <param name='p'>The reference datapoint we would like to use.</param>
/// <param name='r'>The range we want to use.</param>
int LOCI::GetRNeighbourCount(DataChart * d, Datapoint * p, float r)
{
	std::vector<Datapoint> datapoints = *(d->GetValues());

	int startIndex = datapoints.size() > WINDOW_SIZE ? datapoints.size() - WINDOW_SIZE : 0;
	int endIndex = datapoints.size() > WINDOW_SIZE ? startIndex + WINDOW_SIZE : datapoints.size();

	int count = 0;
	// Calculate the distance to p for all the values
	for (int i = startIndex; i < endIndex; ++i)
	{
		// Get the position from the Datapoint
		Vector2 pos = datapoints[i].position;
		float dist = Distance(datapoints[i].position, p->position);
		if (dist <= r)
			count++;
	}

	return count;
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
		Datapoint o = p->neighbours[i];
		SetRNeighbours(d, &o, k*r);
		sum_rNeighbours += o.neighbours.size();
	}

	// Calculate the average over all the neighbours
	p->rNeighbourhood = sum_rNeighbours / p->neighbours.size();
}

/// <summary>Calculates and returns the r-neighbourhood(average amount of r-neighbours in its neighbourhood) of a datapoint.</summary>
/// <param name='d'>The collection of data we want to use for our calculation.</param>
/// <param name='p'>The reference datapoint we would like to use.</param>
/// <param name='r'>The range we want to use.</param>
float LOCI::GetRNeighbourhood(DataChart * d, Datapoint * p, float r, float k)
{
	float sum_rNeighbours = 0;

	// Get the neighbours of p in range r
	std::vector<Datapoint> rNeighbours = GetRNeighbours(d, p, r);

	// Sum the r-neighbour count for all the points in the neighbourhood of p
	for (int i = 0; i < rNeighbours.size(); ++i)
	{
		Datapoint o = rNeighbours[i];
		sum_rNeighbours += GetRNeighbourCount(d, &o, k*r);
	}

	// Calculate the average over all the neighbours
	return sum_rNeighbours / rNeighbours.size();
}

#pragma endregion

LOCI::~LOCI()
{
}

#endif
