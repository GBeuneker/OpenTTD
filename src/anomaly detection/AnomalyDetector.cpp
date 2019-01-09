#include "AnomalyDetector.h"
#include "lof.h"
#include "knn.h"
#include "loci.h"
#include "som.h"
#include "Vector2.h"

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
	for (int i = 0; i < m_variables.size(); ++i)
	{
		int value = *m_variables[i];
		printf("%i\n", value);
	}
}

void AnomalyDetector::TrackPointer(size_t* _var)
{
	m_variables.push_back(_var);
}
