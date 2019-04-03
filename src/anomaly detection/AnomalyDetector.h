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
	enum class Algorithm { KNN, LOF, LOCI, SOM, NONE };

	static AnomalyDetector* GetInstance()
	{
		if (!AnomalyDetector::instance)
			AnomalyDetector::instance = new AnomalyDetector;
		return AnomalyDetector::instance;
	}
	AnomalyDetector();
	~AnomalyDetector();
	void BuildCharts();
	void RunExperiments(const char * path);
	void RunExperiments(std::vector<DataChart*> loadedCharts, std::map<int, std::string> anomalyOccurences);
	void LogDataTick();
	void Serialize();
	void Serialize(std::vector<DataChart*> datacharts, std::vector<std::tuple<int, float>> anomalyScores, std::map<int, std::string> anomalyOccurances, int events);
	std::vector<DataChart*> DeSerializeCharts(const char* folder);
	std::map<int, std::string> DeserializeAnomalyOccurences(const char * folder);
	bool TriggerVariableIncrease(float chance = 10, char* msg = "");
	bool TriggerVariableReset(float chance = 10, char* msg = "");
	bool TriggerFunctionFailure(float chance = 10, char* msg = "");
	void TrackPointer(size_t * var, char * name);
	float GetAnomalyPercentage() { return anomaly_percentage; }
	const std::string GetBaseFolder()
	{

		if (algorithm == Algorithm::KNN)
			return ".\\..\\_data\\KNN\\";
		else if (algorithm == Algorithm::LOF)
			return ".\\..\\_data\\LOF\\";
		else if (algorithm == Algorithm::LOCI)
			return ".\\..\\_data\\LOCI\\";
		else if (algorithm == Algorithm::SOM)
			return ".\\..\\_data\\SOM\\";
		else
			return  ".\\..\\_data\\BASE\\";
	}
	const std::string GetDataPath() {
		return "seed_" + std::to_string(_random.seed) + "_a_" + std::to_string(anomaly_percentage) + "_t_" + std::to_string(threshold) + "_w_" + std::to_string(windowSize) + "_k_" + std::to_string(k_percentage);
	}
	void Reset();
private:
	float GetThreshold(int chartAmount) {
		// Variable amount is equal to: n!/2(n-2)! = 0.5 * n(n-1).
		// Let chartAmount = L
		// Formula: 0.5 * n(n-1) = L => n² - n - 2L = 0 => (ABC-formula) n = (1 +/- sqrt(1+4*2L)) / 2
		float variableAmount = (1 + sqrtf(1 + 4 * 2 * chartAmount)) / 2;
		return variableAmount * 0.5f;
	};
	void DetectAnomaly(std::vector<Classification> results);
	void AnalyzeCharts(std::vector<DataChart*> charts, std::map<int, std::string> anomalyOccurences);
	void AnalyzeAllData();
	void AnalyzeData(std::string spath, int events);
	void LogAnomalyScore(uint32_t tick, float score);
	static AnomalyDetector* instance;
	std::vector<VariablePointer> m_variables;
	std::vector<DataChart*> m_datacharts;
	std::vector<std::tuple<int, float>> m_anomalyScores;
	std::map<int, std::string> m_anomalyOccurrences;
	uint32_t m_ticks = 0, m_events = 0;
	bool chartsBuilt = false;

	float threshold = ANOMALY_THRESHOLD;
	int windowSize = WINDOW_SIZE;
	float k_percentage = K_PERCENTAGE;
	float anomaly_percentage = ANOMALY_PERCENTAGE;

	Algorithm algorithm = Algorithm::SOM;

	// Anomaly Detectors
	KNN *knn;
	LOF *lof;
	LOCI *loci;
	SOM *som;
};

