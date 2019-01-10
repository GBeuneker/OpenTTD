#include "AnomalyDetector.h"
#include "lof.h"
#include "knn.h"
#include "loci.h"
#include "som.h"
#include "Vector2.h"

#include <iostream>
#include <fstream>
using namespace std;

AnomalyDetector* AnomalyDetector::instance;

AnomalyDetector::AnomalyDetector()
{

}

AnomalyDetector::~AnomalyDetector()
{
}

/// <summary>Logs the data in our anomaly detection algorithms.</summary>
/// <param name="_data">Used to indicate status.</param>
void AnomalyDetector::LogData()
{
	ofstream datafile;

	for (int i = 0; i < m_variables.size(); ++i)
	{
		char src[60];
		sprintf(src, ".\\..\\_data\\data%i.dat", i);
		datafile.clear();
		datafile.open(src);

		for (int j = 0; j < 50; ++j)
			datafile << j << " " << rand() % 100 << "\n";

		datafile.close();

		//int value = *m_variables[i];
		//printf("%i\n", value);
	}
}

void AnomalyDetector::TrackPointer(size_t* _var)
{
	m_variables.push_back(_var);
}
