#include "DataChart.h"

DataChart::DataChart(VariablePointer varA, VariablePointer varB)
{
	this->m_varA = varA;
	this->m_varB = varB;

	values = new std::vector<Vector2>();
}

void DataChart::LogData()
{
	Vector2 value = Vector2(m_varA.GetValue(), m_varB.GetValue());
	values->push_back(value);

	lastValue = value;
}

std::string DataChart::SerializeLine()
{
	std::ostringstream stringstream;

	stringstream << values->back().X << " " << values->back().Y << "\n";

	return stringstream.str();
}

std::string DataChart::SerializeFull()
{
	std::ostringstream stringstream;

	// Write the labels in the first line
	stringstream << GetLabelString();

	// Loop through all the values
	for (int i = 0; i < values->size(); ++i)
		stringstream << values->at(i).X << " " << values->at(i).Y << "\n";

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
