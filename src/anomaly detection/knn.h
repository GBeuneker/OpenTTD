#pragma once
#include "Detector.h";

#if USE_KNN

class KNN : public Detector
{
public:
	KNN() {};
	KNN(uint16_t k_values[]);
	void SetData(std::vector<DataChart*> _datacharts) override;
	~KNN();
protected:
	Classification Classify(DataChart* d, Datapoint* p) override;
	void Train(DataChart* d, Datapoint* p) override;
private:
	uint16_t* k_values;
	std::vector<float> distances;
	std::vector<std::vector<float>> kDistances;
	std::vector<int> kIndices;
};

#endif
