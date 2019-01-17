#include "DataChart.h"

DataChart::DataChart(VariablePointer varA, VariablePointer varB)
{
	this->m_varA = varA;
	this->m_varB = varB;
}

void DataChart::LogData()
{
	xAxis.push_back(m_varA.GetValue());
	yAxis.push_back(m_varB.GetValue());
}

std::string DataChart::SerializeLine()
{
	std::ostringstream stringstream;

	stringstream << xAxis.back() << " " << yAxis.back() << "\n";

	return stringstream.str();
}

std::string DataChart::SerializeFull()
{
	std::ostringstream stringstream;

	// Write the labels in the first line
	stringstream << GetLabelString();

	// Loop through all the values
	for (int i = 0; i < xAxis.size() && i < yAxis.size(); ++i)
		stringstream << xAxis[i] << " " << yAxis[i] << "\n";

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
