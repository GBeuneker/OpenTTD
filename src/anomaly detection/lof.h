#pragma once
#include "Detector.h";

#define WINDOW_SIZE 100

class LOF : public Detector
{
public:
	LOF(uint16_t k);
	void SetData(std::vector<DataChart*> _datacharts) override;
	void Run() override;
	~LOF();
protected:
	Classification Classify(DataChart* d, LOF_Datapoint p) override;
	void SetLOF(DataChart * d, LOF_Datapoint * p);
	void SetLRD(DataChart * d, LOF_Datapoint *p);
	float GetReachDistance(DataChart * d, LOF_Datapoint p, LOF_Datapoint o);
	void SetKDistance(DataChart * d, LOF_Datapoint *p);
	void SetKNeighbours(DataChart* d, LOF_Datapoint *p);
private:
	uint16_t k;
	LOF_Datapoint lofDatapoints[WINDOW_SIZE];
	std::vector<float[WINDOW_SIZE]> lofValues;
	std::vector<int> lofIndices;
};

