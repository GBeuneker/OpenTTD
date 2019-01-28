#pragma once
#include "Detector.h";

class LOF : public Detector
{
public:
    LOF();
	void Run() override;
    ~LOF();
protected:
	Classification Classify(DataChart* d, DataPoint p) override;
private:
	std::vector<DataPoint> GetNeighbours(DataChart* d, DataPoint p, uint16_t k);
	uint16_t k;
};

