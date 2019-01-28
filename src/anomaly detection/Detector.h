#pragma once
#include "DataChart.h"
#include "Vector2.h"

#include <algorithm>

struct Classification {
	bool answer = false;
	float certainty = -1;
};

class Detector
{
public:
	virtual void AddData(std::vector<DataChart*> _datacharts) { this->datacharts = _datacharts; };
	virtual void Run() {};
protected:
	virtual Classification Classify(DataChart* d, DataPoint p) { return Classification(); };
	std::vector<DataChart*> datacharts;
};
