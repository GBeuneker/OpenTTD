#include "AnomalyDetector.h"

using namespace std;

AnomalyDetector* AnomalyDetector::instance;

AnomalyDetector::AnomalyDetector()
{

	ticks = 0;
#if USE_KNN
	this->knn = new KNN(new uint16_t[10]{ 44,9,131,15,50,175,58,140,23,141 });
#elif USE_LOF
	this->lof = new LOF(new uint16_t[10]{ 33,9,131,15,37,131,43,140,23,141 });
#elif USE_LOCI
	this->loci = new LOCI();
#elif USE_SOM
	this->som = new SOM(40, 40, 0.5);
#endif
	AnalyzeCharts(DeSerialize("KNN\\seed_100_a_10.000000_t_15.000000_w_5000_k_0.500000"));

#if 0
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
#elif 0
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
#endif
}
#pragma region Variable Tracking

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

#pragma endregion

#pragma region Anomaly Detection

/// <summary>Logs the data in our anomaly detection algorithms.</summary>
void AnomalyDetector::LogDataTick()
{
	// Skip first 10 ticks
	if (ticks < 10)
	{
		ticks++;
		return;
	}

	// Build data charts on the first tick
	if (ticks == 10)
		BuildCharts();

	bool eventTriggered = false;
	// Log new values
	for (int i = 0; i < m_datacharts.size(); ++i)
	{
		m_datacharts[i]->LogData(ticks);

		// Check if one of the charts triggered an event
		if (!eventTriggered && m_datacharts[i]->isDirty)
			eventTriggered = true;
	}

	if (eventTriggered)
		events++;

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

	// Do not report anomalies during training time
	if (ticks >= TRAINING_TIME)
		DetectAnomaly(results);

	if (ticks >= MAX_TICK_COUNT)
	{
		Serialize();
#if USE_SOM
		this->som->Serialize();
#endif
		exit(0);
	}

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

	// If anomaly score is greater than threshold
	if (anomalyScore >= GetThreshold())
	{
		printf("ANOMALY DETECTED! | Tick: %i | Total score: %f\n", ticks, anomalyScore);
		for (int i = 0; i < results.size(); ++i)
			if (results[i].isAnomaly)
				printf("|    Chart %i: %s | Value: %i | Score: %f\n", i, m_datacharts[i]->GetLabelString().c_str(), m_datacharts[i]->GetValues()->size() - 1, results[i].certainty);
	}
	else if (anomalyScore > 0.1f)
	{
		printf("No amomalies | Tick: %i | Score: %f\n", ticks, anomalyScore);
		for (int i = 0; i < results.size(); ++i)
			if (results[i].isAnomaly)
				printf("|    Chart %i: %s | Value: %i | Score: %f\n", i, m_datacharts[i]->GetLabelString().c_str(), m_datacharts[i]->GetValues()->size() - 1, results[i].certainty);
	}

	LogAnomalyScore(ticks, anomalyScore);
}

void AnomalyDetector::AnalyzeCharts(std::vector<DataChart*> charts)
{
	std::vector<DataChart*> tempCharts;
	for (int i = 0; i < charts.size(); ++i)
		tempCharts.push_back(new DataChart());

	knn->SetData(tempCharts);
	std::vector<Classification> results;
	// Go through the ticks backwards
	for (int tick = 0; tick < MAX_TICK_COUNT; ++tick)
	{
		for (int i = 0; i < charts.size(); ++i)
		{
			if (charts.at(i)->GetValueAt(tick) != 0)
				tempCharts.at(i)->LogData(tick);
		}

#if USE_KNN
		results = knn->Run();
#elif USE_LOF
		results = lof->Run();
#elif USE_LOCI
		results = loci->Run();
#elif USE_SOM
		results = som->Run();
#endif
		float anomalyScore = 0;
		for (int i = 0; i < results.size(); ++i)
		{
			if (results[i].isAnomaly)
				anomalyScore += results[i].certainty;
		}
		LogAnomalyScore(tick, anomalyScore);
	}

	// Destroy the charts and free up memory
	for (int i = 0; i < charts.size(); ++i)
	{
		delete charts.at(i);
		delete tempCharts.at(i);
	}
}


void AnomalyDetector::AnalyzeAllData()
{
	string spath = ".\\..\\_data\\";
	const char* folderpath = spath.c_str();
	if (mkdir(folderpath) == 0)
	{
		printf("ERROR: Path %s could not be found!", folderpath);
		return;
	}

	for (const auto & entry : std::filesystem::recursive_directory_iterator(folderpath))
	{
		string spath = entry.path().string();
		if (spath.find("anomaly_scores.dat") != std::string::npos)
			AnalyzeData(spath);
	}
}

void AnomalyDetector::AnalyzeData(string spath)
{
	if (spath.find("anomaly_scores.dat") == std::string::npos)
	{
		printf("ERROR: %s is not anomaly score data");
		return;
	}

	int tp = 0, fp = 0, fn = 0, tn = 0;
	int prev_anomalyTick = 0;

	// Open the file
	std::ifstream infile(spath);
	std::string line;
	// Read the file
	while (std::getline(infile, line))
	{
		int a;
		float b, c;
		std::istringstream iss(line);

		// Try and parse 3 columns
		if (iss >> a >> b >> c)
		{
			// Anomaly line is not empty
			prev_anomalyTick = a;
			fn++;
		}

		// Check if anomaly was detected
		if (b >= 2)
		{
			// Check if anomaly was detected within range of inserted anomaly
			if (a - prev_anomalyTick <= 10)
			{
				tp++;
				fn--;
			}
			else
				fp++;
		}
	}
	tn = events - fp - fn - tp;

	ofstream datafile;
	string outputPath = spath.substr(0, spath.length() - ((string)"anomaly_scores.dat").length()) + "\\anomaly_output.dat";
	datafile.open(outputPath, ofstream::trunc);
	datafile << "tp: " << tp << " fp: " << fp << " fn: " << fn << " tn: " << tn;
	datafile.close();
}

void AnomalyDetector::LogAnomalyScore(uint32_t tick, float score)
{
	m_anomalyScores.push_back(std::make_tuple(tick, score));
}

#pragma endregion

#pragma region Serialization

/// <summary>Serializes the entire data charts.</summary>
void AnomalyDetector::Serialize()
{
	// Serialize the data
#if ENABLE_ANOMALIES
	string spath = GetBaseFolder() + GetDataPath();
#else
	string spath = GetBaseFolder() + "seed_" + to_string(_random.seed) + "_baseline";
#endif

	if (mkdir(spath.c_str()) == 0)
		printf("Directory: \'%s\' was successfully created", spath.c_str());

	ofstream datafile;
	for (int i = 0; i < m_datacharts.size(); ++i)
	{
		// Open the file and start writing stream
		char src[255];
		sprintf(src, "%s\\data_%i.dat", spath.c_str(), i);
		datafile.open(src, ofstream::trunc);

		datafile << m_datacharts[i]->Serialize();

		// Close the file when writing is done
		datafile.close();
	}

	// Open the file and start writing stream
	string src = spath + "\\anomaly_scores.dat";
	datafile.open(src, ofstream::trunc);

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

	AnalyzeData(src);

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
		if (spath.find("data_") != std::string::npos)
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

#pragma endregion

#pragma region Anomaly Triggers

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

#pragma endregion

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
