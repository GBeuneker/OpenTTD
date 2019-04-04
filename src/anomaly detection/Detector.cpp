#pragma once
#include "Detector.h"

void Detector::SetData(std::vector<DataChart*> _datacharts)
{
	this->datacharts = _datacharts;
}

float Detector::Sigmoid(float x)
{
	return 1 / (1 + exp(-this->slope * (x - this->midPoint)));
}

std::vector<Classification> Detector::Run(int _tick)
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
#if USE_SUBPOINTS
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
		if (subPoints.size() > 0)
			results.push_back(result);
		else
			results.push_back(Classify(d, p));
#else
		results.push_back(Classify(d, p));
#endif
		// Train using the average of all points
		Train(d, p);
	}

	return results;
}
