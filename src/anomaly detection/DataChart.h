#pragma once
#include <vector>
#include <sstream>
#include "VariablePointer.h"
#include "DataPoint.h"
#include "../core/random_func.hpp"
#include <fstream>

class DataChart
{
public:
	DataChart() { values = new std::vector<Datapoint*>(); };
	DataChart(VariablePointer varA, VariablePointer varB);
	void SetPointers(VariablePointer varA, VariablePointer varB);
	void LogData();
	std::string Serialize();
	void DeSerialize(const char* path);
	std::string GetLabelString();
	std::vector<Datapoint*>* GetValues() { return this->values; }
	Datapoint* GetLast() { return values->back(); }
	Datapoint* GetRandom() { return values->at(rand() % values->size()); }
#if FILTER_POINTS
	int valueCount = 0;
	bool isDirty = true;
#endif
	~DataChart();
private:
	VariablePointer m_varA = VariablePointer(0, "null"), m_varB = VariablePointer(0, "null");
	std::vector<Datapoint*>* values;
};

