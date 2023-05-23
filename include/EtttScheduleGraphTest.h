#ifndef ETTT_DEDICATED_MULTICORE_ETTTSCHEDULEGRAPHTEST_H
#define ETTT_DEDICATED_MULTICORE_ETTTSCHEDULEGRAPHTEST_H

#include <algorithm>
#include <set>
#include <unordered_map>
#include <chrono>
#include "EtttScheduleGraphNode.h"
#include "utils.h"
#include "VectorHasher.h"

using namespace std::chrono;

vector<vector<vector<int>>> runScheduleGraphEttt(vector<EtTask> &etTasks, vector<TtTask> &ttTasks, int numProcessors, bool FGG);
vector<vector<vector<int>>> generateScheduleGraphEttt(vector<vector<vector<EtJob>>> &etJobs, vector<vector<vector<TtJob>>> &ttJobs, int numProcessors, auto start, bool FGG);
vector<vector<vector<int>>> generateScheduleGraphEtttRecursively(vector<vector<vector<EtJob>>> &etJobs, vector<vector<vector<TtJob>>> &ttJobs, vector<EtttScheduleGraphNode> &currentLevelNodes, vector<vector<vector<int>>> &startTimeIndexes, vector<unordered_map<vector<int>, bool, VectorHasher>> &invalidStartTimes, auto start, bool FGG);
vector<pair<int, int>> fixationPhase(vector<EtttScheduleGraphNode> &currentLevelNodes);
vector<EtttScheduleGraphNode> expansionPhase(vector<vector<vector<EtJob>>> &etJobs, EtttScheduleGraphNode &currentNode);
EtttScheduleGraphNode expandVertexTt(vector<vector<vector<TtJob>>> &ttJobs, vector<vector<vector<int>>> &startTimeIndexes, EtttScheduleGraphNode &currentNode, int addedJobIdx, int startTime);
EtttScheduleGraphNode expandVertexEt(vector<vector<vector<EtJob>>> &etJobs, EtttScheduleGraphNode &currentNode, int addedJobIdx, int earliestStartTime, int latestStartTime);
bool isPossiblyEligible(EtttScheduleGraphNode &currentNode, int possiblyEligibleJobIdx, int newCEJobCandidateIdx, int t);
void mergePhase(vector<EtttScheduleGraphNode> &nextLevelNodes, int lastIndex, unordered_map<vector<int>, vector<int>, VectorHasher> &hashTableOfNodeIdxsBasedOnPath, set<int, greater<>> &nodeIndexesToErase);
void adjustHashTableEttt(vector<int> &indexesOfHashTable, set<int, greater<>> &indexesToErase, int indexToAdd);

#endif