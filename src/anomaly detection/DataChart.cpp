#include "DataChart.h"
#include "AnomalyDetector.h"

DataChart::DataChart(VariablePointer varA, VariablePointer varB)
{
	this->m_varA = varA;
	this->m_varB = varB;

	aggregatedValues = new std::vector<Datapoint*>();
}

void DataChart::SetPointers(VariablePointer varA, VariablePointer varB)
{
	this->m_varA = varA;
	this->m_varB = varB;
}

void DataChart::LogData(int tick, Datapoint* aggregatedDatapoint, std::vector<Datapoint*> subValues)
{
#if FILTER_POINTS && USE_SUBVALUES
	// Get the subvalues from the previous frame
	std::vector<Datapoint*> prev_subvalues;
	if (aggregatedValues->size() > 0 && subvalues.find(aggregatedValues->back()) != subvalues.end())
		prev_subvalues = subvalues.at(aggregatedValues->back());

	int count = 0;
	// Flag any points different from the previous frame as dirty
	for (int i = 0; i < subValues.size() && i < prev_subvalues.size(); ++i)
	{
		Datapoint* subpoint = subValues.at(i);
		// If we're tracking the valueCount, just look at Y-position
		if (m_varA.GetName() == "ValueCount" || m_varA.GetPointers().at(0) == (size_t*)&valueCount)
			subpoint->isDirty = subpoint->position.Y != prev_subvalues.at(i)->position.Y;
		// Otherwise look at x,y position
		else
			subpoint->isDirty = subpoint->position != prev_subvalues.at(i)->position;

		if (subpoint->isDirty)
			count++;

	}

	// None of the variables were dirty, so return
	if (aggregatedValues->size() > 0 && count == 0)
	{
		isDirty = false;
		return;
	}
#endif

#if FILTER_POINTS
	isDirty = false;
	// Only add points if the previous one was different
	if (aggregatedValues->size() > 0)
	{
		// Make sure the previous value is different
		if (aggregatedValues->back()->position == aggregatedDatapoint->position)
		{
			delete aggregatedDatapoint;
			return;
		}

		// If we're tracking the valueCount, only check the y-value
		if (m_varA.GetPointers().at(0) == (size_t*)&valueCount)
			if (aggregatedValues->back()->position.Y == aggregatedDatapoint->position.Y)
			{
				delete aggregatedDatapoint;
				return;
			}
	}
#endif

	aggregatedValues->push_back(aggregatedDatapoint);
	tickToValue.emplace(tick, aggregatedDatapoint);
#if USE_SUBVALUES
	// Map the index of the aggregatedValues to a list of subPoints in subvalues
	subvalues.emplace(aggregatedDatapoint, subValues);
#endif

#if FILTER_POINTS
	valueCount = aggregatedValues->size();
	isDirty = true;
#endif
}

void DataChart::LogData(int tick)
{
#if USE_SUBVALUES
	// Get the previous sub points
	std::vector<Datapoint*> subPoints;

	// Both pointer lists are the same size, match 1-1
	if (m_varA.GetPointers().size() == m_varB.GetPointers().size())
		for (int i = 0; i < m_varA.GetPointers().size(); ++i)
			subPoints.push_back(new Datapoint(m_varA.GetValueAt(i), m_varB.GetValueAt(i), tick));

	// Pointer lists are different sizes, make all possible combinations
	else
		for (int a = 0; a < m_varA.GetPointers().size(); ++a)
			for (int b = 0; b < m_varB.GetPointers().size(); ++b)
				subPoints.push_back(new Datapoint(m_varA.GetValueAt(a), m_varB.GetValueAt(b), tick));
#if FILTER_POINTS
	// The average position of the variable
	Vector2 avgVariableposition = Vector2(0, 0);

	// Get the subvalues from the previous frame
	std::vector<Datapoint*> prev_subvalues;
	if (aggregatedValues->size() > 0 && subvalues.find(aggregatedValues->back()) != subvalues.end())
		prev_subvalues = subvalues.at(aggregatedValues->back());

	int count = 0;
	// Flag any points different from the previous frame as dirty
	for (int i = 0; i < subPoints.size() && i < prev_subvalues.size(); ++i)
	{
		Datapoint* subpoint = subPoints.at(i);
		// If we're tracking the valueCount, just look at Y-position
		if (m_varA.GetPointers().at(0) == (size_t*)&valueCount)
			subpoint->isDirty = subpoint->position.Y != prev_subvalues.at(i)->position.Y;
		// Otherwise look at x,y position
		else
			subpoint->isDirty = subpoint->position != prev_subvalues.at(i)->position;

		if (subpoint->isDirty)
		{
			avgVariableposition += subpoint->position;
			count++;
		}
	}

	// None of the variables were dirty, so return
	if (aggregatedValues->size() > 0 && count == 0)
	{
		isDirty = false;
		return;
	}

	// Only divide if there was a count
	if (count > 0)
		avgVariableposition /= (float)count;

	Datapoint* aggregatedDatapoint = new Datapoint(avgVariableposition.X, avgVariableposition.Y, tick);
#else
	Datapoint* aggregatedDatapoint = new Datapoint(m_varA.GetAvgValue(), m_varB.GetAvgValue(), tick);
#endif
#else
	Datapoint* aggregatedDatapoint = new Datapoint(m_varA.GetSumValue(), m_varB.GetSumValue(), tick);
#endif

#if FILTER_POINTS
	isDirty = false;
	// Only add points if the previous one was different
	if (aggregatedValues->size() > 0)
	{
		// Make sure the previous value is different
		if (aggregatedValues->back()->position == aggregatedDatapoint->position)
		{
			delete aggregatedDatapoint;
			return;
		}

		// If we're tracking the valueCount, only check the y-value
		if (m_varA.GetPointers().at(0) == (size_t*)&valueCount)
			if (aggregatedValues->back()->position.Y == aggregatedDatapoint->position.Y)
			{
				delete aggregatedDatapoint;
				return;
			}
	}
#endif

	aggregatedValues->push_back(aggregatedDatapoint);
	tickToValue.emplace(tick, aggregatedDatapoint);
#if USE_SUBVALUES
	// Map the index of the aggregatedValues to a list of subPoints in subvalues
	subvalues.emplace(aggregatedDatapoint, subPoints);
#endif

#if FILTER_POINTS
	valueCount = aggregatedValues->size();
	isDirty = true;
#endif
}

std::string DataChart::Serialize()
{
	std::ostringstream stringstream;

	// Write the labels in the first line
	stringstream << GetLabelString() << "\n";

	// Loop through all the values
	for (int i = 0; i < aggregatedValues->size(); ++i)
	{
		//Serialize the tick frame of this event
		stringstream << aggregatedValues->at(i)->tick << " ";
		// Serialize the aggregated values
		stringstream << aggregatedValues->at(i)->position.X << " " << aggregatedValues->at(i)->position.Y << " ";
		// Serialize the subvalues
		std::vector<Datapoint*> currentSubvalues = subvalues[aggregatedValues->at(i)];
		for (int j = 0; j < currentSubvalues.size(); ++j)
			stringstream << currentSubvalues.at(j)->position.X << " " << currentSubvalues.at(j)->position.Y << " ";

		// End the line
		stringstream << "\n";
	}

	return stringstream.str();
}

void DataChart::DeSerialize(const char* path)
{
	aggregatedValues->clear();

	std::ifstream infile(path);

	std::string line;
	// Read the variable name values
	std::getline(infile, line);
	std::istringstream iss(line);
	std::vector<std::string> results(std::istream_iterator<std::string>{iss},
		std::istream_iterator<std::string>());
	m_varA = VariablePointer(0, results.at(1));
	m_varB = VariablePointer(0, results.at(2));

	// Read the rest
	while (std::getline(infile, line))
	{
		std::istringstream iss(line);
		std::vector<std::string> results(std::istream_iterator<std::string>{iss},
			std::istream_iterator<std::string>());

		int tick = std::stoi(results[0]);
		float valA = std::stof(results[1]);
		float valB = std::stof(results[2]);
		Datapoint* aggregatedValue = new Datapoint(valA, valB, tick);
		aggregatedValues->push_back(aggregatedValue);
		tickToValue.emplace(tick, aggregatedValue);

		std::vector<Datapoint*> subValueList;

		for (int i = 3; i < results.size(); i += 2)
		{
			float subA = std::stof(results[i]);
			float subB = std::stof(results[i + 1]);
			subValueList.push_back(new Datapoint(subA, subB, tick));
		}
		subvalues.emplace(aggregatedValue, subValueList);
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
	delete aggregatedValues;
}
