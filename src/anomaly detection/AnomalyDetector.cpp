#include "AnomalyDetector.h"

using namespace std;

AnomalyDetector* AnomalyDetector::instance;

AnomalyDetector::AnomalyDetector()
{
	ticks = 0;
#if USE_KNN
	this->knn = new KNN(5);
#elif USE_LOF
	this->lof = new LOF(5);
#elif USE_LOCI
	this->loci = new LOCI();
#elif USE_SOM
	this->som = new SOM(40, 40, 0.5);
#endif
}

/// <summary>Build the data charts by combining all variables.</summary>
void AnomalyDetector::BuildCharts()
{
	// Build charts based on all combinations of variables
	for (int i = 0; i < m_variables.size(); ++i)
		for (int j = i + 1; j < m_variables.size(); ++j)
			m_datacharts.push_back(new DataChart(m_variables[i], m_variables[j]));

	// Add the data to the knn algorithm
#if USE_KNN
	knn->SetData(m_datacharts);
#elif USE_LOF
	lof->SetData(m_datacharts);
#elif USE_LOCI
	loci->SetData(m_datacharts);
#elif USE_SOM
	som->SetData(m_datacharts);
#endif

	chartsBuilt = true;
}

/// <summary>Logs the data in our anomaly detection algorithms.</summary>
void AnomalyDetector::LogDataTick()
{
	// Build data charts on the first tick
	if (ticks == 0)
		BuildCharts();

	// Log new values
	for (int i = 0; i < m_datacharts.size(); ++i)
		m_datacharts[i]->LogData();

	std::vector<Classification> results;
#if USE_KNN
	results = knn->Run();
#elif USE_LOF
	results = lof->Run();
#elif USE_LOCI
	results = loci->Run();
#elif USE_SOM
	results = som->Run();
#endif

	DetectAnomaly(results);

	ticks++;
}

void AnomalyDetector::DetectAnomaly(std::vector<Classification> results)
{
	float anomalyScore = 0;
	for (int i = 0; i < results.size(); ++i)
	{
		if (results[i].isAnomaly)
			anomalyScore += results[i].certainty;
	}

	// Threshold based on percentage of possible combinations that report anomalies
	float threshold = (m_variables.size() - 1) * 0.3f;
	// If anomaly score is greater than threshold
	if (anomalyScore >= threshold)
		printf("ANOMALY DETECTED! | Tick: %i | Score: %f\n", ticks, anomalyScore);
	else if (anomalyScore > 0)
		printf("No amomalies | Tick: %i | Score: %f\n", ticks, anomalyScore);
}

/// <summary>Serializes the entire data charts.</summary>
void AnomalyDetector::Serialize()
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

		datafile << m_datacharts[i]->Serialize();

		// Close the file when writing is done
		datafile.close();
	}
}

/// <summary>Deserializes a folder of data containing datacharts.</summary>
/// <param name='folder'>The folder we would like to use for deserialization.</param>
std::vector<DataChart*> AnomalyDetector::DeSerialize(const char* folder)
{
	std::vector<DataChart*> answer;

	string spath = ".\\..\\_data\\" + (string)folder;
	const char* path = spath.c_str();
	if (mkdir(path) == 0)
	{
		printf("ERROR: Path %s could not be found!", path);
		return answer;
	}

	for (const auto & entry : std::filesystem::directory_iterator(path))
	{
		string spath = entry.path().string();
		if (spath.substr(spath.length() - 4, 4) == ".dat")
		{
			const char* path = spath.c_str();

			DataChart* newChart = new DataChart();
			// Deserialize the data in the file
			newChart->DeSerialize(path);
			// Add the chart to the list
			answer.push_back(newChart);
		}
	}

	return answer;
}

/// <summary>Determines whether a variable increase anomaly can be triggered.</summary>
/// <param name='chance'>Chance the anomaly will be triggered (range: 0-100%)</param>
bool AnomalyDetector::TriggerVariableIncrease(float chance, char* msg)
{
	float randomValue = _random.Next(100000) / 1000.0f;

#if ENABLE_ANOMALIES
	bool canTrigger = randomValue < chance;
	if (canTrigger)
		printf("Variable Increase triggered. Tick: %i | message: %s\n", ticks, msg);
	return canTrigger;
#else
	return false;
#endif
}

/// <summary>Determines whether a variable reset anomaly can be triggered.</summary>
/// <param name='chance'>Chance the anomaly will be triggered (range: 0-100%)</param>
bool AnomalyDetector::TriggerVariableReset(float chance, char* msg)
{
	float randomValue = _random.Next(100000) / 1000.0f;

#if ENABLE_ANOMALIES
	bool canTrigger = randomValue < chance;
	if (canTrigger)
		printf("Variable Reset triggered. Tick: %i | message: %s\n", ticks, msg);
	return canTrigger;
#else
	return false;
#endif
}

/// <summary>Determines whether a function failure anomaly can be triggered.</summary>
/// <param name='chance'>Chance the anomaly will be triggered (range: 0-100%)</param>
bool AnomalyDetector::TriggerFunctionFailure(float chance, char* msg)
{
	float randomValue = _random.Next(100000) / 1000.0f;

#if ENABLE_ANOMALIES
	bool canTrigger = randomValue < chance;
	if (canTrigger)
		printf("Funtion Failure triggered. Tick: %i | message: %s\n", ticks, msg);
	return canTrigger;
#else
	return false;
#endif
}

/// <summary>Track the pointer of a variable for Anomaly Detection.</summary>
/// <param name='var'>The pointer we would like to track.</param>
/// <param name='name'>The name we can identify the variable with.</param>
void AnomalyDetector::TrackPointer(size_t* var, char* name)
{
	m_variables.push_back(VariablePointer(var, name));
}

/// <summary>Resets all variables in the Anomaly Detector.</summary>
void AnomalyDetector::Reset()
{
	m_variables.clear();
	ticks = 0;

	m_datacharts.clear();
	chartsBuilt = false;

	// Track your own ticks
	TrackPointer((size_t*)&ticks, "Ticks");
}

AnomalyDetector::~AnomalyDetector()
{
}
