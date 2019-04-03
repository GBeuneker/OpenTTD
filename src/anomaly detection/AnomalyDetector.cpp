#include "AnomalyDetector.h"

using namespace std;

AnomalyDetector* AnomalyDetector::instance;

AnomalyDetector::AnomalyDetector()
{
	m_ticks = 0;

	this->knn = new KNN(k_percentage);
	this->lof = new LOF(k_percentage);
	this->loci = new LOCI();
	this->som = new SOM(40, 40, 0.5);

	RunExperiments("BASE\\seed_100_a_10.000000_t_15.000000_w_5000_k_0.500000");
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
		if (strcmp(m_variables.at(i).GetName().c_str(), name) == 0)
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
	if (algorithm == Algorithm::KNN)
	{
		knn->SetData(m_datacharts);
		knn->SetParameters(threshold, windowSize);
	}
	else if (algorithm == Algorithm::LOF)
	{
		lof->SetData(m_datacharts);
		lof->SetParameters(threshold, windowSize);
	}
	else if (algorithm == Algorithm::LOCI)
	{
		loci->SetData(m_datacharts);
		loci->SetParameters(threshold, windowSize);
	}
	else if (algorithm == Algorithm::SOM)
	{
		som->SetData(m_datacharts);
		som->SetParameters(threshold, windowSize);
	}

	chartsBuilt = true;
}

#pragma endregion

#pragma region Anomaly Detection

void AnomalyDetector::RunExperiments(const char* path)
{
	std::vector<DataChart*> loadedCharts = DeSerializeCharts(path);
	std::map<int, std::string> anomalyOccurances = DeserializeAnomalyOccurences(path);
	RunExperiments(loadedCharts, anomalyOccurances);

	for (std::vector<DataChart*>::iterator it = loadedCharts.begin(); it != loadedCharts.end(); ++it)
		delete (*it);
}

void AnomalyDetector::RunExperiments(std::vector<DataChart*> loadedCharts, std::map<int, std::string> anomalyOccurences)
{
	this->knn->SetParameters(15, 5000);
	this->lof->SetParameters(15, 5000);
	this->loci->SetParameters(15, 5000);
	this->som->SetParameters(15, 5000);
	AnalyzeCharts(loadedCharts, anomalyOccurences);
	return;

	this->knn->SetParameters(5, 5000);
	this->lof->SetParameters(5, 5000);
	this->loci->SetParameters(5, 5000);
	this->som->SetParameters(5, 5000);
	algorithm = Algorithm::KNN;
	AnalyzeCharts(loadedCharts, anomalyOccurences);
	algorithm = Algorithm::LOF;
	AnalyzeCharts(loadedCharts, anomalyOccurences);
	algorithm = Algorithm::LOCI;
	AnalyzeCharts(loadedCharts, anomalyOccurences);
	algorithm = Algorithm::SOM;
	AnalyzeCharts(loadedCharts, anomalyOccurences);

	this->knn->SetParameters(15, 5000);
	this->lof->SetParameters(15, 5000);
	this->loci->SetParameters(15, 5000);
	this->som->SetParameters(15, 5000);
	algorithm = Algorithm::KNN;
	AnalyzeCharts(loadedCharts, anomalyOccurences);
	algorithm = Algorithm::LOF;
	AnalyzeCharts(loadedCharts, anomalyOccurences);
	algorithm = Algorithm::LOCI;
	AnalyzeCharts(loadedCharts, anomalyOccurences);
	algorithm = Algorithm::SOM;
	AnalyzeCharts(loadedCharts, anomalyOccurences);

	this->knn->SetParameters(50, 5000);
	this->lof->SetParameters(50, 5000);
	this->loci->SetParameters(50, 5000);
	this->som->SetParameters(50, 5000);
	algorithm = Algorithm::KNN;
	AnalyzeCharts(loadedCharts, anomalyOccurences);
	algorithm = Algorithm::LOF;
	AnalyzeCharts(loadedCharts, anomalyOccurences);
	algorithm = Algorithm::LOCI;
	AnalyzeCharts(loadedCharts, anomalyOccurences);
	algorithm = Algorithm::SOM;
	AnalyzeCharts(loadedCharts, anomalyOccurences);

	this->knn->SetParameters(15, 20);
	this->lof->SetParameters(15, 20);
	this->loci->SetParameters(15, 20);
	this->som->SetParameters(15, 20);
	algorithm = Algorithm::KNN;
	AnalyzeCharts(loadedCharts, anomalyOccurences);
	algorithm = Algorithm::LOF;
	AnalyzeCharts(loadedCharts, anomalyOccurences);
	algorithm = Algorithm::LOCI;
	AnalyzeCharts(loadedCharts, anomalyOccurences);
	algorithm = Algorithm::SOM;
	AnalyzeCharts(loadedCharts, anomalyOccurences);

	this->knn->SetParameters(15, 75);
	this->lof->SetParameters(15, 75);
	this->loci->SetParameters(15, 75);
	this->som->SetParameters(15, 75);
	algorithm = Algorithm::KNN;
	AnalyzeCharts(loadedCharts, anomalyOccurences);
	algorithm = Algorithm::LOF;
	AnalyzeCharts(loadedCharts, anomalyOccurences);
	algorithm = Algorithm::LOCI;
	AnalyzeCharts(loadedCharts, anomalyOccurences);
	algorithm = Algorithm::SOM;
	AnalyzeCharts(loadedCharts, anomalyOccurences);

	k_percentage = 0.25f;
	delete this->knn;
	this->knn = new KNN(k_percentage);
	delete this->lof;
	this->lof = new LOF(k_percentage);
	algorithm = Algorithm::KNN;
	AnalyzeCharts(loadedCharts, anomalyOccurences);
	algorithm = Algorithm::LOF;
	AnalyzeCharts(loadedCharts, anomalyOccurences);

	k_percentage = 0.50f;
	delete this->knn;
	this->knn = new KNN(k_percentage);
	delete this->lof;
	this->lof = new LOF(k_percentage);
	algorithm = Algorithm::KNN;
	AnalyzeCharts(loadedCharts, anomalyOccurences);
	algorithm = Algorithm::LOF;
	AnalyzeCharts(loadedCharts, anomalyOccurences);

	k_percentage = 0.75f;
	delete this->knn;
	this->knn = new KNN(k_percentage);
	delete this->lof;
	this->lof = new LOF(k_percentage);
	algorithm = Algorithm::KNN;
	AnalyzeCharts(loadedCharts, anomalyOccurences);
	algorithm = Algorithm::LOF;
	AnalyzeCharts(loadedCharts, anomalyOccurences);
}

/// <summary>Logs the data in our anomaly detection algorithms.</summary>
void AnomalyDetector::LogDataTick()
{
	// Skip first 10 ticks
	if (m_ticks < 10)
	{
		m_ticks++;
		return;
	}

	// Build data charts on the first tick
	if (m_ticks == 10)
		BuildCharts();

	bool eventTriggered = false;
	// Log new values
	for (int i = 0; i < m_datacharts.size(); ++i)
	{
		m_datacharts[i]->LogData(m_ticks);

		// Check if one of the charts triggered an event
		if (!eventTriggered && m_datacharts[i]->isDirty)
			eventTriggered = true;
	}

	if (eventTriggered)
		m_events++;

	std::vector<Classification> results;
	if (algorithm == Algorithm::KNN)
		results = knn->Run(m_ticks);
	else if (algorithm == Algorithm::LOF)
		results = lof->Run(m_ticks);
	else if (algorithm == Algorithm::LOCI)
		results = loci->Run(m_ticks);
	else if (algorithm == Algorithm::SOM)
		results = som->Run(m_ticks);

	// Do not report anomalies during training time
	if (m_ticks >= TRAINING_TIME)
		DetectAnomaly(results);

	if (m_ticks >= MAX_TICK_COUNT)
	{
		Serialize();
		if (algorithm == Algorithm::SOM)
			this->som->Serialize();
		exit(0);
	}

	m_ticks++;
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
	if (anomalyScore >= GetThreshold(m_datacharts.size()))
	{
		printf("ANOMALY DETECTED! | Tick: %i | Total score: %f\n", m_ticks, anomalyScore);
		for (int i = 0; i < results.size(); ++i)
			if (results[i].isAnomaly)
				printf("|    Chart %i: %s | Value: %i | Score: %f\n", i, m_datacharts[i]->GetLabelString().c_str(), m_datacharts[i]->GetValues()->size() - 1, results[i].certainty);
	}
	else if (anomalyScore > 0.1f)
	{
		printf("No amomalies | Tick: %i | Score: %f\n", m_ticks, anomalyScore);
		for (int i = 0; i < results.size(); ++i)
			if (results[i].isAnomaly)
				printf("|    Chart %i: %s | Value: %i | Score: %f\n", i, m_datacharts[i]->GetLabelString().c_str(), m_datacharts[i]->GetValues()->size() - 1, results[i].certainty);
	}

	LogAnomalyScore(m_ticks, anomalyScore);
}

void AnomalyDetector::AnalyzeCharts(std::vector<DataChart*> charts, std::map<int, std::string> anomalyOccurences)
{
	// Initialize the charts 
	std::vector<DataChart*> tempCharts;
	for (int i = 0; i < charts.size(); ++i)
	{
		DataChart* newChart = new DataChart();
		newChart->SetPointers(charts.at(i)->GetPointerA(), charts.at(i)->GetPointerB());
		tempCharts.push_back(newChart);
	}

	if (algorithm == Algorithm::KNN)
		knn->SetData(tempCharts);
	else if (algorithm == Algorithm::LOF)
		lof->SetData(tempCharts);
	else if (algorithm == Algorithm::LOCI)
		loci->SetData(tempCharts);
	else if (algorithm == Algorithm::SOM)
		som->SetData(tempCharts);

	std::vector<Classification> results;
	std::vector<std::tuple<int, float>> anomalyScores;
	int events = 0;
	// Go through the ticks backwards
	for (int tick = 0; tick < MAX_TICK_COUNT; ++tick)
	{
		this->m_ticks = tick;
		bool eventTriggered = false;
		for (int i = 0; i < charts.size(); ++i)
		{
			Datapoint* aggregatedVaue = charts.at(i)->GetValueAt(tick);
			tempCharts.at(i)->LogData(tick, aggregatedVaue, charts.at(i)->GetSubvalues(aggregatedVaue));
			if (tempCharts.at(i)->isDirty)
				eventTriggered = true;
		}
		if (eventTriggered)
			events++;

		if (algorithm == Algorithm::KNN)
			results = knn->Run(tick);
		else if (algorithm == Algorithm::LOF)
			results = lof->Run(tick);
		else if (algorithm == Algorithm::LOCI)
			results = loci->Run(tick);
		else if (algorithm == Algorithm::SOM)
			results = som->Run(tick);

		if (tick >= TRAINING_TIME)
		{
			float anomalyScore = 0;
			for (int i = 0; i < results.size(); ++i)
			{
				if (results[i].isAnomaly)
					anomalyScore += results[i].certainty;
			}
			anomalyScores.push_back(std::make_tuple(tick, anomalyScore));
		}
	}

	Serialize(tempCharts, anomalyScores, anomalyOccurences, events);

	// Destroy the charts and free up memory
	for (std::vector<DataChart*>::iterator it = tempCharts.begin(); it != tempCharts.end(); ++it)
		delete(*it);
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
			AnalyzeData(spath, m_events);
	}
}

void AnomalyDetector::AnalyzeData(string spath, int events)
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

void AnomalyDetector::Serialize()
{
	Serialize(m_datacharts, m_anomalyScores, m_anomalyOccurrences, m_events);
}

/// <summary>Serializes the entire data charts.</summary>
void AnomalyDetector::Serialize(std::vector<DataChart*> datacharts, std::vector<std::tuple<int, float>> anomalyScores, std::map<int, std::string> anomalyOccurances, int events)
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
	for (int i = 0; i < datacharts.size(); ++i)
	{
		// Open the file and start writing stream
		char src[255];
		sprintf(src, "%s\\data_%i.dat", spath.c_str(), i);
		datafile.open(src, ofstream::trunc);

		datafile << datacharts[i]->Serialize();

		// Close the file when writing is done
		datafile.close();
	}

	// Open the file and start writing stream
	string src = spath + "\\anomaly_scores.dat";
	datafile.open(src, ofstream::trunc);

	// Write the ticks and the anomaly scores
	for (int i = 0; i < anomalyScores.size(); ++i)
	{
		int ticks = std::get<0>(anomalyScores.at(i));
		float anomalyScore = std::get<1>(anomalyScores.at(i));
		datafile << ticks << " " << anomalyScore << " ";

		if (anomalyOccurances.find(ticks) != anomalyOccurances.end())
		{
			string anomalyMsg = anomalyOccurances.at(ticks);
			datafile << GetThreshold(datacharts.size()) << " " << anomalyMsg << endl;
		}

		datafile << endl;
	}

	// Close the file when writing is done
	datafile.close();

	AnalyzeData(src, events);

	if (algorithm == Algorithm::SOM)
		// Seriaize the SOM maps
		this->som->Serialize();
}

/// <summary>Deserializes a folder of data containing datacharts.</summary>
/// <param name='folder'>The folder we would like to use for deserialization.</param>
std::vector<DataChart*> AnomalyDetector::DeSerializeCharts(const char* folder)
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

std::map<int, std::string> AnomalyDetector::DeserializeAnomalyOccurences(const char* folder)
{
	std::map<int, std::string> answer;

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
		if (spath.find("anomaly_scores") != std::string::npos)
		{
			const char* path = spath.c_str();

			std::ifstream infile(path);

			std::string line;

			// Read the file
			while (std::getline(infile, line))
			{
				int a;
				float b, c;
				string d;
				std::istringstream iss(line);

				// Try and parse 4 columns
				if (iss >> a >> b >> c >> d)
					// Enlist an anomaly occurance with its name
					answer.emplace(a, d);
			}
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
		printf("Variable Increase triggered. Tick: %i | message: %s\n", m_ticks, msg);
		m_anomalyOccurrences.emplace(m_ticks, msg);
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
		printf("Variable Reset triggered. Tick: %i | message: %s\n", m_ticks, msg);
		m_anomalyOccurrences.emplace(m_ticks, msg);
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
		printf("Funtion Failure triggered. Tick: %i | message: %s\n", m_ticks, msg);
		m_anomalyOccurrences.emplace(m_ticks, msg);
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
	m_ticks = 0;

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
