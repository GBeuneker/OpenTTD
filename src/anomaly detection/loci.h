#pragma once
#include "Detector.h"

#if USE_LOCI

class LOCI : public Detector
{
public:
	LOCI();
	~LOCI();
protected:
	Classification Classify(DataChart* d, Datapoint* p) override;
	void SetRNeighbours(DataChart *d, Datapoint *p, float r);
	void SetRNeighbourhood(DataChart *d, Datapoint *p, float r, float k);
	std::vector<Datapoint> GetRNeighbours(DataChart *d, Datapoint *p, float r);
	int GetRNeighbourCount(DataChart * d, Datapoint * p, float r);
	float GetRNeighbourhood(DataChart *d, Datapoint *p, float r, float k);
	float GetMDEF(DataChart *d, Datapoint *p, float r, float k);
	float GetStandardDeviationMDEF(DataChart *d, Datapoint *p, float r, float k);
	float GetSigma(DataChart *d, Datapoint *p, float r, float k);
private:
	float k = 0.5f, l = 3;
	Datapoint lociDatapoints[WINDOW_SIZE];
};

#endif

