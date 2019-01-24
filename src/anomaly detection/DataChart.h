#pragma once
#include <vector>
#include <sstream>
#include "VariablePointer.h"
#include "Vector2.h"

class DataChart
{
public:
	DataChart(VariablePointer varA, VariablePointer varB);
	void LogData();
	std::string SerializeLine();
	std::string SerializeFull();
	std::string GetLabelString();
	std::vector<Vector2>* GetValues() { return this->values; }
	Vector2 GetLast() { return lastValue; }
	~DataChart();
private:
	VariablePointer m_varA, m_varB;
	std::vector<Vector2>* values;
	Vector2 lastValue;
};

