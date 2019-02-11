#pragma once
#include "Detector.h"

void Detector::SetData(std::vector<DataChart*> _datacharts)
{
	this->datacharts = _datacharts;
	cooldownSteps = std::vector<int>(datacharts.size());
}

std::vector<Classification> Detector::Run()
{
	std::vector<Classification> results;

	for (int i = 0; i < datacharts.size(); ++i)
	{
		DataChart* d = datacharts[i];
		Datapoint p = d->GetLast();
		if (d->IsDirty())
			results.push_back(Classify(d, p));
	}

	return results;
}
