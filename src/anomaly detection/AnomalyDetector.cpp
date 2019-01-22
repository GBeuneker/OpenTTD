#include "AnomalyDetector.h"

using namespace std;

AnomalyDetector* AnomalyDetector::instance;

AnomalyDetector::AnomalyDetector()
{
	ticks = 0;
}

AnomalyDetector::~AnomalyDetector()
{
}

void AnomalyDetector::BuildCharts()
{
	// Build charts based on all combinations of variables
	for (int i = 0; i < m_variables.size(); ++i)
		for (int j = i + 1; j < m_variables.size(); ++j)
		{
			m_datacharts.push_back(new DataChart(m_variables[i], m_variables[j]));
		}

	chartsBuilt = true;
}

/// <summary>Logs the data in our anomaly detection algorithms.</summary>
void AnomalyDetector::LogDataTick()
{
	// Build data charts on the first tick
	if (ticks == 0)
		BuildCharts();

	for (int i = 0; i < m_datacharts.size(); ++i)
		m_datacharts[i]->LogData();

	ticks++;
}

/// <summary>Serializes a single line.</summary>
void AnomalyDetector::SerializeLine()
{
	string spath = ".\\..\\_data\\seed_" + to_string(_random.seed);
	const char* path = spath.c_str();
	if (mkdir(path) == 0)
		printf("Directory: \'%s\' was successfully created", path);

	ofstream datafile;
	for (int i = 0; i < m_datacharts.size(); ++i)
	{
		// Open the file and start writing stream
		char src[60];
		sprintf(src, "%s\\data_%i.dat", path, i);
		datafile.open(src, ofstream::app);
		if (ticks == 0)
		{
			datafile.clear();
			datafile << m_datacharts[i]->GetLabelString();
		}

		datafile << m_datacharts[i]->SerializeLine();

		// Close the file when writing is done
		datafile.close();
	}
}

/// <summary>Serializes the entire data charts.</summary>
void AnomalyDetector::SerializeFull()
{
	string spath = ".\\..\\_data\\seed_" + to_string(_random.seed);
	const char* path = spath.c_str();
	if (mkdir(path) == 0)
		printf("Directory: \'%s\' was successfully created", path);

	ofstream datafile;
	for (int i = 0; i < m_datacharts.size(); ++i)
	{
		// Open the file and start writing stream
		char src[60];
		sprintf(src, "%s\\data_%i.dat", path, i);
		datafile.open(src, ofstream::trunc);

		datafile << m_datacharts[i]->SerializeFull();

		// Close the file when writing is done
		datafile.close();
	}
}
bool AnomalyDetector::TriggerVariableIncrease(int chance)
{
	bool canTrigger = _random.Next(100) < chance;
	if (canTrigger)
		printf("Variable Increase triggered at: %i\n", ticks);

	return canTrigger;
}

bool AnomalyDetector::TriggerVariableReset(int chance)
{
	bool canTrigger = _random.Next(100) < chance;
	if (canTrigger)
		printf("Variable Reset triggered at: %i\n", ticks);

	return canTrigger;
}

bool AnomalyDetector::TriggerFunctionFailure(int chance)
{
	bool canTrigger = _random.Next(100) < chance;
	if (canTrigger)
		printf("Funtion Failure triggered at: %i\n", ticks);

	return canTrigger;
}

void AnomalyDetector::TrackPointer(size_t* var, char* name)
{
	m_variables.push_back(VariablePointer(var, name));
}

void AnomalyDetector::Reset()
{
	m_variables.clear();
	ticks = 0;

	m_datacharts.clear();
	chartsBuilt = false;

	// Track your own ticks
	TrackPointer((size_t*)&ticks, "Ticks");
}
