#pragma once
#include <vector>
#include <sstream>
#include <string>
#include <iterator>
#include <map>
#include "VariablePointer.h"
#include "DataPoint.h"
#include "../core/random_func.hpp"
#include <fstream>

class DataChart
{
public:
	DataChart() { aggregatedValues = new std::vector<Datapoint*>(); };
	DataChart(VariablePointer varA, VariablePointer varB);
	void SetPointers(VariablePointer varA, VariablePointer varB);
	void LogData(int tick, Datapoint * aggregatedValue, std::vector<Datapoint*> subPoints);
	void LogData(int tick);
	std::string Serialize();
	void DeSerialize(const char* path);
	std::string GetLabelString();
	std::vector<Datapoint*>* GetValues() { return this->aggregatedValues; }
#if USE_SUBVALUES
	std::vector<Datapoint*> GetSubvalues(Datapoint* p)
	{
		if (p != 0)
			return subvalues.at(p);
		else
			return std::vector<Datapoint*>();
	}
#endif
	Datapoint* GetLast() { return aggregatedValues->size() > 0 ? aggregatedValues->back() : 0; }
	Datapoint* GetRandom() { return aggregatedValues->at(rand() % aggregatedValues->size()); }
	Datapoint* GetValueAt(int _tick)
	{
		if (tickToValue.find(_tick) != tickToValue.end())
			return tickToValue.at(_tick);
		else
			return 0;
	}
#if FILTER_POINTS
	int valueCount = 0;
	bool isDirty = true;
#endif
	~DataChart();
private:
	VariablePointer m_varA = VariablePointer(0, "null"), m_varB = VariablePointer(0, "null");
	std::vector<Datapoint*>* aggregatedValues;
	std::map<int, Datapoint*> tickToValue;
#if USE_SUBVALUES
	std::map<Datapoint*, std::vector<Datapoint*>> subvalues;
#endif
};

