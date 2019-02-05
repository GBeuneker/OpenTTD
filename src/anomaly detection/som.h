#pragma once
#include "Detector.h"
#include "../core/random_func.hpp"
#include <map>

class SOM : public Detector
{
public:
	SOM() {};
	SOM(uint16_t width, uint16_t height, float learningRate);
	void IntializeMap(DataChart * d, std::vector<SOM_Datapoint>* nodes);
	void Run() override;
	void TrainAll(uint16_t iterations);
	void Serialize();
	virtual void SetTrainingData(std::vector<DataChart*> _trainingSet);
	~SOM();
protected:
	Classification Classify(uint16_t index, SOM_Datapoint p);
	bool IsInSOMMap(std::vector<SOM_Datapoint> nodes, uint16_t size, SOM_Datapoint datapoint);
	void Train(DataChart * d, std::vector<SOM_Datapoint> *nodes, uint16_t iterations);
	float GetRadius(uint16_t iteration, uint16_t totalIterations);
	float GetLearningRate(uint16_t iteration, uint16_t totalIterations);
	float GetDistanceDecay(float distance, float radius);
	void UpdatePosition(SOM_Datapoint* p, Vector2 targetPosition, float learningRate, float distanceDecay = 1);
	void SortCounterClockwise(std::vector<SOM_Datapoint>* nodes);
private:
	std::vector<std::vector<SOM_Datapoint>> nodesList;
	std::vector<DataChart*> trainingSet;
	uint16_t width, height, nodeAmount;
	float startRadius = 1, learningRate = 1;
};

