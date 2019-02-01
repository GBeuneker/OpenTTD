#pragma once
#include "Detector.h"
#include "../core/random_func.hpp"
#include <map>

class SOM : public Detector
{
public:
	SOM() {};
	SOM(uint16_t size);
	void Run() override;
	void IntializeMap(DataChart * d, SOM_Datapoint * nodes);
	virtual void AddData(std::vector<DataChart*> _datacharts) override;
	~SOM();
protected:
	Classification Classify(DataChart* d, SOM_Datapoint p) override;
	bool IsInSOMMap(SOM_Datapoint * nodes, uint16_t size, SOM_Datapoint node);
	void Train(DataChart * d, SOM_Datapoint * nodes, uint16_t iterations);
	float GetRadius(uint16_t iteration, uint16_t totalIterations);
	float GetLearningRate(uint16_t iteration, uint16_t totalIterations);
	float GetDistanceDecay(float distance, float radius);
	void UpdatePosition(SOM_Datapoint* p, Vector2 targetPosition, float learningRate, float distanceDecay = 1);
private:
	std::map<DataChart*, int> chartIndices;
	SOM_Datapoint nodesList[1][1];
	uint16_t size;
	float startRadius = 10, learningRate = 1;
};

