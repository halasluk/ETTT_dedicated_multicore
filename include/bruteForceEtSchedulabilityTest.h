#ifndef ETTT_DEDICATED_MULTICORE_BRUTEFORCEETSCHEDULABILITYTEST_H
#define ETTT_DEDICATED_MULTICORE_BRUTEFORCEETSCHEDULABILITYTEST_H

#include <chrono>
#include "globalVariables.h"
#include "utils.h"

using namespace std::chrono;

bool runBruteForceEt(vector<EtTask> &etTasks, int numProcessors);
bool runWorstCase(vector<vector<vector<EtJob>>> &etJobs, int numProcessors);
bool assignJobs(vector<vector<vector<EtJob>>> &etJobs, int currentTaskId, int currentPeriodId, int numProcessors, auto start);
bool scenarioSimulation(vector<vector<vector<EtJob>>> &etJobs, int numProcessors);

#endif