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
	void LogData();
	std::string Serialize();
	void DeSerialize(const char* path);
	std::string GetLabelString();
	std::vector<Datapoint*>* GetValues() { return this->values; }
	Datapoint* GetLast() { return values->back(); }
	Datapoint* GetRandom() { return values->at(_random.Next(values->size())); }
	Vector2 GetMinValue() { return Vector2(minX, minY); }
	Vector2 GetMaxValue() { return Vector2(maxX, maxY); }
	bool IsDirty() { return isDirty; }
	~DataChart();
private:
	std::vector<Datapoint*>* values;
	float minX = FLT_MAX, maxX = FLT_MIN;
	float minY = FLT_MAX, maxY = FLT_MIN;
	VariablePointer m_varA = VariablePointer(0, "null"), m_varB = VariablePointer(0, "null");
	bool isDirty = true;
};

