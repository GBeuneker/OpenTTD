#include "DataChart.h"

DataChart::DataChart(VariablePointer varA, VariablePointer varB)
{
	this->m_varA = varA;
	this->m_varB = varB;

	values = new std::vector<Datapoint*>();
}

void DataChart::LogData()
{
	Vector2 position = Vector2(m_varA.GetValue(), m_varB.GetValue());

	Datapoint* newDataPoint = new Datapoint(position.X, position.Y);

	//// Only add unique values
	//for (int i = 0; i < values->size(); ++i)
	//	if (values->at(i)->position == newDataPoint->position)
	//	{
	//		values->at(i)->count++;
	//		return;
	//	}

	values->push_back(newDataPoint);

	// Get the datarange
	minX = fmin(position.X, minX);
	maxX = fmax(position.X, maxX);
	minY = fmin(position.Y, minY);
	maxY = fmax(position.Y, maxY);
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
