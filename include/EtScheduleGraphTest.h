#ifndef ETTT_DEDICATED_MULTICORE_ETSCHEDULEGRAPHTEST_H
#define ETTT_DEDICATED_MULTICORE_ETSCHEDULEGRAPHTEST_H

#include <algorithm>
#include <set>
#include <unordered_map>
#include "bruteForceEtSchedulabilityTest.h"
#include "EtScheduleGraphNode.h"
#include "VectorHasher.h"

bool runScheduleGraphEt(vector<EtTask> &etTasks, int numProcessors);
bool generateScheduleGraphEt(vector<vector<vector<EtJob>>> &etJobs, int numProcessors, auto start);
vector<EtScheduleGraphNode> expansionPhase(vector<vector<vector<EtJob>>> &etJobs, EtScheduleGraphNode &currentNode);
EtScheduleGraphNode expandVertex(vector<vector<vector<EtJob>>> &etJobs, EtScheduleGraphNode &currentNode, int addedJobIdx, int earliestStartTime, int latestStartTime);
bool isPossiblyEligible(EtScheduleGraphNode &currentNode, int possiblyEligibleJobIdx, int newCEJobCandidateIdx, int t);
void mergePhase(vector<EtScheduleGraphNode> &nextLevelNodes, int lastIndex, unordered_map<vector<int>, vector<int>, VectorHasher> &hashTableOfNodeIdxsBasedOnPath, set<int, greater<>> &nodeIndexesToErase);
void adjustHashTable(vector<int> &indexesOfHashTable, set<int, greater<>> &indexesToErase, int indexToAdd);

#endif