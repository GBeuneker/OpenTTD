#include "som.h"
#include "AnomalyDetector.h"

#include <iostream>
#include <fstream>
#include <filesystem>
#include <vector>
#include <direct.h>
#include <stdlib.h>
#include <stdio.h>

#if USE_SOM

#pragma region Initialization

/// <summary>Constructor for Self-Organizing Map.</summary>
/// <param name='k'>The amount of nodes we would like to use</param>
SOM::SOM(uint16_t width, uint16_t height, float learningRate)
{
	this->width = width;
	this->height = height;
	this->learningRate = learningRate;
}

void SOM::SetData(std::vector<DataChart*> _datacharts)
{
	Detector::SetData(_datacharts);

	somNodes.clear();
	initializedCharts.clear();
	maxChartIterations.clear();
	chartIterations.clear();
	startRadii.clear();

	// Initialize values
	for (int i = 0; i < datacharts.size(); ++i)
	{
		somNodes.push_back(new std::vector<Datapoint*>(width * height));
		initializedCharts.push_back(false);
		maxChartIterations.push_back(WINDOW_SIZE);
		chartIterations.push_back(0);
		startRadii.push_back(0);
	}

	// Assign all the somDistances
	somDistances = std::vector<std::vector<float>>(datacharts.size());
	somIndices = std::vector<int>(datacharts.size());
}

/// <summary>Initializes the SOM.</summary>
/// <param name='d'>The chart used for initialization.</param>
/// <param name='nodes'>The collection of SOM nodes we would like to initialize</param>
void SOM::IntializeMap(DataChart *d, std::vector<Datapoint*> *nodes)
{
	// Initialize the min and max x,y values
	float minValue_x = FLT_MAX, minValue_y = FLT_MAX;
	float maxValue_x = FLT_MIN, maxValue_y = FLT_MIN;

	// Get the minimum and maximum values from the datachart
	for (int i = 0; i < d->GetValues()->size(); ++i)
	{
		Vector2 pos = d->GetValues()->at(i)->position;

		// Get the minumum x,y values
		minValue_x = fmin(minValue_x, pos.X);
		minValue_y = fmin(minValue_y, pos.Y);

		// Get the maximum x,y values
		maxValue_x = fmax(maxValue_x, pos.X);
		maxValue_y = fmax(maxValue_y, pos.Y);
	}

	float valueWidth = (maxValue_x - minValue_x);
	float valueHeight = (maxValue_y - minValue_y);
	;
	// Initialize nodes at random positions based on the dataset
	for (int y = 0; y < this->height; ++y)
		for (int x = 0; x < this->width; ++x)
		{
			Vector2 position = d->GetRandom()->position;

			int index = y * this->width + x;
			float xPos = position.X;
			float yPos = position.Y;

			// Add random offset to x and y position
			if (valueWidth > 0.001f)
				xPos += (-0.5f*valueWidth + ((rand() % (int)(valueWidth * 1000)) / 1000.0f));
			if (valueHeight > 0.001f)
				yPos += (-0.5f*valueHeight + ((rand() % (int)(valueHeight * 1000)) / 1000.0f));

			nodes->at(index) = new Datapoint(xPos, yPos);
		}

	// Find the index of the chart
	int chartIndex = std::distance(datacharts.begin(), std::find(datacharts.begin(), datacharts.end(), d));
	// Set the start radius
	this->startRadii[chartIndex] = sqrtf(powf(valueWidth, 2) + powf(valueHeight, 2)) / 2;
}

#pragma endregion

/// <summary>Run the SOM and test the datapoints against the trained SOM maps.</summary>
std::vector<Classification> SOM::Run()
{
	std::vector<Classification> results;

	for (int i = 0; i < datacharts.size(); ++i)
	{
		DataChart* d = datacharts[i];
		Datapoint* p = d->GetLast();
		// Initialize the SOM if we have seen at least passed the training time and if the chart has enough values
		if (!initializedCharts[i] && AnomalyDetector::GetInstance()->GetTicks() >= TRAINING_TIME && d->GetValues()->size() >= 10)
		{
			// Initialize the map
			IntializeMap(d, somNodes[i]);
			initializedCharts[i] = true;

			// Max iterations is the amount of iterations we have seen in the first window (minimum of 10)
			maxChartIterations[i] = d->GetValues()->size();
		}
		//If the chart is initialized, update it with newer values
		else if (initializedCharts[i])
		{
			// Update the SOM map
			UpdateMap(i, p);
			// Train using the average of all points
			Train(d, p);
#if USE_SUBVALUES
			Classification result;
			std::vector<Datapoint*> subPoints = d->GetSubvalues(p);
			// Get the result with the highest certainty
			for (int i = 0; i < subPoints.size(); ++i)
			{
				Classification r = Classify(d, subPoints.at(i));
				if (r.isAnomaly)
				{
					result.isAnomaly = true;
					result.certainty = fmax(result.certainty, r.certainty);
				}
			}
			results.push_back(result);
#else
			// Classify the datapoint and add it to the results
			results.push_back(Classify(d, p));
#endif
			if (chartIterations[i] < maxChartIterations[i])
				chartIterations[i]++;
		}
	}

	return results;
}

#pragma region Classification

/// <summary>Classifies whether a datapoint is anomalous.</summary>
/// <param name='d'>The collection of data we want to use for our classification.</param>
/// <param name='p'>The datapoint we would like to classify.</param>
void SOM::Train(DataChart *d, Datapoint *p)
{
	Classification result;
	// Find the index of the chart
	int chartIndex = std::distance(datacharts.begin(), std::find(datacharts.begin(), datacharts.end(), d));

	// Index cannot be larger than the nodes list
	if (chartIndex >= somNodes.size())
	{
		printf("ERROR: index is larger than trainingset");
		return;
	}

	// Get the right collection of trained nodes from the list
	std::vector<Datapoint*> nodes = *somNodes[chartIndex];
	// Convert the nodes to a convex hull
	ConvexHull(&nodes);
	//Sort Nodes to form a polygon
	SortCounterClockwise(&nodes);
	float distanceToEdge = DistToEdge(&nodes, p);

	// Add the average to the list
	int index = somIndices[chartIndex];
	if (somDistances.at(chartIndex).size() <= index)
		somDistances.at(chartIndex).push_back(distanceToEdge);
	else
		somDistances.at(chartIndex)[index] = distanceToEdge;

	// Increase the index
	somIndices[chartIndex] = (index + 1) % WINDOW_SIZE;
}

/// <summary>Classifies whether a datapoint is anomalous.</summary>
/// <param name='d'>The collection of data we want to use for our classification.</param>
/// <param name='p'>The datapoint we would like to classify.</param>
Classification SOM::Classify(DataChart *d, Datapoint *p)
{
	Classification result;
	// Find the index of the chart
	int chartIndex = std::distance(datacharts.begin(), std::find(datacharts.begin(), datacharts.end(), d));

	// Index cannot be larger than the nodes list
	if (chartIndex >= somNodes.size())
	{
		printf("ERROR: index is larger than trainingset");
		return result;
	}

	// Get the right collection of trained nodes from the list
	std::vector<Datapoint*> nodes = *somNodes[chartIndex];
	// Convert the nodes to a convex hull
	ConvexHull(&nodes);
	//Sort Nodes to form a polygon
	SortCounterClockwise(&nodes);

	// Check if the point is inside the SOM
	if (somDistances.at(chartIndex).size() > 0)
		result.isAnomaly = !IsInSOMMap(&nodes, p);

	float distanceToEdge = DistToEdge(&nodes, p);

	// Only calculate anomaly score if we have observed more than one point
	if (somDistances.at(chartIndex).size() > 0)
	{
		// Calculate the error radius of this window based on the average distances to the edge
		float errorRadius = 0;
		for (int i = 0; i < somDistances.at(chartIndex).size(); ++i)
			errorRadius += somDistances.at(chartIndex)[i] / somDistances.at(chartIndex).size();

		float deviation = distanceToEdge - errorRadius;
		// The certainty increases exponentially until a distance of 3x error radius
		float deviationDistance = deviation / (3 * errorRadius);
		// Only give a certainty if there is a significant error radius (prevents rounding errors)
		result.certainty = errorRadius > 0.01f ? Sigmoid(deviationDistance) : 0;
	}

	return result;
}

/// <summary>Polygon check whether a point is inside the SOM polygon</summary>
/// <param name='nodes'>The nodes of trained SOM map forming a polygon.</param>
/// <param name='size'>The size of our collection of nodes.</param>
/// <param name='datapoint'>The datapoint we would like to check.</param>
bool SOM::IsInSOMMap(std::vector<Datapoint*> *nodes, Datapoint *p)
{
	// There must be at least 3 vertices in polygon[] 
	if (nodes->size() < 3)  return false;

	// Create a point for line segment from p to infinite 
	Vector2 extreme = Vector2(FLT_MAX, p->position.Y);

	// Count intersections of the above line with sides of polygon 
	int count = 0, i = 0;
	do
	{
		int next = (i + 1) % nodes->size();

		// Check if the line segment from 'p' to 'extreme' intersects 
		// with the line segment from 'polygon[i]' to 'polygon[next]' 
		if (DoIntersect(nodes->at(i)->position, nodes->at(next)->position, p->position, extreme))
		{
			// If the point 'p' is colinear with line segment 'i-next', 
			// then check if it lies on segment. If it lies, return true, 
			// otherwise false 
			if (Orientation(nodes->at(i)->position, p->position, nodes->at(next)->position) == 0)
				return OnSegment(nodes->at(i)->position, p->position, nodes->at(next)->position);

			count++;
		}
		i = next;
	} while (i != 0);

	// Return true if count is odd, false otherwise 
	return count % 2 == 1;
}

#pragma endregion

#pragma region Training

/// <summary>Train the Self Organizing Map.</summary>
/// <param name='d'>The chart used for training.</param>
/// <param name='nodes'>The nodes used for training the SOM.</param>
/// <param name='iterations'>The amount of iterations used for training.</param>
void SOM::TrainMap(uint16_t chartIndex)
{
	std::vector<Datapoint*>* nodes = somNodes.at(chartIndex);
	uint16_t maxIterations = maxChartIterations.at(chartIndex);
	float startRadius = startRadii.at(chartIndex);

	for (int i = 0; i < maxChartIterations.at(chartIndex); ++i)
	{
		// Randomly pick a datapoint from the datachart
		Datapoint* randomPoint = datacharts.at(chartIndex)->GetRandom();

		// Find the node closest to the datapoint(BMU: Best Matching Unit)
		float minDist = FLT_MAX;
		Datapoint* bmu;
		int bmuIndex = -1;
		for (int n = 0; n < nodes->size(); ++n)
		{
			float dist = Distance(nodes->at(n)->position, randomPoint->position);
			if (dist < minDist)
			{
				minDist = dist;
				bmu = nodes->at(n);
				bmuIndex = n;
			}
		}

		// Get the radius around the BMU from the Neighbourhood function
		float radius = GetRadius(startRadius, i, maxIterations);
		// Find the nodes within range of the BMU
		for (int n = 0; n < nodes->size(); ++n)
		{
			float dist = Distance(nodes->at(n)->position, bmu->position);
			// ONly update nodes within range which are not the bmu
			if (dist < radius && nodes->at(n) != bmu)
			{
				// Update the position of the node in the neighbourhood of the BMU
				UpdatePosition(nodes->at(n), randomPoint->position, GetLearningRate(learningRate, i, maxIterations), GetDistanceDecay(dist, radius));
			}
		}

		// Also update the bmu
		UpdatePosition(bmu, randomPoint->position, GetLearningRate(learningRate, i, maxIterations));
	}
}

/// <summary>Update the Self-Organizing Map</summary>
/// <param name='nodes'>The nodes used for training the SOM.</param>
/// <param name='datapoint'>The new point that was added.</param>
/// <param name='i'>The current iteration</param>
void SOM::UpdateMap(uint16_t chartIndex, Datapoint* datapoint)
{
	std::vector<Datapoint*>* nodes = somNodes.at(chartIndex);
	uint16_t iteration = chartIterations.at(chartIndex);
	uint16_t maxIterations = maxChartIterations.at(chartIndex);
	float startRadius = startRadii.at(chartIndex);

	// Find the node closest to the datapoint(BMU: Best Matching Unit)
	float minDist = FLT_MAX;
	Datapoint* bmu;
	int bmuIndex = -1;
	for (int n = 0; n < nodes->size(); ++n)
	{
		float dist = Distance(nodes->at(n)->position, datapoint->position);
		if (dist < minDist)
		{
			minDist = dist;
			bmu = nodes->at(n);
			bmuIndex = n;
		}
	}

	// Get the radius around the BMU from the Neighbourhood function
	float radius = GetRadius(startRadius, iteration, maxIterations);
	// Find the nodes within range of the BMU
	for (int n = 0; n < nodes->size(); ++n)
	{
		float dist = Distance(nodes->at(n)->position, bmu->position);
		// Only update nodes within range which are not the bmu
		if (dist < radius && nodes->at(n) != bmu)
			// Update the position of the node in the neighbourhood of the BMU
			UpdatePosition(nodes->at(n), datapoint->position, GetLearningRate(learningRate, iteration, maxIterations), GetDistanceDecay(dist, radius));
	}

	// Also update the bmu
	UpdatePosition(bmu, datapoint->position, GetLearningRate(learningRate, iteration, maxIterations));
}

/// <summary>The radius of our neighbourhood.</summary>
/// <param name='iteration'>The current iteration we're in.</param>
/// <param name='totalIterations'>The total amount of iterations.</param>
float SOM::GetRadius(float startRadius, uint16_t iteration, uint16_t totalIterations)
{
	return startRadius * exp(-(float)iteration / totalIterations);
}

/// <summary>The current learning rate.</summary>
/// <param name='iteration'>The current iteration we're in.</param>
/// <param name='totalIterations'>The total amount of iterations.</param>
float SOM::GetLearningRate(float startLearningRate, uint16_t iteration, uint16_t totalIterations)
{
	return startLearningRate * exp(-(float)iteration / totalIterations);
}

/// <summary>Gets the drop-off learning rate based on the distance of a point within the radius.</summary>
/// <param name='iteration'>The current iteration we're in.</param>
/// <param name='totalIterations'>The total amount of iterations.</param>
float SOM::GetDistanceDecay(float distance, float radius)
{
	return 1 - (pow(distance, 2) / pow(radius, 2));
}

/// <summary>Updates a datapoint's position.</summary>
/// <param name='p'>The datapoint we would like to update.</param>
/// <param name='targetPosition'>The position we should move the datapoint towards.</param>
/// <param name='learningRate'>The amount we will change the datapoint.</param>
/// <param name='distanceDecay'>The decay based on the distance from the BMU.</param>
void SOM::UpdatePosition(Datapoint * p, Vector2 targetPosition, float learningRate, float distanceDecay)
{
	p->position += (distanceDecay * learningRate * (targetPosition - p->position));
}

#pragma endregion

#pragma region Utilities

void SOM::SortCounterClockwise(std::vector<Datapoint*>* nodes)
{
	// Calculate the barycenter of the datapoints
	Vector2 barycenter = Vector2(0, 0);
	for (int i = 0; i < nodes->size(); ++i)
		barycenter += nodes->at(i)->position;
	barycenter /= nodes->size();

	// Sort the points relative to the barycenter
	std::sort(nodes->begin(), nodes->end(),
		[barycenter](const Datapoint* lhs, const Datapoint* rhs) -> bool
	{
		return atan2(lhs->position.X - barycenter.X, lhs->position.Y - barycenter.Y) < atan2(rhs->position.X - barycenter.X, rhs->position.Y - barycenter.Y);
	});
}

void SOM::ConvexHull(std::vector<Datapoint*>* nodes)
{
	size_t n = nodes->size(), k = 0;
	// Convex hull cannot be generated from less than 3 points
	if (nodes->size() < 3) return;
	std::vector<Datapoint*> hull(2 * n);

	// Sort the points by x-coordinate
	std::sort(nodes->begin(), nodes->end(),
		[](const Datapoint* lhs, const Datapoint* rhs) -> bool
	{
		return lhs->position.X < rhs->position.X || (lhs->position.X == rhs->position.X && lhs->position.Y < rhs->position.Y);
	});

	// Build the lower hull
	for (size_t i = 0; i < n; ++i)
	{
		while (k >= 2 && Cross(hull.at(k - 2)->position, hull.at(k - 1)->position, nodes->at(i)->position) <= 0)
			k--;

		hull[k++] = nodes->at(i);
	}

	// Build the upper hull
	for (size_t i = n - 1, t = k + 1; i > 0; --i)
	{
		while (k >= t && Cross(hull.at(k - 2)->position, hull.at(k - 1)->position, nodes->at(i - 1)->position) <= 0)
			k--;

		hull[k++] = nodes->at(i - 1);
	}

	hull.resize(k - 1);
	nodes->swap(hull);
}

int SOM::Orientation(Vector2 p, Vector2 q, Vector2 r)
{
	float val = (q.Y - p.Y) * (r.X - q.X) -
		(q.X - p.X) * (r.Y - q.Y);

	if (val == 0) return 0;  // colinear 
	return (val > 0) ? 1 : 2; // clock or counterclock wise 
}

// The function that returns true if line segment 'p1q1' 
// and 'p2q2' intersect. 
bool SOM::DoIntersect(Vector2 p1, Vector2 q1, Vector2 p2, Vector2 q2)
{
	// Find the four orientations needed for general and 
	// special cases 
	int o1 = Orientation(p1, q1, p2);
	int o2 = Orientation(p1, q1, q2);
	int o3 = Orientation(p2, q2, p1);
	int o4 = Orientation(p2, q2, q1);

	// General case 
	if (o1 != o2 && o3 != o4)
		return true;

	// Special Cases 
	// p1, q1 and p2 are colinear and p2 lies on segment p1q1 
	if (o1 == 0 && OnSegment(p1, p2, q1)) return true;

	// p1, q1 and p2 are colinear and q2 lies on segment p1q1 
	if (o2 == 0 && OnSegment(p1, q2, q1)) return true;

	// p2, q2 and p1 are colinear and p1 lies on segment p2q2 
	if (o3 == 0 && OnSegment(p2, p1, q2)) return true;

	// p2, q2 and q1 are colinear and q1 lies on segment p2q2 
	if (o4 == 0 && OnSegment(p2, q1, q2)) return true;

	return false; // Doesn't fall in any of the above cases 
}

/// <summary>Polygon check whether a point is inside the SOM polygon</summary>
/// <param name='nodes'>The nodes of trained SOM map forming a polygon.</param>
/// <param name='size'>The size of our collection of nodes.</param>
/// <param name='datapoint'>The datapoint we would like to check.</param>
float SOM::DistToEdge(std::vector<Datapoint*>* nodes, Datapoint *p)
{
	// There have to be at least 3 nodes to be a polygon
	if (nodes->size() < 3)
		return false;

	float closestDist = FLT_MAX;
	// Loop over all the edges
	for (int i = 0; i < nodes->size() - 1; ++i)
	{
		Vector2 a = nodes->at(i)->position;
		Vector2 b = nodes->at(i + 1)->position;
		float l2 = SqrMagnitude(b - a);

		float t = fmax(0, fmin(1, Dot((p->position - a), b - a) / l2));
		Vector2 proj = a + (t * (b - a));
		float dist = Distance(p->position, proj);
		if (dist < closestDist)
			closestDist = dist;
	}

	return closestDist;
}

// Given three colinear points p, q, r, the function checks if 
// point q lies on line segment 'pr' 
bool SOM::OnSegment(Vector2 p, Vector2 q, Vector2 r)
{
	if (q.X <= fmax(p.X, r.X) && q.X >= fmin(p.X, r.X) &&
		q.Y <= fmax(p.Y, r.Y) && q.Y >= fmin(p.Y, r.Y))
		return true;
	return false;
}

void SOM::Serialize()
{
	for (int i = 0; i < datacharts.size(); ++i)
	{
		if (!initializedCharts[i])
			continue;

		std::vector<Datapoint*> nodes = *somNodes[i];
		ConvexHull(&nodes);
		SortCounterClockwise(&nodes);

		// Serialize the values
		DataChart dc;
		// Add all values to a new datachart
		for (int j = 0; j < nodes.size(); ++j)
			dc.GetValues()->push_back(new Datapoint(nodes[j]->position.X, nodes[j]->position.Y));

#if ENABLE_ANOMALIES
		std::string spath = AnomalyDetector::GetInstance()->GetBaseFolder() + "seed_" + std::to_string(_random.seed) + "_anomalous";
#else
		std::string spath = AnomalyDetector::GetInstance()->GetBaseFolder() + "seed_" + std::to_string(_random.seed) + "_baseline";
#endif
		const char* path = spath.c_str();
		if (mkdir(path) == 0)
			printf("Directory: \'%s\' was successfully created", path);

		std::ofstream datafile;
		// Open the file and start writing stream
		char src[60];
		sprintf(src, "%s\\som_%i.dat", path, i);
		datafile.open(src, std::ofstream::trunc);

		datafile << dc.Serialize();

		// Close the file when writing is done
		datafile.close();
	}
}

#pragma endregion

SOM::~SOM()
{
}

#endif
