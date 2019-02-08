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
	virtual void Run() {};
protected:
	virtual Classification Classify(DataChart* d, Datapoint p) { return Classification(); };
	virtual Classification Classify(DataChart* d, LOF_Datapoint p) { return Classification(); };
	virtual Classification Classify(DataChart* d, LOCI_Datapoint p) { return Classification(); };
	virtual Classification Classify(DataChart* d, SOM_Datapoint p) { return Classification(); }
	virtual void DetermineAnomaly(std::vector<Classification> results);
	std::vector<DataChart*> datacharts;
	float anomalyThreshold = 2.0;
};
