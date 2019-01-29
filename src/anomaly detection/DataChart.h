#pragma once
#include <vector>
#include <sstream>
#include "VariablePointer.h"
#include "Vector2.h"
#include "DataPoint.h"

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
	std::vector<Datapoint>* values;
	Datapoint GetLast() { return lastValue; }
	~DataChart();
private:
	VariablePointer m_varA, m_varB;
	Datapoint lastValue;
};

