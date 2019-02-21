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
	void Reset();
private:
	void DetectAnomaly(std::vector<Classification> results);
	static AnomalyDetector* instance;
	std::vector<VariablePointer> m_variables;
	std::vector<DataChart*> m_datacharts;
	uint32_t ticks = 0;
	bool chartsBuilt = false;

	// Anomaly Detectors
#if USE_KNN
	KNN *knn;
#elif USE_LOF
	LOF *loci;
#elif USE_LOCI
	LOCI *loci;
#elif USE_SOM
	SOM *som;
#endif
};

