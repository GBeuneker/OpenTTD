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
	DataChart() { values = new std::vector<Datapoint>(); };
	DataChart(VariablePointer varA, VariablePointer varB);
	void LogData();
	std::string Serialize();
	void DeSerialize(const char* path);
	std::string GetLabelString();
	std::vector<Datapoint>* GetValues() { return this->values; }
	Datapoint GetLast() { return lastValue; }
	Datapoint GetRandom() { return values->at(_random.Next(values->size())); }
	~DataChart();
private:
	std::vector<Datapoint>* values;
	VariablePointer m_varA = VariablePointer(0, "null"), m_varB = VariablePointer(0, "null");
	Datapoint lastValue;
};

