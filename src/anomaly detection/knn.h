#pragma once
#include "Detector.h";

#define WINDOW_SIZE 500

class KNN : public Detector
{
public:
	KNN() {};
	KNN(uint16_t k);
	void Run() override;
	~KNN();
protected:
	Classification Classify(DataChart* d, Datapoint p) override;
private:
	uint16_t k;
	float distances[WINDOW_SIZE];
};
