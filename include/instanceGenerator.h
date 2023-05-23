#ifndef ETTT_DEDICATED_MULTICORE_INSTANCEGENERATOR_H
#define ETTT_DEDICATED_MULTICORE_INSTANCEGENERATOR_H

#include <algorithm>
#include <random>
#include "instanceParser.h"
#include "NetworkTopologyGraph.h"

void generateEtInstances(const string& outputFile, int numVerts, bool random);
void generateEtttInstances(const string& outputFile, int numVerts, bool random);
vector<double> getTaskUtilization(int numProcessors, double maxUtilization, vector<vector<int>> &jobSequences);
EtInstance generateEtInstance(vector<vector<int>> &jobSequences, vector<int> &possiblePeriods,
                              vector<double> &taskUtilization, double variationPercentage, double releaseShiftPercentage,
                              double deadlineShiftPercentage, int minPriority, int maxPriority, random_device &seed, int numProcessors);
EtttInstance generateEtttInstance(vector<vector<int>> &jobSequences, vector<int> &possiblePeriods, int numEtTasks,
                                  vector<double> &taskUtilization, double variationPercentage, double releaseShiftPercentage,
                                  double deadlineShiftPercentage, int minPriority, int maxPriority, random_device &seed, int numProcessors);
vector<int> getCMax(vector<vector<int>> &jobSequences, vector<int> &taskPeriods, vector<double> &taskUtilization, random_device &seed);
int getIntFromInterval(int min, int max, random_device &seed);
vector<int> getAllDivisorsOfNumBiggerThan(int num, int limit);
vector<int> getAllDivisors(int num);


#endif