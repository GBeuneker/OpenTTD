#pragma once
#include <vector>
#include <sstream>
#include "VariablePointer.h"
#include "DataPoint.h"
#include "../core/random_func.hpp"

class DataChart
{
public:
	DataChart() { values = new std::vector<Datapoint>(); };
	DataChart(VariablePointer varA, VariablePointer varB);
	void LogData();
	std::string SerializeLine();
	std::string SerializeFull();
	std::string GetLabelString();
	std::vector<Datapoint>* GetValues() { return this->values; }
	Datapoint GetLast() { return lastValue; }
	Datapoint GetRandom() { return values->at(_random.Next(values->size())); }
	~DataChart();
private:
	VariablePointer m_varA, m_varB;
	Datapoint lastValue;
	std::vector<Datapoint>* values;
};

