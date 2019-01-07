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
	void LogData(float _data);
	void TrackVariable(int &_var);
	void TrackVariable(float &_var);
	void TrackVariable(bool &_var);
	void TrackPointer(size_t * _var);
private:
	static AnomalyDetector* instance;
	std::vector<size_t*> m_variables;
};

