#pragma once
#include "lof.h"
#include "knn.h"
#include "loci.h"
#include "som.h"
#include "DataChart.h"
#include "VariablePointer.h"

#include <iostream>
#include <fstream>
#include <vector>

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
	void TrackPointer(size_t * var, char * name);
	void Reset();
	int seed;
private:
	static AnomalyDetector* instance;
	std::vector<VariablePointer> m_variables;
	std::vector<DataChart*> m_datacharts;
	int ticks = 0;
	bool chartsBuilt = false;
};

