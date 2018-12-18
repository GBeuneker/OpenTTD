#pragma once
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
private:
	static AnomalyDetector* instance;
};

