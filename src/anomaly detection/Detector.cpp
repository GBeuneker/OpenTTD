#pragma once
#include "Detector.h"

void Detector::SetData(std::vector<DataChart*> _datacharts)
{
	this->datacharts = _datacharts;
	cooldownSteps = std::vector<int>(datacharts.size());

	for (int i = 0; i < cooldownSteps.size(); ++i)
		cooldownSteps.at(i) = cooldownSize;
}

/// <summary>Applies a cooldown to an outlier to account for the forming of new clusters.</summary>
/// <param name='chartIndex'>The chart we are currently tracking.</param>
/// <param name='isOutlier'>Whether a point was flagged as outlier.</param>
bool Detector::ApplyCooldown(uint16_t chartIndex, bool isOutlier)
{
	if (isOutlier)
	{
		// If we still have room to cool down, don't flag as outlier
		if (cooldownSteps[chartIndex] > 0)
			cooldownSteps[chartIndex]--;

		// If the cooldown has ended and we're still outlier, then flag as outlier
		if (cooldownSteps[chartIndex] <= 0)
		{
			// Restart the cooldown
			cooldownSteps[chartIndex] = cooldownSize;
			return true;
		}
	}
	else
		// Restart the cooldown
		cooldownSteps[chartIndex] = cooldownSize;

	return false;
}

float Detector::Sigmoid(float x, float slope, float midPoint)
{
	return 1 / (1 + exp(-slope * (x - midPoint)));
}

std::vector<Classification> Detector::Run()
{
	std::vector<Classification> results;

	for (int i = 0; i < datacharts.size(); ++i)
	{
		DataChart* d = datacharts[i];
		Datapoint* p = d->GetLast();

		// Classify using the individual variables
#if FILTER_POINTS
		if (!d->isDirty)
		{
			results.push_back(Classification());
			continue;
		}
#endif
#if USE_SUBVALUES
		Classification result;
		std::vector<Datapoint*> subPoints = d->GetSubvalues(p);
		// Get the result with the highest certainty
		for (int i = 0; i < subPoints.size(); ++i)
		{
#if FILTER_POINTS
			// Skip any points which are not marked as dirty
			if (!subPoints.at(i)->isDirty)
				continue;
#endif
			Classification r = Classify(d, subPoints.at(i));
			if (r.isAnomaly)
			{
				result.isAnomaly = true;
				result.certainty = fmax(result.certainty, r.certainty);
			}
		}
		results.push_back(result);
#else
		results.push_back(Classify(d, p));
#endif
		// Train using the average of all points
		Train(d, p);
	}

	return results;
}
