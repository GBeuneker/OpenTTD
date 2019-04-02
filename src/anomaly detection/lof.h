#pragma once
#include "Detector.h";

class LOF : public Detector
{
public:
	LOF() {};
	LOF(float k_percentage);
	void SetData(std::vector<DataChart*> _datacharts) override;
	~LOF();
protected:
	void Train(DataChart * d, Datapoint * lof_p);
	Classification Classify(DataChart* d, Datapoint *p) override;
	float GetLOF(DataChart * d, Datapoint * p);
	void SetLRD(DataChart * d, Datapoint *p);
	float GetReachDistance(DataChart * d, Datapoint* p, Datapoint* o);
	void SetKDistance(DataChart * d, Datapoint *p);
	void SetKNeighbours(DataChart* d, Datapoint *p);
	void UpdateKNeighbours(DataChart* d, Datapoint* p, Datapoint* new_p);
private:
	uint16_t current_k;
	float k_percentage;
	std::vector<std::vector<float>> lofValues;
	std::vector<int> lofIndices;
};

