#pragma once
#include "Detector.h"

#define WINDOW_SIZE 500

class LOCI : public Detector
{
public:
	LOCI();
	void Run() override;
	~LOCI();
protected:
	Classification Classify(DataChart* d, LOCI_Datapoint p) override;
	void SetRNeighbours(DataChart *d, LOCI_Datapoint *p, float r);
	void SetRNeighbourhood(DataChart *d, LOCI_Datapoint *p, float r, float k);
	std::vector<LOCI_Datapoint> GetRNeighbours(DataChart *d, LOCI_Datapoint *p, float r);
	int GetRNeighbourCount(DataChart * d, LOCI_Datapoint * p, float r);
	float GetRNeighbourhood(DataChart *d, LOCI_Datapoint *p, float r, float k);
	float GetMDEF(DataChart *d, LOCI_Datapoint *p, float r, float k);
	float GetStandardDeviationMDEF(DataChart *d, LOCI_Datapoint *p, float r, float k);
	float GetSigma(DataChart *d, LOCI_Datapoint *p, float r, float k);
private:
	float k = 0.5f, l = 3;
	LOCI_Datapoint lociDatapoints[WINDOW_SIZE];
};

