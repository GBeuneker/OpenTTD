#include "AnomalyDetector.h"

using namespace std;

AnomalyDetector* AnomalyDetector::instance;

AnomalyDetector::AnomalyDetector()
{
	instance = this;

	m_ticks = 0;

	this->knn = new KNN(k_percentage);
	this->lof = new LOF(k_percentage);
	this->loci = new LOCI();
	this->som = new SOM(40, 40, 0.5);

#if RUN_EXPERIMENTS
	RunExperiments("BASE\\1v10\\seed_100_a_1.000000_t_15.000000_w_5000_k_0.500000");
	RunExperiments("BASE\\1v10\\seed_100_a_2.000000_t_15.000000_w_5000_k_0.500000");
	RunExperiments("BASE\\1v10\\seed_100_a_10.000000_t_15.000000_w_5000_k_0.500000");
	RunExperiments("BASE\\1v10\\seed_101_a_1.000000_t_15.000000_w_5000_k_0.500000");
	RunExperiments("BASE\\1v10\\seed_101_a_2.000000_t_15.000000_w_5000_k_0.500000");
	RunExperiments("BASE\\1v10\\seed_101_a_10.000000_t_15.000000_w_5000_k_0.500000");
	RunExperiments("BASE\\1v10\\seed_102_a_1.000000_t_15.000000_w_5000_k_0.500000");
	RunExperiments("BASE\\1v10\\seed_102_a_2.000000_t_15.000000_w_5000_k_0.500000");
	RunExperiments("BASE\\1v10\\seed_102_a_10.000000_t_15.000000_w_5000_k_0.500000");
	RunExperiments("BASE\\1v10\\seed_103_a_1.000000_t_15.000000_w_5000_k_0.500000");
	RunExperiments("BASE\\1v10\\seed_103_a_2.000000_t_15.000000_w_5000_k_0.500000");
	RunExperiments("BASE\\1v10\\seed_103_a_10.000000_t_15.000000_w_5000_k_0.500000");
	RunExperiments("BASE\\1v10\\seed_104_a_1.000000_t_15.000000_w_5000_k_0.500000");
	RunExperiments("BASE\\1v10\\seed_104_a_2.000000_t_15.000000_w_5000_k_0.500000");
	RunExperiments("BASE\\1v10\\seed_104_a_10.000000_t_15.000000_w_5000_k_0.500000");
	RunExperiments("BASE\\1v10\\seed_105_a_1.000000_t_15.000000_w_5000_k_0.500000");
	RunExperiments("BASE\\1v10\\seed_105_a_2.000000_t_15.000000_w_5000_k_0.500000");
	RunExperiments("BASE\\1v10\\seed_105_a_10.000000_t_15.000000_w_5000_k_0.500000");
	RunExperiments("BASE\\1v10\\seed_106_a_1.000000_t_15.000000_w_5000_k_0.500000");
	RunExperiments("BASE\\1v10\\seed_106_a_2.000000_t_15.000000_w_5000_k_0.500000");
	RunExperiments("BASE\\1v10\\seed_106_a_10.000000_t_15.000000_w_5000_k_0.500000");
	RunExperiments("BASE\\1v10\\seed_107_a_1.000000_t_15.000000_w_5000_k_0.500000");
	RunExperiments("BASE\\1v10\\seed_107_a_2.000000_t_15.000000_w_5000_k_0.500000");
	RunExperiments("BASE\\1v10\\seed_107_a_10.000000_t_15.000000_w_5000_k_0.500000");
	RunExperiments("BASE\\1v10\\seed_108_a_1.000000_t_15.000000_w_5000_k_0.500000");
	RunExperiments("BASE\\1v10\\seed_108_a_2.000000_t_15.000000_w_5000_k_0.500000");
	RunExperiments("BASE\\1v10\\seed_108_a_10.000000_t_15.000000_w_5000_k_0.500000");
	RunExperiments("BASE\\1v10\\seed_109_a_1.000000_t_15.000000_w_5000_k_0.500000");
	RunExperiments("BASE\\1v10\\seed_109_a_2.000000_t_15.000000_w_5000_k_0.500000");
	RunExperiments("BASE\\1v10\\seed_109_a_10.000000_t_15.000000_w_5000_k_0.500000");
	exit(0);
#endif

#if COMBINE_DATA
	CombineFolder("Combine", CombineMode::INTERSECTION);
	exit(0);
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

#pragma region Data Analysis

void AnomalyDetector::RunExperiments(const char* folder)
{
	printf("Running experiments with base: %s...\n", folder);

	std::vector<DataChart*> loadedCharts = DeSerializeCharts(folder);
	std::map<int, std::string> anomalyOccurances = DeserializeAnomalyOccurences(folder);
	_random.SetSeed(DeserializeSeed(folder));
	this->anomalyPercentage = DeserializeAnomalyPercentage(folder);
	RunExperiments(loadedCharts, anomalyOccurances);

	for (int i = 0; i < loadedCharts.size(); ++i)
	{
		loadedCharts.at(i)->DeleteAllData();
		delete loadedCharts.at(i);
	}
}

void AnomalyDetector::RunExperiments(std::vector<DataChart*> loadedCharts, std::map<int, std::string> anomalyOccurences)
{
	this->threshold = 15;
	this->windowSize = 5000;
	this->knn->SetParameters(threshold, windowSize);
	this->lof->SetParameters(threshold, windowSize);
	this->loci->SetParameters(threshold, windowSize);
	this->som->SetParameters(threshold, windowSize);
	algorithm = Algorithm::KNN;
	AnalyzeCharts(loadedCharts, anomalyOccurences);
	algorithm = Algorithm::LOF;
	AnalyzeCharts(loadedCharts, anomalyOccurences);
	algorithm = Algorithm::LOCI;
	AnalyzeCharts(loadedCharts, anomalyOccurences);
	algorithm = Algorithm::SOM;
	AnalyzeCharts(loadedCharts, anomalyOccurences);

	//this->threshold = 15;
	//this->windowSize = 5000;
	//this->knn->SetParameters(threshold, windowSize);
	//this->lof->SetParameters(threshold, windowSize);
	//this->loci->SetParameters(threshold, windowSize);
	//this->som->SetParameters(threshold, windowSize);
	//algorithm = Algorithm::KNN;
	//AnalyzeCharts(loadedCharts, anomalyOccurences);
	//algorithm = Algorithm::LOF;
	//AnalyzeCharts(loadedCharts, anomalyOccurences);
	//algorithm = Algorithm::LOCI;
	//AnalyzeCharts(loadedCharts, anomalyOccurences);
	//algorithm = Algorithm::SOM;
	//AnalyzeCharts(loadedCharts, anomalyOccurences);

	//this->threshold = 50;
	//this->windowSize = 5000;
	//this->knn->SetParameters(threshold, windowSize);
	//this->lof->SetParameters(threshold, windowSize);
	//this->loci->SetParameters(threshold, windowSize);
	//this->som->SetParameters(threshold, windowSize);
	//algorithm = Algorithm::KNN;
	//AnalyzeCharts(loadedCharts, anomalyOccurences);
	//algorithm = Algorithm::LOF;
	//AnalyzeCharts(loadedCharts, anomalyOccurences);
	//algorithm = Algorithm::LOCI;
	//AnalyzeCharts(loadedCharts, anomalyOccurences);
	//algorithm = Algorithm::SOM;
	//AnalyzeCharts(loadedCharts, anomalyOccurences);

	//this->threshold = 15;
	//this->windowSize = 20;
	//this->knn->SetParameters(threshold, windowSize);
	//this->lof->SetParameters(threshold, windowSize);
	//this->loci->SetParameters(threshold, windowSize);
	//this->som->SetParameters(threshold, windowSize);
	//algorithm = Algorithm::KNN;
	//AnalyzeCharts(loadedCharts, anomalyOccurences);
	//algorithm = Algorithm::LOF;
	//AnalyzeCharts(loadedCharts, anomalyOccurences);
	//algorithm = Algorithm::LOCI;
	//AnalyzeCharts(loadedCharts, anomalyOccurences);
	//algorithm = Algorithm::SOM;
	//AnalyzeCharts(loadedCharts, anomalyOccurences);

	//this->threshold = 15;
	//this->windowSize = 75;
	//this->knn->SetParameters(threshold, windowSize);
	//this->lof->SetParameters(threshold, windowSize);
	//this->loci->SetParameters(threshold, windowSize);
	//this->som->SetParameters(threshold, windowSize);
	//algorithm = Algorithm::KNN;
	//AnalyzeCharts(loadedCharts, anomalyOccurences);
	//algorithm = Algorithm::LOF;
	//AnalyzeCharts(loadedCharts, anomalyOccurences);
	//algorithm = Algorithm::LOCI;
	//AnalyzeCharts(loadedCharts, anomalyOccurences);
	//algorithm = Algorithm::SOM;
	//AnalyzeCharts(loadedCharts, anomalyOccurences);

	//k_percentage = 0.25f;
	//delete this->knn;
	//this->knn = new KNN(k_percentage);
	//delete this->lof;
	//this->lof = new LOF(k_percentage);
	//algorithm = Algorithm::KNN;
	//AnalyzeCharts(loadedCharts, anomalyOccurences);
	//algorithm = Algorithm::LOF;
	//AnalyzeCharts(loadedCharts, anomalyOccurences);

	//k_percentage = 0.50f;
	//delete this->knn;
	//this->knn = new KNN(k_percentage);
	//delete this->lof;
	//this->lof = new LOF(k_percentage);
	//algorithm = Algorithm::KNN;
	//AnalyzeCharts(loadedCharts, anomalyOccurences);
	//algorithm = Algorithm::LOF;
	//AnalyzeCharts(loadedCharts, anomalyOccurences);

	//k_percentage = 0.75f;
	//delete this->knn;
	//this->knn = new KNN(k_percentage);
	//delete this->lof;
	//this->lof = new LOF(k_percentage);
	//algorithm = Algorithm::KNN;
	//AnalyzeCharts(loadedCharts, anomalyOccurences);
	//algorithm = Algorithm::LOF;
	//AnalyzeCharts(loadedCharts, anomalyOccurences);
}

void AnomalyDetector::AnalyzeCharts(std::vector<DataChart*> charts, std::map<int, std::string> anomalyOccurences)
{
	printf("Analyzing chart: Algorithm: %i | Anom: %f | Thres: %f | Window: %i | K\%: %f\n", algorithm, ANOMALY_PERCENTAGE, threshold, windowSize, k_percentage);

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
	for (int tick = 0; tick <= MAX_TICK_COUNT; ++tick)
	{
		bool eventTriggered = false;
		for (int i = 0; i < charts.size(); ++i)
		{
			Datapoint* aggregatedVaue = charts.at(i)->GetValueAt(tick);
			std::vector<Datapoint*> subValues;
#if USE_SUBPOINTS
			subValues = charts.at(i)->GetSubvalues(aggregatedVaue);
#endif
			tempCharts.at(i)->LogData(tick, aggregatedVaue, subValues);
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

	// Delete all the charts
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
	if (spath.find("anomaly_scores") == std::string::npos)
	{
		printf("ERROR: %s is not anomaly score data\n");
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
	string outputPath = spath.substr(0, spath.find("anomaly_scores")) + "\\anomaly_output.dat";
	datafile.open(outputPath, ofstream::trunc);
	datafile << "tp: " << tp << " fp: " << fp << " fn: " << fn << " tn: " << tn;
	datafile.close();
}

void AnomalyDetector::CombineFolder(const char* path, CombineMode _mode)
{
	string spath = ".\\..\\_data\\" + (string)path;
	const char* folderpath = spath.c_str();
	if (mkdir(folderpath) == 0)
	{
		printf("ERROR: Path %s could not be found!", folderpath);
		return;
	}

	std::vector<std::vector<std::tuple<int, float>>> anomalyScoresList;
	std::map<int, std::string> anomalyOccurances = DeserializeAnomalyOccurences(path);
	float threshold = 0;
	int events = 0;

	for (const auto & entry : std::filesystem::recursive_directory_iterator(folderpath))
	{
		string entryPath = entry.path().string();
		// Subtract the root folder from the path
		string subPath = entryPath.substr(entryPath.find(spath) + spath.size() + 1, entryPath.size());
		// Check if we're in the root folder
		if (subPath.find("\\") == std::string::npos)
			continue;

		if (entryPath.find("anomaly_scores") != std::string::npos)
		{
			anomalyScoresList.push_back(DeserializeAnomalScores(entryPath.c_str()));
			if (anomalyOccurances.size() == 0)
				anomalyOccurances = DeserializeAnomalyOccurences(entryPath.c_str());
			if (threshold == 0)
				threshold = DeserializeAnomalyThreshold(entryPath.c_str());
		}
		if (events == 0 && entryPath.find("anomaly_output") != std::string::npos)
			events = DeserializeEvents(entryPath.c_str());
	}
	std::vector<std::tuple<int, float>> combinedAnomalyScores = CombineData(anomalyScoresList, _mode);

	// Open the file and start writing stream
	ofstream datafile;
	string src = spath + "\\anomaly_scores_combined.dat";
	datafile.open(src, ofstream::trunc);

	// Write the ticks and the anomaly scores
	for (int i = 0; i < combinedAnomalyScores.size(); ++i)
	{
		int ticks = std::get<0>(combinedAnomalyScores.at(i));
		float anomalyScore = std::get<1>(combinedAnomalyScores.at(i));
		datafile << ticks << " " << anomalyScore << " ";
		if (anomalyOccurances.find(ticks) != anomalyOccurances.end())
		{
			string anomalyMsg = anomalyOccurances.at(ticks);
			datafile << GetThreshold(10) << " " << anomalyMsg << endl;
		}

		datafile << endl;

	}

	// Close the file when writing is done
	datafile.close();

	AnalyzeData(src, events);
}

std::vector<std::tuple<int, float>> AnomalyDetector::CombineData(std::vector<std::vector<std::tuple<int, float>>> _anomalyScoresList, CombineMode _mode)
{
	std::vector<std::tuple<int, float>> answer;

	if (_anomalyScoresList.size() == 0)
		return answer;

	// Loop over all the tick values in the anomaly scores
	for (int listIndex = 0; listIndex < _anomalyScoresList.at(0).size(); ++listIndex)
	{
		float combinedScore = 0;
		int tick = -1;
		// Compare the values of all the anomaly scores lists
		for (int i = 0; i < _anomalyScoresList.size(); ++i)
		{
			// Get the tick and the score
			if (tick == -1)
				tick = std::get<0>(_anomalyScoresList.at(i).at(listIndex));
			float score = std::get<1>(_anomalyScoresList.at(i).at(listIndex));

			// Combine the values based on the mode
			if (_mode == CombineMode::INTERSECTION)
				combinedScore = fmin(combinedScore, score);
			else if (_mode == CombineMode::UNION)
				combinedScore = fmax(combinedScore, score);

		}
		answer.push_back(std::make_tuple(tick, combinedScore));
	}

	return answer;
}

#pragma endregion

#pragma region Anomaly Detection

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
		if (currentRun < runs.size())
			IConsoleCmdExec(this->runs[currentRun++]);
		else
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
		printf("Directory: \'%s\' was successfully created\n", spath.c_str());

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
		printf("ERROR: Path %s could not be found!\n", path);
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
	string sfolder = ".\\..\\_data\\" + (string)folder;

	for (const auto & entry : std::filesystem::directory_iterator(sfolder))
	{
		string entryPath = entry.path().string();
		if (entryPath.find("anomaly_scores") != std::string::npos)
		{
			std::ifstream infile(entryPath);
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

std::vector<tuple<int, float>> AnomalyDetector::DeserializeAnomalScores(const char* folder)
{
	std::vector<tuple<int, float>> answer;
	string sfolder = (string)folder;

	for (const auto & entry : std::filesystem::directory_iterator(sfolder))
	{
		string entryPath = entry.path().string();
		if (sfolder.find("anomaly_scores") != std::string::npos)
		{
			std::ifstream infile(folder);
			std::string line;

			// Read the file
			while (std::getline(infile, line))
			{
				int a;
				float b;
				std::istringstream iss(line);

				// Parse the columns
				if (iss >> a >> b)
					answer.push_back(std::make_tuple(a, b));
			}
		}
	}

	return answer;
}

float AnomalyDetector::DeserializeAnomalyThreshold(const char* path)
{
	if (mkdir(path) == 0)
	{
		printf("ERROR: Path %s could not be found!\n", path);
		return 0;
	}

	std::ifstream infile(path);
	std::string line;

	// Read the file
	while (std::getline(infile, line))
	{
		int a;
		float b, c;
		std::istringstream iss(line);

		// Parse the columns
		if (iss >> a >> b >> c)
			return c;
	}

	return 0;
}

int AnomalyDetector::DeserializeEvents(const char* path)
{
	if (((string)path).find("anomaly_output") == std::string::npos)
	{
		printf("ERROR: Path %s is not an anomaly_output!\n", path);
		return 0;
	}

	std::ifstream infile(path);
	std::string line;
	std::getline(infile, line);
	std::istringstream iss(line);
	string a1, b1, c1, d1;
	int a2, b2, c2, d2;

	iss >> a1 >> a2 >> b1 >> b2 >> c1 >> c2 >> d1 >> d2;

	return a2 + b2 + c2 + d2;
}

uint32_t AnomalyDetector::DeserializeSeed(const char* path)
{
	string spath = (string)path;
	size_t pos = spath.find("seed_") + 5;

	string s_answer = "";
	while (pos != std::string::npos)
	{
		char character = spath.at(pos);
		if (character == '_')
			break;
		s_answer += character;
		pos++;
	}

	return std::stoi(s_answer);
}

float AnomalyDetector::DeserializeAnomalyPercentage(const char* path)
{
	string spath = (string)path;
	size_t pos = spath.find("a_") + 2;

	string s_answer = "";
	while (pos != std::string::npos)
	{
		char character = spath.at(pos);
		if (character == '_')
			break;
		s_answer += character;
		pos++;
	}

	return std::stof(s_answer);
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
	m_events = 0;
	m_anomalyScores.clear();
	m_anomalyOccurrences.clear();

	for (std::vector<DataChart*>::iterator it = m_datacharts.begin(); it != m_datacharts.end(); ++it)
	{
		(*it)->DeleteAllData();
		delete(*it);
	}

	m_datacharts.clear();
	m_datacharts.shrink_to_fit();
	chartsBuilt = false;

#if !FILTER_POINTS
	// Track your own ticks
	TrackPointer((size_t*)&ticks, "Ticks");
#endif
}

AnomalyDetector::~AnomalyDetector()
{
}
