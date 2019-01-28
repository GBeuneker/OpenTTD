#pragma once
#include <vector>
#include <sstream>
#include "VariablePointer.h"
#include "Vector2.h"

struct DataPoint
{
	Vector2 position;
	float distance;

	bool operator < (const DataPoint& dp) const
	{
		return (distance < dp.distance);
	}
};

class DataChart
{
public:
	DataChart(VariablePointer varA, VariablePointer varB);
	void LogData();
	std::string SerializeLine();
	std::string SerializeFull();
	std::string GetLabelString();
	std::vector<DataPoint>* GetValues() { return this->values; }
	DataPoint GetLast() { return lastValue; }
	~DataChart();
private:
	VariablePointer m_varA, m_varB;
	std::vector<DataPoint>* values;
	DataPoint lastValue;
};

