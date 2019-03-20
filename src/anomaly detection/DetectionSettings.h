#pragma once
#define ENABLE_ANOMALIES 1
// Definitions for the anomaly values
#define ANOMALY_PERCENTAGE 10.0f
#define ANOMALY_THRESHOLD 15.0f

#define USE_KNN 0
#define USE_LOF 0
#define USE_LOCI 1
#define USE_SOM 0

#define WINDOW_SIZE 500
#define TRAINING_TIME 1000
#define MAX_TICK_COUNT 27000

#define FILTER_POINTS 1
#define USE_SUBVALUES 1

#define USE_K_PERCENTAGE 1
#if USE_K_PERCENTAGE
#define K_PERCENTAGE 0.75f
#endif
