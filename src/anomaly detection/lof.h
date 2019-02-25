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
	uint16_t k_values[10]
	{
		5, // 1
		5, // 2
		5, // 3
		5, // 4
		5, // 5
		5, // 6
		5, // 7
		5, // 8
		5, // 9
		5  // 10
	};
	std::vector<float[WINDOW_SIZE]> lofValues;
	std::vector<int> lofIndices;
};

#endif

