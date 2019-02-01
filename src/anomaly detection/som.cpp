#include "som.h"

/// <summary>Constructor for Self-Organizing Map.</summary>
/// <param name='k'>The amount of nodes we would like to use</param>
SOM::SOM(uint16_t size)
{
	this->size = size;
}

/// <summary>Add the datacharts and initialize the SOMs.</summary>
/// <param name='_datacharts'>The datacharts we want to add.</param>
void SOM::AddData(std::vector<DataChart*> _datacharts)
{
	Detector::AddData(_datacharts);

	nodesList[_datacharts.size()][size];

	for (int i = 0; i < datacharts.size(); ++i)
	{
		chartIndices.emplace(datacharts.at(i), i);
		IntializeMap(datacharts.at(i), nodesList[i]);
	}
}

/// <summary>Run the SOM and test the datapoints against the trained SOM maps.</summary>
void SOM::Run()
{
	std::vector<Classification> results;

	for (int i = 0; i < datacharts.size(); ++i)
	{
		DataChart* d = datacharts[i];
		Datapoint datapoint = d->GetLast();

		SOM_Datapoint p = SOM_Datapoint(datapoint.position.X, datapoint.position.Y);
		results.push_back(Classify(d, p));
	}
}

SOM::~SOM()
{
}

/// <summary>Classifies whether a datapoint is anomalous.</summary>
/// <param name='d'>The collection of data we want to use for our classification.</param>
/// <param name='p'>The datapoint we would like to classify.</param>
Classification SOM::Classify(DataChart *d, SOM_Datapoint p)
{
	Classification result;

	// Get the right collection nodes from our trained SOMs
	int chartIndex = chartIndices.at(d);
	SOM_Datapoint* nodes = nodesList[chartIndex];

	// Check if the point is inside the SOM
	result.isAnomaly = IsInSOMMap(nodes, size, p);
	result.certainty = 1;

	return result;
}

/// <summary>Polygon check whether a point is inside the SOM polygon</summary>
/// <param name='nodes'>The nodes of trained SOM map forming a polygon.</param>
/// <param name='size'>The size of our collection of nodes.</param>
/// <param name='datapoint'>The datapoint we would like to check.</param>
bool SOM::IsInSOMMap(SOM_Datapoint * nodes, uint16_t size, SOM_Datapoint datapoint)
{
	// There have to be at least 3 nodes to be a polygon
	if (size < 3)
		return false;

	bool isInside = false;

	for (int i = 0, j = size - 1; i < size; j = i++)
	{
		if (((nodes[i].position.Y > datapoint.position.Y) != (nodes[j].position.Y > datapoint.position.Y)) &&
			(datapoint.position.X < (nodes[j].position.X - nodes[i].position.X) * (datapoint.position.Y - nodes[i].position.Y) / (nodes[j].position.Y - nodes[i].position.Y) + nodes[i].position.X))
			isInside = !isInside;
	}

	return isInside;
}

/// <summary>Initializes the SOM.</summary>
/// <param name='d'>The chart used for initialization.</param>
/// <param name='nodes'>The collection of SOM nodes we would like to initialize</param>
void SOM::IntializeMap(DataChart *d, SOM_Datapoint *nodes)
{
	// Initialize the min and max x,y values
	float minValue_x = FLT_MAX, minValue_y = FLT_MAX;
	float maxValue_x = FLT_MIN, maxValue_y = FLT_MIN;

	// Get the minimum and maximum values from the datachart
	for (int i = 0; i < d->GetValues()->size(); ++i)
	{
		Vector2 pos = d->GetValues()->at(i).position;

		// Get the minumum x,y values
		minValue_x = fmin(minValue_x, pos.X);
		minValue_y = fmin(minValue_y, pos.Y);

		// Get the maximum x,y values
		maxValue_x = fmax(maxValue_x, pos.X);
		maxValue_y = fmax(maxValue_y, pos.Y);
	}

	// Initialize nodes at random positions
	for (int n = 0; n < size; ++n)
	{
		// Get a random x,y position
		float xPos = _random.Next(maxValue_x - minValue_x) - minValue_x;
		float yPos = _random.Next(maxValue_y - minValue_y) - minValue_y;

		nodes[n].position = Vector2(xPos, yPos);
	}
}

/// <summary>Train the Self Organizing Map.</summary>
/// <param name='d'>The chart used for training.</param>
/// <param name='nodes'>The nodes used for training the SOM.</param>
/// <param name='iterations'>The amount of iterations used for training.</param>
void SOM::Train(DataChart *d, SOM_Datapoint *nodes, uint16_t iterations)
{
	for (int i = 0; i < iterations; ++i)
	{
		// Randomly pick a datapoint from the datachart
		Datapoint randomPoint = d->GetRandom();

		// Find the node closest to the datapoint(BMU: Best Matching Unit)
		float minDist = FLT_MAX;
		SOM_Datapoint bmu;
		for (int n = 0; n < size; ++n)
		{
			float dist = Distance(nodes[n].position, randomPoint.position);
			if (dist < minDist)
			{
				minDist = dist;
				bmu = nodes[n];
			}
		}

		// Get the radius around the BMU from the Neighbourhood function
		float radius = GetRadius(i, iterations);
		// Find the nodes within range of the BMU
		for (int n = 0; n < size; ++n)
		{
			float dist = Distance(nodes[n].position, bmu.position);
			if (dist < radius)
			{
				// Update the position of the node in the neighbourhood of the BMU
				UpdatePosition(&nodes[n], randomPoint.position, GetLearningRate(i, iterations), GetDistanceDecay(dist, radius));
			}
		}

		UpdatePosition(&bmu, randomPoint.position, GetLearningRate(i, iterations));
	}

	//TODO: Sort Nodes to form a polygon
}

/// <summary>The radius of our neighbourhood.</summary>
/// <param name='iteration'>The current iteration we're in.</param>
/// <param name='totalIterations'>The total amount of iterations.</param>
float SOM::GetRadius(uint16_t iteration, uint16_t totalIterations)
{
	return startRadius * exp(-(float)iteration / totalIterations);
}

/// <summary>The current learning rate.</summary>
/// <param name='iteration'>The current iteration we're in.</param>
/// <param name='totalIterations'>The total amount of iterations.</param>
float SOM::GetLearningRate(uint16_t iteration, uint16_t totalIterations)
{
	return learningRate * exp(-(float)iteration / totalIterations);
}

/// <summary>Gets the drop-off learning rate based on the distance of a point within the radius.</summary>
/// <param name='iteration'>The current iteration we're in.</param>
/// <param name='totalIterations'>The total amount of iterations.</param>
float SOM::GetDistanceDecay(float distance, float radius)
{
	return exp(-(pow(distance, 2) / pow(radius, 2)));
}

/// <summary>Updates a datapoint's position.</summary>
/// <param name='p'>The datapoint we would like to update.</param>
/// <param name='targetPosition'>The position we should move the datapoint towards.</param>
/// <param name='learningRate'>The amount we will change the datapoint.</param>
/// <param name='distanceDecay'>The decay based on the distance from the BMU.</param>
void SOM::UpdatePosition(SOM_Datapoint * p, Vector2 targetPosition, float learningRate, float distanceDecay)
{
	p->position.operator+=(distanceDecay * learningRate * (targetPosition.operator-(p->position)));
}
