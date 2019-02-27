#include "loci.h"

#if USE_LOCI

LOCI::LOCI()
{
}

LOCI::LOCI(uint16_t nbrValues[])
{
	nbrRange_values = nbrValues;
}

/// <summary>Classifies whether a datapoint is anomalous.</summary>
/// <param name='d'>The collection of data we want to use for our classification.</param>
/// <param name='p'>The datapoint we would like to classify.</param>
Classification LOCI::Classify(DataChart * d, Datapoint* loci_p)
{
	Classification result;

	// Find the index of the chart
	int chartIndex = std::distance(datacharts.begin(), std::find(datacharts.begin(), datacharts.end(), d));
	// Get a range-value from the pre-configured list
	uint16_t maxNeighbourRange = nbrRange_values[chartIndex];

	// Determine rmin, rmax and the stepsize
	int steps = 100;
	float rMax = GetRadius(d, loci_p, maxNeighbourRange), rMin = 0;
	float stepSize = (rMax - rMin) / (float)steps;

	// Set the neighbours and neighbourhood of p
	SetRNeighbours(d, loci_p, &loci_p->neighbours, rMax);
	loci_p->tmp_neighbours = loci_p->neighbours;
	SetRNeighbours(d, loci_p, &loci_p->rkNeighbours, k*rMax);
	loci_p->tmp_rkNeighbours = loci_p->rkNeighbours;
	SetRNeighbourhood(d, loci_p, rMax, k);

	for (float r = rMax; r > rMin; r -= stepSize)
	{
		// Update the neighbours and neighbourhood of p
		UpdateRNeighbours(loci_p, &loci_p->tmp_neighbours, r);
		UpdateRNeighbours(loci_p, &loci_p->tmp_rkNeighbours, k*r);
		UpdateRNeighbourhood(d, loci_p, r, k);

		// Get the MDEF value for p for this range r
		float mdef = GetMDEF(d, loci_p);

		// Get the standard deviation for this range r
		float s_mdef = GetStandardDeviationMDEF(d, loci_p);

		// Point is flagged as anomalous if the mdef is greater than the standard deviation
		bool outlier = mdef > s_mdef;

		// Remember the highest certainty
		if (outlier)
		{
			result.isAnomaly = true;
			// The certainty is the amount of standard deviations removed. 100% certain at l standard deviations
			float certainty = s_mdef > 0 ? fmin(mdef / (l * s_mdef), 1) : 1;
			result.certainty = fmax(result.certainty, certainty);
		}
	}

	// Apply a cooldown to the result
	result.isAnomaly = ApplyCooldown(chartIndex, result.isAnomaly);

	return result;
}

/// <summary>Calculates the maximum radius of the dataset.</summary>
/// <param name='d'>The collection of data we want to use for our calculation.</param>
/// <param name='nbrAmount'>The amount of nearest neighbours we are looking for.</param>
float LOCI::GetRadius(DataChart *d, Datapoint *p, uint16_t nbrAmount)
{
	int startIndex = d->GetValues()->size() > WINDOW_SIZE ? d->GetValues()->size() - WINDOW_SIZE : 0;
	int endIndex = d->GetValues()->size() > WINDOW_SIZE ? startIndex + WINDOW_SIZE : d->GetValues()->size();

	float minX = FLT_MAX, maxX = FLT_MIN;
	float minY = FLT_MAX, maxY = FLT_MIN;

	std::vector<Datapoint*> values;

	// Get the minimum and maximum x,y values
	for (int i = startIndex; i < endIndex; ++i)
		values.push_back(d->GetValues()->at(i));

	// Sort the values from smallest to greatest distance
	std::sort(values.begin(), values.end(),
		[p](const Datapoint * a, const Datapoint * b) -> bool
	{
		return Distance(a->position, p->position) < Distance(b->position, p->position);
	});

	return Distance(p->position, values.at(fmin(values.size() - 1, nbrAmount))->position);
}

/// <summary>Calculates and returns the MDEF value of a datapoint.</summary>
/// <param name='d'>The collection of data we want to use for our calculation.</param>
/// <param name='p'>The reference datapoint we would like to use.</param>
/// <param name='r'>The range we want to use.</param>
/// <param name='k'>The neighbourhood size.</param>
float LOCI::GetMDEF(DataChart * d, Datapoint * p)
{
	return  1 - (p->tmp_rkNeighbours.size() / p->prkNeighbourhood);
}

/// <summary>Calculates and returns the standard deviation of the MDEF.</summary>
/// <param name='d'>The collection of data we want to use for our calculation.</param>
/// <param name='p'>The reference datapoint we would like to use.</param>
/// <param name='r'>The range we want to use.</param>
/// <param name='k'>The neighbourhood size.</param>
float LOCI::GetStandardDeviationMDEF(DataChart * d, Datapoint * p)
{
	return GetSigma(d, p) / p->prkNeighbourhood;
}

/// <summary>Calculates and returns the sigma values for the standard deviations of the MDEF.</summary>
/// <param name='d'>The collection of data we want to use for our calculation.</param>
/// <param name='p'>The reference datapoint we would like to use.</param>
/// <param name='r'>The range we want to use.</param>
/// <param name='k'>The neighbourhood size.</param>
float LOCI::GetSigma(DataChart * d, Datapoint * p)
{
	float stdevSum = 0;

	int pkNeighbourCount = 0;
	for (int i = 0; i < p->tmp_neighbours.size(); ++i)
	{
		pkNeighbourCount = p->tmp_neighbours[i]->tmp_rkNeighbours.size();
		stdevSum += powf((pkNeighbourCount - p->prkNeighbourhood), 2);
	}

	return sqrtf(stdevSum / p->tmp_neighbours.size());
}

#pragma region R-Neighbours

/// <summary>Sets the r-neighbours(neighbours within range r) of a datapoint.</summary>
/// <param name='d'>The collection of data we want to use for our calculation.</param>
/// <param name='p'>The reference datapoint we would like to use.</param>
/// <param name='r'>The range we want to use.</param>
void LOCI::SetRNeighbours(DataChart * d, Datapoint * p, std::vector<Datapoint*>* neighbours, float r)
{
	int startIndex = d->GetValues()->size() > WINDOW_SIZE ? d->GetValues()->size() - WINDOW_SIZE : 0;
	int endIndex = d->GetValues()->size() > WINDOW_SIZE ? startIndex + WINDOW_SIZE : d->GetValues()->size();

	neighbours->clear();
	// Calculate the distance to p for all the values
	for (int i = startIndex; i < endIndex; ++i)
	{
		// Get the position from the Datapoint
		float dist = Distance(d->GetValues()->at(i)->position, p->position);
		// Add a neighbour to p
		if (dist <= r)
			neighbours->push_back(d->GetValues()->at(i));
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
			sum_rNeighbours += p->rkNeighbours.size();
		else
		{
			Datapoint* o = p->neighbours[i];
			SetRNeighbours(d, o, &o->rkNeighbours, k*r);

			o->tmp_rkNeighbours = o->rkNeighbours;
			sum_rNeighbours += o->rkNeighbours.size();
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
	for (int i = 0; i < p->tmp_neighbours.size(); ++i)
	{
		// Make sure we do not edit the r-neighbours of p itself
		if (p->tmp_neighbours[i] == p)
			sum_rNeighbours += p->tmp_rkNeighbours.size();
		else
		{
			Datapoint* o = p->tmp_neighbours[i];
			UpdateRNeighbours(o, &o->tmp_rkNeighbours, k*r);
			sum_rNeighbours += o->tmp_rkNeighbours.size();
		}
	}

	// Calculate the average over all the neighbours
	p->prkNeighbourhood = sum_rNeighbours / p->tmp_neighbours.size();
}

#pragma endregion

LOCI::~LOCI()
{
}

#endif
