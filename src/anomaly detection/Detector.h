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
	void SetParameters(float slope, int windowSize, float midPoint = 0.75f)
	{
		this->slope = slope;
		this->windowSize = windowSize;
		this->midPoint = midPoint;
	}
	void SetWindowSize(int windowSize) { this->windowSize = windowSize; }
	virtual std::vector<Classification> Run(int _tick);
protected:
	float Sigmoid(float x);
	virtual Classification Classify(DataChart* d, Datapoint* p) { return Classification(); };
	virtual void Train(DataChart* d, Datapoint* p) {  };
	std::vector<DataChart*> datacharts;
	float slope, midPoint;
	int windowSize;
};
