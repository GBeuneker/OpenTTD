#pragma once
#include "lof.h"
#include "knn.h"
#include "loci.h"
#include "som.h"
#include "DataChart.h"
#include "VariablePointer.h"
#include "../core/random_func.hpp"

#include <iostream>
#include <fstream>
#include <filesystem>
#include <vector>
#include <direct.h>
#include <stdlib.h>
#include <stdio.h>

struct AnomalyScore
{
	AnomalyScore(int tick, float anomalyScore)
	{
		this->tick = tick;
		this->anomalyScore = anomalyScore;
	}

	int tick;
	float anomalyScore;
};

class AnomalyDetector
{
public:
	static AnomalyDetector* GetInstance()
	{
		if (!AnomalyDetector::instance)
			AnomalyDetector::instance = new AnomalyDetector;
		return AnomalyDetector::instance;
	}
	AnomalyDetector();
	~AnomalyDetector();
	void BuildCharts();
	void LogDataTick();
	void Serialize();
	std::vector<DataChart*> DeSerialize(const char* folder);
	bool TriggerVariableIncrease(float chance = 10, char* msg = "");
	bool TriggerVariableReset(float chance = 10, char* msg = "");
	bool TriggerFunctionFailure(float chance = 10, char* msg = "");
	void TrackPointer(size_t * var, char * name);
	uint32_t GetTicks() { return ticks; }
	const std::string GetBaseFolder()
	{
#if USE_KNN
		return ".\\..\\_data\\KNN\\";
#elif USE_LOF
		return ".\\..\\_data\\LOF\\";
#elif USE_LOCI
		return ".\\..\\_data\\LOCI\\";
#elif USE_SOM
		return ".\\..\\_data\\SOM\\";
#endif
	}
	const std::string GetDataPath() {
		return "seed_" + std::to_string(_random.seed) + "_a_" + std::to_string(ANOMALY_PERCENTAGE) + "_t_" + std::to_string(ANOMALY_THRESHOLD) + "_w_" + std::to_string(WINDOW_SIZE);
	}
	void Reset();
private:
	float GetThreshold() {
#if FILTER_POINTS
		// Variable size is actually increased by one due to the valueCount variable
		return m_variables.size() * 0.5f;
#endif
		// Threshold is equal to half the amount of combinations possible for every variable
		return (m_variables.size() - 1) * 0.5f;
};
	void DetectAnomaly(std::vector<Classification> results);
	void AnalyzeAllData();
	void AnalyzeData(std::string spath);
	void LogAnomalyScore(uint32_t tick, float score);
	static AnomalyDetector* instance;
	std::vector<VariablePointer> m_variables;
	std::vector<DataChart*> m_datacharts;
	std::vector<std::tuple<int, float>> m_anomalyScores;
	std::map<int, std::string> m_anomalyOccurrences;
	uint32_t ticks = 0;
	bool chartsBuilt = false;

	// Anomaly Detectors
#if USE_KNN
	KNN *knn;
#elif USE_LOF
	LOF *lof;
#elif USE_LOCI
	LOCI *loci;
#elif USE_SOM
	SOM *som;
#endif
};

