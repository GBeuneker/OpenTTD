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
	std::vector<Classification> Run() override;
	void Serialize();
	~SOM();
protected:
	// Initialization
	void IntializeMap(DataChart * d, std::vector<Datapoint>* nodes);
	// Training
	void UpdateMap(std::vector<Datapoint>* nodes, Datapoint datapoint, uint16_t i);
	void Train(DataChart * d, std::vector<Datapoint> *nodes, uint16_t iterations);
	float GetRadius(uint16_t iteration, uint16_t totalIterations);
	float GetLearningRate(uint16_t iteration, uint16_t totalIterations);
	float GetDistanceDecay(float distance, float radius);
	void UpdatePosition(Datapoint* p, Vector2 targetPosition, float learningRate, float distanceDecay = 1);
	// Classification
	Classification Classify(DataChart* d, Datapoint p) override;
	bool IsInSOMMap(std::vector<Datapoint> nodes, uint16_t size, Datapoint datapoint);
	bool DistToEdge(std::vector<Datapoint> nodes, Datapoint p);
	// Utilities
	void SortCounterClockwise(std::vector<Datapoint>* nodes);
	void ConvexHull(std::vector<Datapoint>* nodes);
	int Orientation(Datapoint p, Datapoint q, Datapoint r);
private:
	uint16_t windowSize = WINDOW_SIZE;

	std::vector<std::vector<Datapoint>> somNodes;
	std::vector<bool> initializedCharts;
	uint16_t width, height;
	float startRadius = 1, learningRate = 1, timeConstant;
	uint16_t iteration = 1;
};

