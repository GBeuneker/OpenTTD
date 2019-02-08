#pragma once
#include "Detector.h"
#include "../core/random_func.hpp"
#include <map>

#define WINDOW_SIZE 500;

class SOM : public Detector
{
public:
	SOM() {};
	SOM(uint16_t width, uint16_t height, float learningRate);
	void SetData(std::vector<DataChart*> _datacharts) override;
	void Run() override;
	void Serialize();
	~SOM();
protected:
	// Initialization
	void IntializeMap(DataChart * d, std::vector<SOM_Datapoint>* nodes);
	// Training
	void UpdateMap(std::vector<SOM_Datapoint>* nodes, SOM_Datapoint datapoint, uint16_t i);
	void Train(DataChart * d, std::vector<SOM_Datapoint> *nodes, uint16_t iterations);
	float GetRadius(uint16_t iteration, uint16_t totalIterations);
	float GetLearningRate(uint16_t iteration, uint16_t totalIterations);
	float GetDistanceDecay(float distance, float radius);
	void UpdatePosition(SOM_Datapoint* p, Vector2 targetPosition, float learningRate, float distanceDecay = 1);
	// Classification
	Classification Classify(uint16_t index, SOM_Datapoint p);
	bool IsInSOMMap(std::vector<SOM_Datapoint> nodes, uint16_t size, SOM_Datapoint datapoint);
	bool DistToEdge(std::vector<SOM_Datapoint> nodes, SOM_Datapoint p);
	// Utilities
	void SortCounterClockwise(std::vector<SOM_Datapoint>* nodes);
	void ConvexHull(std::vector<SOM_Datapoint>* nodes);
	int Orientation(SOM_Datapoint p, SOM_Datapoint q, SOM_Datapoint r);
private:
	uint16_t windowSize = WINDOW_SIZE;

	std::vector<std::vector<SOM_Datapoint>> somNodes;
	std::vector<bool> initializedCharts;
	uint16_t width, height;
	float startRadius = 1, learningRate = 1, timeConstant;
	uint16_t iteration = 1;
};

