#pragma once
#include "Detector.h";

#define WINDOW_SIZE 500

class LOF : public Detector
{
public:
	LOF(uint16_t k);
	void Run() override;
	~LOF();
protected:
	Classification Classify(DataChart* d, LOF_Datapoint p) override;
	void SetLOF(DataChart * d, LOF_Datapoint * p);
	void SetLRD(DataChart * d, LOF_Datapoint *p);
	float GetReachDistance(DataChart * d, LOF_Datapoint p, LOF_Datapoint o);
	void SetKDistance(DataChart * d, LOF_Datapoint *p);
	void SetKNeighbours(DataChart* d, LOF_Datapoint *p);
	uint16_t k;
	LOF_Datapoint lofDatapoints[WINDOW_SIZE];
};

