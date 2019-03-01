#include "AnomalyDetector.h"

using namespace std;

AnomalyDetector* AnomalyDetector::instance;

AnomalyDetector::AnomalyDetector()
{
	ticks = 0;
#if USE_KNN
	this->knn = new KNN(new uint16_t[10]{ 10,5,20,5,10,20,10,15,5,20 });
#elif USE_LOF
	this->lof = new LOF(new uint16_t[10]{ 10,5,20,5,10,20,10,15,5,20 });
#elif USE_LOCI
	this->loci = new LOCI(new uint16_t[10]{ 10,5,20,5,10,20,10,15,5,20 });
#elif USE_SOM
	this->som = new SOM(40, 40, 0.5);

#endif

	DataChart* test = new DataChart();
	m_datacharts.push_back(test);
#if USE_KNN
	this->knn->SetData(m_datacharts);
#elif USE_LOF
	this->lof->SetData(m_datacharts);
#elif USE_LOCI
	this->loci->SetData(m_datacharts);
#endif
#if 0
	// Test 1
	for (int i = 0; i <= 9; ++i)
	{
		test->GetValues()->push_back(new Datapoint(i, 0));
#if USE_KNN
		this->knn->Run();
#elif USE_LOF
		this->lof->Run();
#elif USE_LOCI
		this->loci->Run();
#endif
	}
#elif 0
	// Test 2
	for (int i = 0; i <= 9; ++i)
	{
		test->GetValues()->push_back(new Datapoint(i, i == 7 || i == 9 ? 1 : 0));
#if USE_KNN
		this->knn->Run();
#elif USE_LOF
		this->lof->Run();
#elif USE_LOCI
		this->loci->Run();
#endif
	}
#elif 1
	// Test 3
	test->GetValues()->push_back(new Datapoint(0, 0)); // 0
	test->GetValues()->push_back(new Datapoint(1, 0)); // 1
	test->GetValues()->push_back(new Datapoint(0.5, 0.5)); // 2
	test->GetValues()->push_back(new Datapoint(0, 1)); // 3
	test->GetValues()->push_back(new Datapoint(1, 1)); // 4
	test->GetValues()->push_back(new Datapoint(0.25, 1.5)); // 5
#if USE_KNN
	this->knn->Run();
#elif USE_LOF
	this->lof->Run();
#elif USE_LOCI
	this->loci->Run();
#endif
	test->GetValues()->push_back(new Datapoint(0.75, 1.5)); // 6
#if USE_KNN
	this->knn->Run();
#elif USE_LOF
	this->lof->Run();
#elif USE_LOCI
	this->loci->Run();
#endif
	test->GetValues()->push_back(new Datapoint(0, 2)); // 7
#if USE_KNN
	this->knn->Run();
#elif USE_LOF
	this->lof->Run();
#elif USE_LOCI
	this->loci->Run();
#endif
	test->GetValues()->push_back(new Datapoint(1, 2)); // 8
#if USE_KNN
	this->knn->Run();
#elif USE_LOF
	this->lof->Run();
#elif USE_LOCI
	this->loci->Run();
#endif
	test->GetValues()->push_back(new Datapoint(8, 1)); // 9
#if USE_KNN
	this->knn->Run();
#elif USE_LOF
	this->lof->Run();
#elif USE_LOCI
	this->loci->Run();
#endif

#endif
}

/// <summary>Build the data charts by combining all variables.</summary>
void AnomalyDetector::BuildCharts()
{
#if FILTER_POINTS
	// Add datacharts tracking the amount of variables
	for (int i = 0; i < m_variables.size(); ++i)
	{
		DataChart* dc = new DataChart();
		dc->SetPointers(VariablePointer((size_t*)&dc->valueCount, "ValueCount"), m_variables[i]);
		m_datacharts.push_back(dc);
	}
#endif

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
	ticks++;

	// Build data charts on the first tick
	if (ticks < 10)
		return;
	else if (ticks == 10)
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

	if (ticks >= MAX_TICK_COUNT)
	{
		Serialize();
#if USE_SOM
		this->som->Serialize();
#endif
		exit(0);
	}
}

void AnomalyDetector::DetectAnomaly(std::vector<Classification> results)
{
	float anomalyScore = 0;
	for (int i = 0; i < results.size(); ++i)
	{
		if (results[i].isAnomaly)
			anomalyScore += results[i].certainty;
	}

	// If anomaly score is greater than threshold
	if (anomalyScore >= GetThreshold())
	{
		printf("ANOMALY DETECTED! | Tick: %i | Total score: %f\n", ticks, anomalyScore);
		for (int i = 0; i < results.size(); ++i)
			if (results[i].isAnomaly)
				printf("|    Chart %i: %s | Value: %i | Score: %f\n", i, m_datacharts[i]->GetLabelString().c_str(), m_datacharts[i]->GetValues()->size(), results[i].certainty);
	}
	else if (anomalyScore > 0)
		printf("No amomalies | Tick: %i | Score: %f\n", ticks, anomalyScore);

	LogAnomalyScore(ticks, anomalyScore);
}

void AnomalyDetector::LogAnomalyScore(uint32_t tick, float score)
{
	m_anomalyScores.push_back(std::make_tuple(tick, score));
}

/// <summary>Serializes the entire data charts.</summary>
void AnomalyDetector::Serialize()
{
	// Serialize the data
#if ENABLE_ANOMALIES
	string spath = GetBaseFolder() + "seed_" + to_string(_random.seed) + "_anomalous";
#else
	string spath = GetBaseFolder() + "seed_" + to_string(_random.seed) + "_baseline";
#endif

	if (mkdir(spath.c_str()) == 0)
		printf("Directory: \'%s\' was successfully created", spath.c_str());

	ofstream datafile;
	for (int i = 0; i < m_datacharts.size(); ++i)
	{
		// Open the file and start writing stream
		char src[60];
		sprintf(src, "%s\\data_%i.dat", spath.c_str(), i);
		datafile.open(src, ofstream::trunc);

		datafile << m_datacharts[i]->Serialize();

		// Close the file when writing is done
		datafile.close();
	}

	// Seriaize the anomalies
#if ENABLE_ANOMALIES
	spath = GetBaseFolder() + "seed_" + to_string(_random.seed) + "_anomalous";
#else
	spath = GetBaseFolder() + "seed_" + to_string(_random.seed) + "_baseline";
#endif
	if (mkdir(spath.c_str()) == 0)
		printf("Directory: \'%s\' was successfully created", spath.c_str());

	// Open the file and start writing stream
	string src = spath + "\\anomaly_scores.dat";
	datafile.open(src.c_str(), ofstream::trunc);

	// Write the ticks and the anomaly scores
	for (int i = 0; i < m_anomalyScores.size(); ++i)
	{
		int ticks = std::get<0>(m_anomalyScores.at(i));
		float anomalyScore = std::get<1>(m_anomalyScores.at(i));
		datafile << ticks << " " << anomalyScore << " ";

		if (m_anomalyOccurrences.find(ticks) != m_anomalyOccurrences.end())
		{
			string anomalyMsg = m_anomalyOccurrences.at(ticks);
			datafile << GetThreshold() << " " << anomalyMsg << endl;
		}

		datafile << endl;
	}

	// Close the file when writing is done
	datafile.close();

#if USE_SOM
	// Seriaize the SOM maps
	this->som->Serialize();
#endif
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
	{
		printf("Variable Increase triggered. Tick: %i | message: %s\n", ticks, msg);
		m_anomalyOccurrences.emplace(ticks, msg);
	}
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
	{
		printf("Variable Reset triggered. Tick: %i | message: %s\n", ticks, msg);
		m_anomalyOccurrences.emplace(ticks, msg);
	}
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
	{
		printf("Funtion Failure triggered. Tick: %i | message: %s\n", ticks, msg);
		m_anomalyOccurrences.emplace(ticks, msg);
	}
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
	bool foundElement = false;
	for (int i = 0; i < m_variables.size(); ++i)
	{
		if (strcmp(m_variables.at(i).GetName(), name) == 0)
		{
			m_variables.at(i).AddPointer(var);
			foundElement = true;
			return;
		}
	}

	if (!foundElement)
		m_variables.push_back(VariablePointer(var, name));
}

/// <summary>Resets all variables in the Anomaly Detector.</summary>
void AnomalyDetector::Reset()
{
	m_variables.clear();
	ticks = 0;

	m_datacharts.clear();
	chartsBuilt = false;

#if !FILTER_POINTS
	// Track your own ticks
	TrackPointer((size_t*)&ticks, "Ticks");
#endif
}

AnomalyDetector::~AnomalyDetector()
{
}
