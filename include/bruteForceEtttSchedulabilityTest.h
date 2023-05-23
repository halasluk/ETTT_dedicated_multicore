#ifndef ETTT_DEDICATED_MULTICORE_BRUTEFORCEETTTSCHEDULABILITYTEST_H
#define ETTT_DEDICATED_MULTICORE_BRUTEFORCEETTTSCHEDULABILITYTEST_H

#include "bruteForceEtSchedulabilityTest.h"
#include <chrono>

using namespace std::chrono;

vector<vector<vector<int>>> runBruteForceEttt(vector<EtTask> &etTasks, vector<TtTask> &ttTasks, int numProcessors);
vector<vector<vector<int>>> assignStartTimes(vector<vector<vector<EtJob>>> &etJobs, vector<vector<vector<TtJob>>> &ttJobs, int currentTaskId, int currentPeriodId, int currentPrecedenceId, vector<vector<pair<int, int>>> &fixedTimeSlots, auto start);

#endif