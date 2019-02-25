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
	Classification Classify(DataChart* d, Datapoint* p) override;
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
	float distances[WINDOW_SIZE];
	std::vector<float[WINDOW_SIZE]> kDistances;
	std::vector<int> kIndices;
};

#endif
