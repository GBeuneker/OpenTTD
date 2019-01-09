#pragma once
#include <vector>

class AnomalyDetector
{
public:
	static AnomalyDetector* GetInstance()
	{
		if (!AnomalyDetector::instance)
			AnomalyDetector::instance = new AnomalyDetector;
		return AnomalyDetector::instance;
	}
	AnomalyDetector();
	~AnomalyDetector();
	void LogData();
	void TrackPointer(size_t * _var);
private:
	static AnomalyDetector* instance;
	std::vector<size_t*> m_variables;
};

