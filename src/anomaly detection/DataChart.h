#pragma once
#include <vector>
#include <sstream>
#include "VariablePointer.h"

class DataChart
{
public:
	DataChart(VariablePointer varA, VariablePointer varB);
	void LogData();
	std::string SerializeLine();
	std::string SerializeFull();
	std::string GetLabelString();
	~DataChart();
private:
	VariablePointer m_varA, m_varB;
	std::vector<int> xAxis, yAxis;
};

