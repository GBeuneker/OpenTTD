#include "DataChart.h"

DataChart::DataChart(VariablePointer varA, VariablePointer varB)
{
	this->m_varA = varA;
	this->m_varB = varB;

	values = new std::vector<Datapoint*>();
}

void DataChart::SetPointers(VariablePointer varA, VariablePointer varB)
{
	this->m_varA = varA;
	this->m_varB = varB;
}

void DataChart::LogData()
{
	Vector2 position = Vector2(m_varA.GetValue(), m_varB.GetValue());
	Datapoint* newDataPoint = new Datapoint(position.X, position.Y);

#if FILTER_POINTS
	isDirty = false;
	// Only add points if the previous one was different
	if (values->size() > 0)
	{
		// Make sure the previous value is different
		if (values->back()->position == newDataPoint->position)
			return;

		// If we're tracking the valueCount, only check the y-value
		if (m_varA.GetPointers().at(0) == (size_t*)&valueCount)
			if (values->back()->position.Y == newDataPoint->position.Y)
				return;
	}

#endif

	values->push_back(newDataPoint);
#if FILTER_POINTS
	valueCount = values->size();
	isDirty = true;
#endif
}

std::string DataChart::Serialize()
{
	std::ostringstream stringstream;

	// Write the labels in the first line
	stringstream << GetLabelString() << "\n";

	// Loop through all the values
	for (int i = 0; i < values->size(); ++i)
		stringstream << values->at(i)->position.X << " " << values->at(i)->position.Y << "\n";

	return stringstream.str();
}

void DataChart::DeSerialize(const char* path)
{
	values->clear();

	std::ifstream infile(path);

	std::string line;
	//Skip first line
	std::getline(infile, line);
	// Read the rest
	while (std::getline(infile, line))
	{
		int a, b;
		std::istringstream iss(line);
		if (!(iss >> a >> b)) { break; }

		values->push_back(new Datapoint(a, b));
	}
}

std::string DataChart::GetLabelString()
{
	std::ostringstream stringstream;

	stringstream << "# " << m_varA.GetName() << " " << m_varB.GetName();

	return stringstream.str();
}

DataChart::~DataChart()
{
	delete values;
}
