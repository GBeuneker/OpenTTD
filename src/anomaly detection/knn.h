#pragma once
#include "Detector.h";

#define MAXSIZE 500

class KNN : public Detector
{
public:
	KNN() {};
	KNN(uint16_t k);
	void Run() override;
	Classification Classify(DataChart* d, Vector2 v);
	~KNN();
private:
	uint16_t k;
	float distances[MAXSIZE];
};
