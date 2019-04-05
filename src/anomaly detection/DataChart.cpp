#include "DataChart.h"
#include "AnomalyDetector.h"

DataChart::DataChart(VariablePointer varA, VariablePointer varB)
{
	this->m_varA = varA;
	this->m_varB = varB;

	m_aggregatedPoints = new std::vector<Datapoint*>();
}

void DataChart::SetPointers(VariablePointer varA, VariablePointer varB)
{
	this->m_varA = varA;
	this->m_varB = varB;
}

void DataChart::LogData(int tick, Datapoint* _aggregatedDatapoint, std::vector<Datapoint*> _subDatapoints)
{
#if FILTER_POINTS
	isDirty = false;
#endif

	if (_aggregatedDatapoint == 0)
		return;

#if FILTER_POINTS

#if USE_SUBPOINTS
	// Get the subvalues from the previous frame
	std::vector<Datapoint*> prev_subPoints;
	if (m_aggregatedPoints->size() > 0 && m_subPoints.find(m_aggregatedPoints->back()) != m_subPoints.end())
		prev_subPoints = m_subPoints.at(m_aggregatedPoints->back());

	int count = 0;
	// Flag any points different from the previous frame as dirty
	for (int i = 0; i < _subDatapoints.size() && i < prev_subPoints.size(); ++i)
	{
		Datapoint* subpoint = _subDatapoints.at(i);
		// If we're tracking the valueCount, just look at Y-position
		if (m_varA.GetName() == "ValueCount" || m_varA.GetPointers().at(0) == (size_t*)&valueCount)
			subpoint->isDirty = subpoint->position.Y != prev_subPoints.at(i)->position.Y;
		// Otherwise look at x,y position
		else
			subpoint->isDirty = subpoint->position != prev_subPoints.at(i)->position;

		if (subpoint->isDirty)
			count++;
	}

	// None of the variables were dirty, so return
	if (m_aggregatedPoints->size() > 0 && _subDatapoints.size() > 0 && count == 0)
		return;
#endif
	// Only add points if the previous one was different
	if (m_aggregatedPoints->size() > 0)
	{
		// Make sure the previous value is different
		if (m_aggregatedPoints->back()->position == _aggregatedDatapoint->position)
		{
			delete _aggregatedDatapoint;
			return;
		}

		// If we're tracking the valueCount, only check the y-value
		if (m_varA.GetName() == "ValueCount" || m_varA.GetPointers().at(0) == (size_t*)&valueCount)
			if (m_aggregatedPoints->back()->position.Y == _aggregatedDatapoint->position.Y)
			{
				delete _aggregatedDatapoint;
				return;
			}
	}
#endif

	m_aggregatedPoints->push_back(_aggregatedDatapoint);
	m_tickToPoint.emplace(tick, _aggregatedDatapoint);
#if USE_SUBPOINTS
	// Map the index of the aggregatedPoints to a list of subPoints in subvalues
	m_subPoints.emplace(_aggregatedDatapoint, _subDatapoints);
#endif

#if FILTER_POINTS
	valueCount = m_aggregatedPoints->size();
	isDirty = true;
#endif
}

void DataChart::LogData(int tick)
{
	// Get the previous sub points
	std::vector<Datapoint*> subPoints;
#if USE_SUBPOINTS /// <USE_SUBPOINTS>
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
	Vector2 aggregatedPoint = Vector2(0, 0);

	// Get the subvalues from the previous frame
	std::vector<Datapoint*> prev_subPoints;
	if (m_aggregatedPoints->size() > 0 && m_subPoints.find(m_aggregatedPoints->back()) != m_subPoints.end())
		prev_subPoints = m_subPoints.at(m_aggregatedPoints->back());

	int count = 0;
	// Calculate our aggregated point using only the dirty subpoints
	for (int i = 0; i < subPoints.size() && i < prev_subPoints.size(); ++i)
	{
		Datapoint* subpoint = subPoints.at(i);
		bool isDirty = false;
		// If we're tracking the valueCount, just look at Y-position
		if (m_varA.GetName() == "ValueCount" || m_varA.GetPointers().at(0) == (size_t*)&valueCount)
			isDirty = subpoint->position.Y != prev_subPoints.at(i)->position.Y;
		// Otherwise look at x,y position
		else
			isDirty = subpoint->position != prev_subPoints.at(i)->position;

		if (isDirty)
		{
			aggregatedPoint += subpoint->position;
			count++;
		}
	}

	if (count > 0)
		aggregatedPoint /= (float)count;

	Datapoint* aggregatedDatapoint = new Datapoint(aggregatedPoint.X, aggregatedPoint.Y, tick);
#else
	Datapoint* aggregatedDatapoint = new Datapoint(m_varA.GetAvgValue(), m_varB.GetAvgValue(), tick);
#endif

#else /// </USE_SUBPOINTS>
	Datapoint* aggregatedDatapoint = new Datapoint(m_varA.GetSumValue(), m_varB.GetSumValue(), tick);
#endif

	LogData(tick, aggregatedDatapoint, subPoints);
}

std::string DataChart::Serialize()
{
	std::ostringstream stringstream;

	// Write the labels in the first line
	stringstream << GetLabelString() << "\n";

	// Loop through all the values
	for (int i = 0; i < m_aggregatedPoints->size(); ++i)
	{
		//Serialize the tick frame of this event
		stringstream << m_aggregatedPoints->at(i)->tick << " ";
		// Serialize the aggregated values
		stringstream << m_aggregatedPoints->at(i)->position.X << " " << m_aggregatedPoints->at(i)->position.Y << " ";
#if USE_SUBPOINTS
		// Serialize the subvalues
		std::vector<Datapoint*> currentSubvalues = m_subPoints[m_aggregatedPoints->at(i)];
		for (int j = 0; j < currentSubvalues.size(); ++j)
			stringstream << currentSubvalues.at(j)->position.X << " " << currentSubvalues.at(j)->position.Y << " ";
#endif

		// End the line
		stringstream << "\n";
	}

	return stringstream.str();
}

void DataChart::DeSerialize(const char* path)
{
	m_aggregatedPoints->clear();

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
		m_aggregatedPoints->push_back(aggregatedValue);
		m_tickToPoint.emplace(tick, aggregatedValue);

#if USE_SUBPOINTS
		std::vector<Datapoint*> subValueList;

		for (int i = 3; i < results.size(); i += 2)
		{
			float subA = std::stof(results[i]);
			float subB = std::stof(results[i + 1]);
			subValueList.push_back(new Datapoint(subA, subB, tick));
		}
		m_subPoints.emplace(aggregatedValue, subValueList);
#endif
	}
}

std::string DataChart::GetLabelString()
{
	std::ostringstream stringstream;

	stringstream << "# " << m_varA.GetName() << " " << m_varB.GetName();

	return stringstream.str();
}

void DataChart::DeleteAllData()
{
#if USE_SUBPOINTS
	// Delete all the sub points
	for (std::map<Datapoint*, std::vector<Datapoint*>>::iterator sub = m_subPoints.begin(); sub != m_subPoints.end(); ++sub)
	{
		for (std::vector<Datapoint*>::iterator value = m_subPoints.at(sub->first).begin(); value != m_subPoints.at(sub->first).end(); ++value)
			delete(*value);
		m_subPoints.at(sub->first).clear();
		m_subPoints.at(sub->first).shrink_to_fit();
	}
#endif

	// Delete all the aggregated values
	for (std::vector<Datapoint*>::iterator value = m_aggregatedPoints->begin(); value != m_aggregatedPoints->end(); ++value)
		delete(*value);
	m_aggregatedPoints->clear();
	m_aggregatedPoints->shrink_to_fit();
}

DataChart::~DataChart()
{
	// Delete the aggregatedPoints list
	delete m_aggregatedPoints;
}
