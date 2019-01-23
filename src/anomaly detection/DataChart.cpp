#include "DataChart.h"

DataChart::DataChart(VariablePointer varA, VariablePointer varB)
{
	this->m_varA = varA;
	this->m_varB = varB;
}

void DataChart::LogData()
{
	values.push_back(Vector2(m_varA.GetValue(), m_varB.GetValue()));
}

std::string DataChart::SerializeLine()
{
	std::ostringstream stringstream;

	stringstream << values.back().X << " " << values.back().Y << "\n";

	return stringstream.str();
}

std::string DataChart::SerializeFull()
{
	std::ostringstream stringstream;

	// Write the labels in the first line
	stringstream << GetLabelString();

	// Loop through all the values
	for (int i = 0; i < values.size(); ++i)
		stringstream << values[i].X << " " << values[i].Y << "\n";

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
}
