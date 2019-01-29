#pragma once
#include "Detector.h";

class LOF : public Detector
{
public:
	LOF();
	void Run() override;
	~LOF();
protected:
	Classification Classify(DataChart* d, LOF_DataPoint p) override;
	void SetLOF(DataChart * d, LOF_DataPoint * p);
	void SetLRD(DataChart * d, LOF_DataPoint *p);
	float GetReachDistance(DataChart * d, LOF_DataPoint p, LOF_DataPoint o);
	void SetKDistance(DataChart * d, LOF_DataPoint *p);
	void SetKNeighbours(DataChart* d, LOF_DataPoint *p);
	uint16_t k;
};

