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
SOM::SOM(uint16_t width, uint16_t height, float learningRate)
{
	this->width = width;
	this->height = height;
	this->learningRate = learningRate;
}

/// <summary>Add the datacharts and initialize the SOMs.</summary>
/// <param name='_datacharts'>The datacharts we want to add.</param>
void SOM::SetTrainingData(std::vector<DataChart*> _trainingSet)
{
	trainingSet = _trainingSet;
	int chartAmount = trainingSet.size();

	for (int i = 0; i < chartAmount; ++i)
	{
		nodesList.push_back(std::vector<SOM_Datapoint>(width * height));
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

	float valueWidth = (maxValue_x - minValue_x);
	float valueHeight = (maxValue_y - minValue_y);

	float deltaX = (maxValue_x - minValue_x) / (this->width - 1);
	float deltaY = (maxValue_y - minValue_y) / (this->height - 1);
	// Initialize nodes at uniform intervals
	for (int y = 0; y < this->height; ++y)
		for (int x = 0; x < this->width; ++x)
		{
			int index = y * this->width + x;
			float xPos = minValue_x + x * deltaX;
			float yPos = minValue_y + y * deltaY;

			nodes->at(index).position = Vector2(xPos, yPos);
		}

	// Set the start radius
	this->startRadius = sqrtf(powf(deltaX, 2) + powf(deltaY, 2)) * 2;
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
	for (int i = 0; i < nodesList.size(); ++i)
	{
		DataChart* d = trainingSet[i];
		Train(d, &nodesList[i], iterations);
	}

	Serialize();
}

void SOM::Serialize()
{
	for (int i = 0; i < nodesList.size(); ++i)
	{
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

	// Index cannot be larger than the nodes list
	if (index >= nodesList.size())
	{
		printf("ERROR: index is larger than trainingset");
		return result;
	}

	// Get the right collection of trained nodes from the list
	std::vector<SOM_Datapoint> nodes = nodesList[index];

	// Check if the point is inside the SOM
	result.isAnomaly = IsInSOMMap(nodes, nodes.size(), p);

	// If our point is outside the SOM, normalize using 2 times the max distance
	if (result.isAnomaly)
		result.certainty = fmax(DistToEdge(nodes, p) / 2 * maxDist, 1);
	// If our point is inside the SOM, normalize using half the max distance
	else
		result.certainty = fmax(DistToEdge(nodes, p) / (maxDist / 2), 1);

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

/// <summary>Polygon check whether a point is inside the SOM polygon</summary>
/// <param name='nodes'>The nodes of trained SOM map forming a polygon.</param>
/// <param name='size'>The size of our collection of nodes.</param>
/// <param name='datapoint'>The datapoint we would like to check.</param>
bool SOM::DistToEdge(std::vector<SOM_Datapoint> nodes, SOM_Datapoint p)
{
	// There have to be at least 3 nodes to be a polygon
	if (nodes.size() < 3)
		return false;

	float closestDist = FLT_MAX;
	// Loop over all the edges
	for (int i = 0; i < nodes.size() - 1; ++i)
	{
		Vector2 a = nodes.at(i).position;
		Vector2 b = nodes.at(i + 1).position;
		float l2 = SqrMagnitude(b - a);
		if (l2 == 0)
			return Distance(p.position, a);

		float t = fmax(0, fmin(1, Dot((p.position - a), b - a) / l2));
		Vector2 proj = a + (t * (b - a));
		float dist = Distance(p.position, proj);
		if (dist < closestDist)
			closestDist = dist;
	}

	return closestDist;
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
		SOM_Datapoint* bmu;
		int bmuIndex = -1;
		for (int n = 0; n < nodes->size(); ++n)
		{
			float dist = Distance(nodes->at(n).position, randomPoint.position);
			if (dist < minDist)
			{
				minDist = dist;
				bmu = &nodes->at(n);
				bmuIndex = n;
			}
		}

		// Get the radius around the BMU from the Neighbourhood function
		float radius = GetRadius(i, iterations);
		// Find the nodes within range of the BMU
		for (int n = 0; n < nodes->size(); ++n)
		{
			float dist = Distance(nodes->at(n).position, bmu->position);
			// ONly update nodes within range which are not the bmu
			if (dist < radius && &nodes->at(n) != bmu)
			{

				// Update the position of the node in the neighbourhood of the BMU
				UpdatePosition(&nodes->at(n), randomPoint.position, GetLearningRate(i, iterations), GetDistanceDecay(dist, radius));
			}
		}

		// Also update the bmu
		UpdatePosition(bmu, randomPoint.position, GetLearningRate(i, iterations));
	}

	// Convert the nodes to a convex hull
	ConvexHull(nodes);
	//Sort Nodes to form a polygon
	SortCounterClockwise(nodes);

	// Get the maximum possible distance between points in our SOM polygon
	for (int i = 0; i < nodes->size(); ++i)
	{
		for (int j = 0; j < nodes->size(); ++j)
			maxDist = fmax(Distance(nodes->at(i).position, nodes->at(j).position), maxDist);
	}
}

/// <summary>The radius of our neighbourhood.</summary>
/// <param name='iteration'>The current iteration we're in.</param>
/// <param name='totalIterations'>The total amount of iterations.</param>
float SOM::GetRadius(uint16_t iteration, uint16_t totalIterations)
{
	return startRadius * exp(-3 * (float)iteration / totalIterations);
}

/// <summary>The current learning rate.</summary>
/// <param name='iteration'>The current iteration we're in.</param>
/// <param name='totalIterations'>The total amount of iterations.</param>
float SOM::GetLearningRate(uint16_t iteration, uint16_t totalIterations)
{
	return learningRate * exp(-1 * (float)iteration / totalIterations);
}

/// <summary>Gets the drop-off learning rate based on the distance of a point within the radius.</summary>
/// <param name='iteration'>The current iteration we're in.</param>
/// <param name='totalIterations'>The total amount of iterations.</param>
float SOM::GetDistanceDecay(float distance, float radius)
{
	return exp(-(3 * pow(distance, 2) / pow(radius, 2)));
}

/// <summary>Updates a datapoint's position.</summary>
/// <param name='p'>The datapoint we would like to update.</param>
/// <param name='targetPosition'>The position we should move the datapoint towards.</param>
/// <param name='learningRate'>The amount we will change the datapoint.</param>
/// <param name='distanceDecay'>The decay based on the distance from the BMU.</param>
void SOM::UpdatePosition(SOM_Datapoint * p, Vector2 targetPosition, float learningRate, float distanceDecay)
{
	p->position += (distanceDecay * learningRate * (targetPosition - p->position));
}

void SOM::SortCounterClockwise(std::vector<SOM_Datapoint>* nodes)
{
	// Calculate the barycenter of the datapoints
	Vector2 barycenter = Vector2(0, 0);
	for (int i = 0; i < nodes->size(); ++i)
		barycenter += nodes->at(i).position;
	barycenter /= nodes->size();

	// Sort the points relative to the barycenter
	std::sort(nodes->begin(), nodes->end(),
		[barycenter](const SOM_Datapoint& lhs, const SOM_Datapoint& rhs) -> bool
	{
		return atan2(lhs.position.X - barycenter.X, lhs.position.Y - barycenter.Y) < atan2(rhs.position.X - barycenter.X, rhs.position.Y - barycenter.Y);
	});
}

void SOM::ConvexHull(std::vector<SOM_Datapoint>* nodes)
{
	size_t n = nodes->size(), k = 0;
	// Convex hull cannot be generated from less than 3 points
	if (nodes->size() < 3) return;
	std::vector<SOM_Datapoint> hull(2 * n);

	// Sort the points by x-coordinate
	std::sort(nodes->begin(), nodes->end(),
		[](const SOM_Datapoint& lhs, const SOM_Datapoint& rhs) -> bool
	{
		return lhs.position.X < rhs.position.X || (lhs.position.X == rhs.position.X && lhs.position.Y < rhs.position.Y);
	});

	// Build the lower hull
	for (size_t i = 0; i < n; ++i)
	{
		while (k >= 2 && Cross(hull.at(k - 2).position, hull.at(k - 1).position, nodes->at(i).position) <= 0)
			k--;

		hull[k++] = nodes->at(i);
	}

	// Build the upper hull
	for (size_t i = n - 1, t = k + 1; i > 0; --i)
	{
		while (k >= t && Cross(hull.at(k - 2).position, hull.at(k - 1).position, nodes->at(i - 1).position) <= 0)
			k--;

		hull[k++] = nodes->at(i - 1);
	}

	hull.resize(k - 1);
	nodes->swap(hull);
}

int SOM::Orientation(SOM_Datapoint p, SOM_Datapoint q, SOM_Datapoint r)
{
	int val = (q.position.Y - p.position.Y) * (r.position.X - q.position.X) -
		(q.position.X - p.position.X) * (r.position.Y - q.position.Y);

	if (val == 0) return 0;  // colinear 
	return (val > 0) ? 1 : 2; // clock or counterclock wise 
}

SOM::~SOM()
{
}
