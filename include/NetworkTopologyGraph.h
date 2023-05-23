#ifndef ETTT_DEDICATED_MULTICORE_NETWORKTOPOLOGYGRAPH_H
#define ETTT_DEDICATED_MULTICORE_NETWORKTOPOLOGYGRAPH_H

#include <iostream>
#include <list>
#include <random>
#include <set>
#include <vector>
#include "constants/constants.h"
using namespace std;

class NetworkTopologyGraph {
public:
    NetworkTopologyGraph(int numVerts, bool random);
    void generateRandomTreeTopologyGraph();
    vector<vector<int>> generateJobSequences(int numTasks);
    vector<int> findShortestPath(int sourceVert, int targetVert);
    void getRelaxedJobSequences(vector<vector<int>> &jobSequences);
    int numVerts;
    int totalNumProcessors;
    int numUsedProcessors;
    bool random;
    vector<vector<pair<int, int>>> adjacencyList;
};

#endif