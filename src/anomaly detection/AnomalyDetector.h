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
	void SerializeLine();
	void SerializeFull();
	bool TriggerVariableIncrease(int chance = 10);
	bool TriggerVariableReset(int chance = 10);
	bool TriggerFunctionFailure(int chance = 10);
	void TrackPointer(size_t * var, char * name);
	void Reset();
private:
	static AnomalyDetector* instance;
	std::vector<VariablePointer> m_variables;
	std::vector<DataChart*> m_datacharts;
	int ticks = 0;
	bool chartsBuilt = false;
};

