#include "DataChart.h"

DataChart::DataChart(VariablePointer varA, VariablePointer varB)
{
	this->m_varA = varA;
	this->m_varB = varB;

	values = new std::vector<Datapoint>();
}

void DataChart::LogData()
{
	Vector2 position = Vector2(m_varA.GetValue(), m_varB.GetValue());

	Datapoint newDataPoint;
	newDataPoint.position = position;

	// Only add unique values
	for (int i = 0; i < values->size(); ++i)
		if (values->at(i) == newDataPoint)
			return;

	values->push_back(newDataPoint);
	lastValue = newDataPoint;
}

std::string DataChart::SerializeLine()
{
	std::ostringstream stringstream;

	stringstream << values->back().position.X << " " << values->back().position.Y << "\n";

	return stringstream.str();
}

std::string DataChart::SerializeFull()
{
	std::ostringstream stringstream;

	// Write the labels in the first line
	stringstream << GetLabelString();

	// Loop through all the values
	for (int i = 0; i < values->size(); ++i)
		stringstream << values->at(i).position.X << " " << values->at(i).position.Y << "\n";

	return stringstream.str();
}

std::string DataChart::GetLabelString()
{
	std::ostringstream stringstream;

	stringstream << "# " << m_varA.GetName() << " " << m_varB.GetName() << "\n";

	return stringstream.str();
}

DataChart::~DataChart()
{
	delete values;
}
