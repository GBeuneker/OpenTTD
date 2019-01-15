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

/// <summary>Logs the data in our anomaly detection algorithms.</summary>
void AnomalyDetector::LogData()
{
	ofstream datafile;

	for (int i = 0; i < m_variables.size(); ++i)
	{
		// Open the file and start writing stream
		char src[60];
		sprintf(src, ".\\..\\_data\\seed_%i\\data_%i.dat", seed, i);
		datafile.open(src, ofstream::app);
		// Clear the data on the first tick
		if (ticks == 0)
			datafile.clear();

		// Interpret the value of the variable
		int value = *m_variables[i];
		datafile << ticks << " " << value << "\n";

		// Close the file when writing is done
		datafile.close();
	}

	ticks++;
}

void AnomalyDetector::TrackPointer(size_t* _var)
{
	m_variables.push_back(_var);
}

void AnomalyDetector::Reset()
{
	m_variables.clear();
	ticks = 0;
}
