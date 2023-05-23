#ifndef ETTT_DEDICATED_MULTICORE_UTILS_H
#define ETTT_DEDICATED_MULTICORE_UTILS_H

#include <cmath>
#include <iostream>
#include <limits>
#include "constants/constants.h"
#include "EtTask.h"
#include "EtJob.h"
#include "TtTask.h"
#include "TtJob.h"

int gcd(int a, int b);
int lcm(int a, int b);
int getHyperperiodEt(vector<EtTask> &etTasks);
int getHyperperiodEttt(vector<EtTask> &etTasks, vector<TtTask> &ttTasks);
vector<vector<vector<EtJob>>> getEtJobsFromEtTasks(vector<EtTask> &etTasks, int hyperperiod);
vector<vector<vector<TtJob>>> getTtJobsFromTtTasks(vector<TtTask> &ttTasks, int hyperperiod);
vector<vector<vector<EtJob>>> transformTtJobsToEtJobs(vector<vector<vector<TtJob>>> &ttJobs, int numEtTasks);
vector<vector<vector<int>>> extractStartTimes(vector<vector<vector<TtJob>>> &ttJobs);
vector<vector<vector<int>>> getStartTimeIndexes(vector<vector<vector<TtJob>>> &ttJobs);
vector<vector<vector<int>>> transformStartTimes(vector<vector<vector<int>>> &startTimeIndexes, vector<int> &startTimesVector);
void printStartTimes(vector<vector<vector<int>>> &startTimes);
void getNextExecutionTimes(EtJob &etJob, vector<int> &executionTimes, int sequenceLength);
long long getNumberOfPossibleScenarios(vector<EtTask> &etTasks, int hyperperiod);
vector<int> edfFpPolicy(vector<EtJob> &applicableEtJobs, vector<int> &processorTimeStamps);
bool compare(EtJob &etJob1, EtJob &etJob2);
bool isIntersectionOfProcessorIntervals(vector<pair<int, int>> &targetIntervals, vector<pair<int, int>> &sourceIntervals);
bool isMergeableSetOfReleaseIntervals(vector<EtJob> &targetApplicableJobs, vector<EtJob> &sourceApplicableJobs, vector<pair<int, int>> &targetIntervals, vector<pair<int, int>> &sourceIntervals);
bool validReleaseIntervals(int maxEFT, int targetRMin, int targetRMax, int sourceRMin, int sourceRMax);

#endif