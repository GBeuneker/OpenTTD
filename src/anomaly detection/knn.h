#pragma once
#include "Detector.h";

class KNN : public Detector
{
public:
	KNN() {};
	KNN(float k_percentage);
	void SetData(std::vector<DataChart*> _datacharts) override;
	~KNN();
protected:
	Classification Classify(DataChart* d, Datapoint* p) override;
	void Train(DataChart* d, Datapoint* p) override;
private:
	float k_percentage;
	std::vector<float> distances;
	std::vector<std::vector<float>> kDistances;
	std::vector<int> kIndices;
};
