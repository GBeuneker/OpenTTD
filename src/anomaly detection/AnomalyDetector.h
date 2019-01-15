#pragma once
#include "lof.h"
#include "knn.h"
#include "loci.h"
#include "som.h"

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
	void LogData();
	void TrackPointer(size_t * _var);
	void Reset();
	int seed;
private:
	static AnomalyDetector* instance;
	std::vector<size_t*> m_variables;
	int ticks = 0;
};

