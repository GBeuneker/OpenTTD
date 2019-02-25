#pragma once
#include "Detector.h"

#if USE_LOCI

class LOCI : public Detector
{
public:
	LOCI();
	LOCI(uint16_t maxNeighbourRange);
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
	uint16_t maxNeighbourRange = 0;
	uint16_t nbrRange_values[10]
	{
		5, // 1
		5, // 2
		5, // 3
		5, // 4
		5, // 5
		5, // 6
		5, // 7
		5, // 8
		5, // 9
		5  // 10
	};
	Datapoint lociDatapoints[WINDOW_SIZE];
};

#endif

