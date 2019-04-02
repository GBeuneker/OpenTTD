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
	bool ApplyCooldown(uint16_t chartIndex, bool isOutlier);
	float Sigmoid(float x, float slope = ANOMALY_THRESHOLD, float midPoint = 0.75f);
	virtual Classification Classify(DataChart* d, Datapoint* p) { return Classification(); };
	virtual void Train(DataChart* d, Datapoint* p) {  };
	std::vector<DataChart*> datacharts;
	const int cooldownSize = 0;
	std::vector<int> cooldownSteps;
};
