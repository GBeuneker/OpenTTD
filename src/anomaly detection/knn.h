#pragma once
#include "Detector.h";

#if USE_KNN

class KNN : public Detector
{
public:
	KNN() {};
	KNN(uint16_t k);
	void SetData(std::vector<DataChart*> _datacharts) override;
	~KNN();
protected:
	Classification Classify(DataChart* d, Datapoint p) override;
private:
	uint16_t k;
	float distances[WINDOW_SIZE];
	float averageDistance;
	std::vector<float[WINDOW_SIZE]> kDistances;
	std::vector<int> kIndices;
};

#endif
