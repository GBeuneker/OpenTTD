#pragma once
#include "Detector.h"

void Detector::SetData(std::vector<DataChart*> _datacharts)
{
	this->datacharts = _datacharts;
}

void Detector::DetermineAnomaly(std::vector<Classification> results)
{
	float anomalyScore = 0;

	for (int i = 0; i < results.size(); ++i)
	{
		Classification result = results.at(i);

		if (result.isAnomaly)
			anomalyScore += result.certainty;
	}

	// If our anomaly score is high enough
	if (anomalyScore >= anomalyThreshold)
	{
		// Report an anomaly
		printf("Anomaly Detected!");
	}
}
