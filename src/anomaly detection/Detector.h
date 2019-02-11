#pragma once
#include "DataChart.h"
#include <algorithm>

struct Classification {
	bool isAnomaly = false;
	float certainty = -1;
};

class Detector
{
public:
	virtual void SetData(std::vector<DataChart*> _datacharts);
	virtual std::vector<Classification> Run();
protected:
	virtual Classification Classify(DataChart* d, Datapoint p) { return Classification(); };
	std::vector<DataChart*> datacharts;
	float anomalyThreshold = 2.0;
	const int cooldownSize = 5;
	std::vector<int> cooldownSteps;
};
