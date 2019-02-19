#pragma once
#include "Detector.h"

void Detector::SetData(std::vector<DataChart*> _datacharts)
{
	this->datacharts = _datacharts;
	cooldownSteps = std::vector<int>(datacharts.size());
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
		{
			cooldownSteps[chartIndex]--;
			return false;
		}
		// If the cooldown has ended and we're still outlier, then flag as outlier
		else if (cooldownSteps[chartIndex] <= 0)
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

std::vector<Classification> Detector::Run()
{
	std::vector<Classification> results;

	for (int i = 0; i < datacharts.size(); ++i)
	{
		DataChart* d = datacharts[i];
		Datapoint* p = d->GetLast();
		if (d->IsDirty())
			results.push_back(Classify(d, p));
	}

	return results;
}
