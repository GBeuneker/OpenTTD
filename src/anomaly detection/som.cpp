#include "som.h"

#include <iostream>
#include <fstream>
#include <filesystem>
#include <vector>
#include <direct.h>
#include <stdlib.h>
#include <stdio.h>

/// <summary>Constructor for Self-Organizing Map.</summary>
/// <param name='k'>The amount of nodes we would like to use</param>
SOM::SOM(uint16_t nodeAmount)
{
	this->nodeAmount = nodeAmount;
}

/// <summary>Add the datacharts and initialize the SOMs.</summary>
/// <param name='_datacharts'>The datacharts we want to add.</param>
void SOM::SetTrainingData(std::vector<DataChart*> _trainingSet)
{
	trainingSet = _trainingSet;
	int chartAmount = trainingSet.size();

	for (int i = 0; i < chartAmount; ++i)
	{
		nodesList.push_back(std::vector<SOM_Datapoint>(nodeAmount));
		IntializeMap(trainingSet.at(i), &nodesList.at(i));
	}
}

/// <summary>Initializes the SOM.</summary>
/// <param name='d'>The chart used for initialization.</param>
/// <param name='nodes'>The collection of SOM nodes we would like to initialize</param>
void SOM::IntializeMap(DataChart *d, std::vector<SOM_Datapoint> *nodes)
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

	// Add a margin of 10% on all sides
	minValue_x -= minValue_x / 10;
	minValue_y -= minValue_y / 10;
	maxValue_x += maxValue_x / 10;
	maxValue_y += maxValue_y / 10;

	// Set the start radius to 1/8 the distance of the minimum and maximum boundary corners
	this->startRadius = Distance(Vector2(minValue_x, minValue_y), Vector2(maxValue_x, maxValue_y)) / 8;

	// Initialize nodes at random positions
	for (int n = 0; n < nodeAmount; ++n)
	{
		// Get a random x,y position
		float xPos = minValue_x + _random.Next((maxValue_x - minValue_x) * 100) / 100.0f;
		float yPos = minValue_y + _random.Next((maxValue_y - minValue_y) * 100) / 100.0f;

		nodes->at(n).position = Vector2(xPos, yPos);
	}
}

/// <summary>Run the SOM and test the datapoints against the trained SOM maps.</summary>
void SOM::Run()
{
	std::vector<Classification> results;

	for (int i = 0; i < datacharts.size(); ++i)
	{
		Datapoint datapoint = datacharts[i]->GetLast();
		SOM_Datapoint p = SOM_Datapoint(datapoint.position.X, datapoint.position.Y);

		// Classify the datapoint and add it to the results
		results.push_back(Classify(i, p));
	}
}

void SOM::TrainAll(uint16_t iterations)
{
	for (int i = 0; i < trainingSet.size(); ++i)
	{
		DataChart* d = trainingSet[i];
		Train(d, &nodesList[i], iterations);

		// Serialize the values
		DataChart dc;
		// Add all values to a new datachart
		for (int j = 0; j < nodesList[i].size(); ++j)
			dc.GetValues()->push_back(Datapoint(nodesList[i][j].position.X, nodesList[i][j].position.Y));

		std::string spath = ".\\..\\_data\\SOM";
		const char* path = spath.c_str();
		if (mkdir(path) == 0)
			printf("Directory: \'%s\' was successfully created", path);

		std::ofstream datafile;
		// Open the file and start writing stream
		char src[60];
		sprintf(src, "%s\\data_%i.dat", path, i);
		datafile.open(src, std::ofstream::trunc);

		datafile << dc.Serialize();

		// Close the file when writing is done
		datafile.close();
	}
}

/// <summary>Classifies whether a datapoint is anomalous.</summary>
/// <param name='d'>The collection of data we want to use for our classification.</param>
/// <param name='p'>The datapoint we would like to classify.</param>
Classification SOM::Classify(uint16_t index, SOM_Datapoint p)
{
	Classification result;

	// Get the right collection of trained nodes from the list
	std::vector<SOM_Datapoint> nodes = nodesList[index];

	// Check if the point is inside the SOM
	result.isAnomaly = IsInSOMMap(nodes, nodeAmount, p);
	result.certainty = 1;

	return result;
}

/// <summary>Polygon check whether a point is inside the SOM polygon</summary>
/// <param name='nodes'>The nodes of trained SOM map forming a polygon.</param>
/// <param name='size'>The size of our collection of nodes.</param>
/// <param name='datapoint'>The datapoint we would like to check.</param>
bool SOM::IsInSOMMap(std::vector<SOM_Datapoint> nodes, uint16_t size, SOM_Datapoint datapoint)
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

/// <summary>Train the Self Organizing Map.</summary>
/// <param name='d'>The chart used for training.</param>
/// <param name='nodes'>The nodes used for training the SOM.</param>
/// <param name='iterations'>The amount of iterations used for training.</param>
void SOM::Train(DataChart *d, std::vector<SOM_Datapoint> *nodes, uint16_t iterations)
{
	for (int i = 0; i < iterations; ++i)
	{
		// Randomly pick a datapoint from the datachart
		Datapoint randomPoint = d->GetRandom();

		// Find the node closest to the datapoint(BMU: Best Matching Unit)
		float minDist = FLT_MAX;
		SOM_Datapoint bmu;
		for (int n = 0; n < nodeAmount; ++n)
		{
			float dist = Distance(nodes->at(n).position, randomPoint.position);
			if (dist < minDist)
			{
				minDist = dist;
				bmu = nodes->at(n);
			}
		}

		// Get the radius around the BMU from the Neighbourhood function
		float radius = GetRadius(i, iterations);
		// Find the nodes within range of the BMU
		for (int n = 0; n < nodeAmount; ++n)
		{
			float dist = Distance(nodes->at(n).position, bmu.position);
			if (dist < radius)
			{
				// Update the position of the node in the neighbourhood of the BMU
				UpdatePosition(&nodes->at(n), randomPoint.position, GetLearningRate(i, iterations), GetDistanceDecay(dist, radius));
			}
		}

		UpdatePosition(&bmu, randomPoint.position, GetLearningRate(i, iterations));
	}

	//Sort Nodes to form a polygon
	SortCounterClockwise(nodes);
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

void SOM::SortCounterClockwise(std::vector<SOM_Datapoint>* nodes)
{
	// Calculate the barycenter of the datapoints
	Vector2 barycenter = Vector2(0, 0);
	for (int i = 0; i < nodes->size(); ++i)
		barycenter.operator+=(nodes->at(i).position);
	barycenter /= nodes->size();

	// Sort the points relative to the barycenter
	std::sort(nodes->begin(), nodes->end(),
		[barycenter](const SOM_Datapoint& lhs, const SOM_Datapoint& rhs) -> bool
	{
		return atan2(lhs.position.X - barycenter.X, lhs.position.Y - barycenter.Y) < atan2(rhs.position.X - barycenter.X, rhs.position.Y - barycenter.Y);
	});
}

SOM::~SOM()
{
}
