#pragma once
#include "Detector.h"
//#include "AnomalyDetector.h"
#include "../core/random_func.hpp"
#include <map>

class SOM : public Detector
{
public:
	SOM() {};
	SOM(uint16_t width, uint16_t height, float learningRate);
	void SetData(std::vector<DataChart*> _datacharts) override;
	std::vector<Classification> Run(int _tick) override;
	void Serialize();
	~SOM();
protected:
	// Initialization
	void IntializeMap(DataChart * d, std::vector<Datapoint*>* nodes, int _tick);
	void TrainMap(uint16_t chartIndex);
	// Training
	void UpdateMap(uint16_t chartIndex, Datapoint * datapoint);
	float GetRadius(float startRadius, uint16_t iteration, uint16_t totalIterations);
	float GetLearningRate(float startLearningRate, uint16_t iteration, uint16_t totalIterations);
	float GetDistanceDecay(float distance, float radius);
	void UpdatePosition(Datapoint* p, Vector2 targetPosition, float learningRate, float distanceDecay = 1);
	void Train(DataChart * d, Datapoint * p);
	// Classification
	Classification Classify(DataChart* d, Datapoint *p) override;
	bool IsInSOMMap(std::vector<Datapoint*>* nodes, Datapoint * p);
	// Utilities
	void SortCounterClockwise(std::vector<Datapoint*>* nodes);
	void ConvexHull(std::vector<Datapoint*>* nodes);
	bool DoIntersect(Vector2 p1, Vector2 q1, Vector2 p2, Vector2 q2);
	float DistToEdge(std::vector<Datapoint*>* nodes, Datapoint *p);
	int Orientation(Vector2 p, Vector2 q, Vector2 r);
	bool OnSegment(Vector2 p, Vector2 q, Vector2 r);
private:
	uint16_t width, height;
	float learningRate = 0.5f;

	std::vector<std::vector<Datapoint*>*> somNodes;
	std::vector<bool> initializedCharts;
	std::vector<uint16_t> chartIterations, maxChartIterations;
	std::vector<float> startRadii;

	std::vector<std::vector<float>> somDistances;
	std::vector<int> somIndices;
};

