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
	DataChart() { m_aggregatedPoints = new std::vector<Datapoint*>(); };
	DataChart(VariablePointer varA, VariablePointer varB);
	void SetPointers(VariablePointer varA, VariablePointer varB);
	VariablePointer GetPointerA() { return m_varA; }
	VariablePointer GetPointerB() { return m_varB; }
	void LogData(int tick, Datapoint * aggregatedValue, std::vector<Datapoint*> subPoints);
	void LogData(int tick);
	std::string Serialize();
	void DeSerialize(const char* path);
	std::string GetLabelString();
	void DeleteAllData();
	std::vector<Datapoint*>* GetValues() { return this->m_aggregatedPoints; }
#if USE_SUBPOINTS
	std::vector<Datapoint*> GetSubvalues(Datapoint* p)
	{
		if (p != 0)
			return m_subPoints.at(p);
		else
			return std::vector<Datapoint*>();
	}
#endif
	Datapoint* GetLast() { return m_aggregatedPoints->size() > 0 ? m_aggregatedPoints->back() : 0; }
	Datapoint* GetRandom() { return m_aggregatedPoints->at(rand() % m_aggregatedPoints->size()); }
	Datapoint* GetValueAt(int _tick)
	{
		if (m_tickToPoint.find(_tick) != m_tickToPoint.end())
			return m_tickToPoint.at(_tick);
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
	std::vector<Datapoint*>* m_aggregatedPoints;
	std::map<int, Datapoint*> m_tickToPoint;
#if USE_SUBPOINTS
	std::map<Datapoint*, std::vector<Datapoint*>> m_subPoints;
#endif
};

