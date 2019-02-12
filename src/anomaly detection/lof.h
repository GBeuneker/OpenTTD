#pragma once
#include "Detector.h";

#if USE_LOF

class LOF : public Detector
{
public:
	LOF(uint16_t k);
	void SetData(std::vector<DataChart*> _datacharts) override;
	~LOF();
protected:
	Classification Classify(DataChart* d, Datapoint *p) override;
	float GetLOF(DataChart * d, Datapoint * p);
	void SetLRD(DataChart * d, Datapoint *p);
	float GetReachDistance(DataChart * d, Datapoint* p, Datapoint* o);
	void SetKDistance(DataChart * d, Datapoint *p);
	void SetKNeighbours(DataChart* d, Datapoint *p);
	void UpdateKNeighbours(DataChart* d, Datapoint* p, Datapoint* new_p);
private:
	uint16_t k;
	std::vector<float[WINDOW_SIZE]> lofValues;
	std::vector<int> lofIndices;
};

#endif

