#pragma once
#include "Detector.h"

class LOCI : public Detector
{
public:
	LOCI();
	~LOCI();
protected:
	Classification Classify(DataChart* d, Datapoint* p) override;
	float GetRadius(DataChart * d, Datapoint * p, uint16_t nbrAmount);
	void SetRNeighbours(DataChart * d, Datapoint * p, std::vector<Datapoint*>* neighbours, float r);
	void UpdateRNeighbours(Datapoint * p, std::vector<Datapoint*>* neighbours, float r);
	void SetRNeighbourhood(DataChart *d, Datapoint *p, float r, float k);
	void UpdateRNeighbourhood(DataChart * d, Datapoint * p, float r, float k);
	float GetMDEF(DataChart *d, Datapoint *p);
	float GetStandardDeviationMDEF(DataChart *d, Datapoint *p);
	float GetSigma(DataChart *d, Datapoint *p);
private:
	float k = 0.5f, l = 3;
};

