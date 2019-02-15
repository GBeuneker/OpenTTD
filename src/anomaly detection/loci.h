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
	void SetRNeighbours(DataChart * d, Datapoint * p, std::vector<Datapoint*>* neighbours, float r);
	void UpdateRNeighbours(Datapoint * p, std::vector<Datapoint*>* neighbours, float r);
	void AddNewNeighbour(Datapoint * p, Datapoint * nbr, std::vector<Datapoint*>* neighbours, float r);
	void SetRNeighbourhood(DataChart *d, Datapoint *p, float r, float k);
	void UpdateRNeighbourhood(DataChart * d, Datapoint * p, float r, float k);
	float GetRNeighbourhood(std::vector<Datapoint*>* neighbours);
	float GetMDEF(DataChart *d, Datapoint *p, float r, float k);
	float GetStandardDeviationMDEF(DataChart *d, Datapoint *p, float r, float k);
	float GetSigma(DataChart *d, Datapoint *p, float r, float k);
private:
	float k = 0.5f, l = 3;
	Datapoint lociDatapoints[WINDOW_SIZE];
};

#endif

