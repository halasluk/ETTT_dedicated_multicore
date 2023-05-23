#include "../../include/EtttScheduleGraphTest.h"

vector<vector<vector<int>>> runScheduleGraphEttt(vector<EtTask> &etTasks, vector<TtTask> &ttTasks, int numProcessors, bool FGG) {
    auto start = high_resolution_clock::now();
    int hyperperiod = getHyperperiodEttt(etTasks, ttTasks);
    vector<vector<vector<EtJob>>> etJobs = getEtJobsFromEtTasks(etTasks, hyperperiod);
    vector<vector<vector<TtJob>>> ttJobs = getTtJobsFromTtTasks(ttTasks, hyperperiod);
    return generateScheduleGraphEttt(etJobs, ttJobs, numProcessors, start, FGG);
}

vector<vector<vector<int>>> generateScheduleGraphEttt(vector<vector<vector<EtJob>>> &etJobs, vector<vector<vector<TtJob>>> &ttJobs, int numProcessors, auto start, bool FGG) {
    vector<vector<vector<int>>> startTimeIndexes = getStartTimeIndexes(ttJobs);
    int numTtJobs = 0;
    if (!startTimeIndexes.empty()) {
        numTtJobs = startTimeIndexes.back().back().back() + 1;
    }
    int levelId = 0;
    int numDoneTtJobs = 0;
    bool deadlineMiss = false;
    vector<EtttScheduleGraphNode> currentLevelNodes;
    vector<pair<int, int>> processorIntervals(numProcessors);
    vector<EtJob> applicableEtJobs;
    applicableEtJobs.reserve(etJobs.size());
    for (auto &etTaskSet : etJobs) {
        applicableEtJobs.push_back(etTaskSet[0][0]);
    }
    vector<TtJob> applicableTtJobs;
    applicableTtJobs.reserve(ttJobs.size());
    for (auto &ttTaskSet : ttJobs) {
        applicableTtJobs.push_back(ttTaskSet[0][0]);
    }
    vector<int> doneEtJobs((int)etJobs.size(), 0);
    vector<int> ttStartTimes(numTtJobs, NIL);
    EtttScheduleGraphNode root(levelId, numDoneTtJobs, deadlineMiss, processorIntervals, applicableEtJobs, applicableTtJobs, doneEtJobs, ttStartTimes);
    currentLevelNodes.push_back(root);
    unordered_map<vector<int>, bool, VectorHasher> temp;
    vector<unordered_map<vector<int>, bool, VectorHasher>> invalidStartTimes(numTtJobs + 1, temp);
    return generateScheduleGraphEtttRecursively(etJobs, ttJobs, currentLevelNodes, startTimeIndexes, invalidStartTimes, start, FGG);
}

vector<vector<vector<int>>> generateScheduleGraphEtttRecursively(vector<vector<vector<EtJob>>> &etJobs, vector<vector<vector<TtJob>>> &ttJobs, vector<EtttScheduleGraphNode> &currentLevelNodes, vector<vector<vector<int>>> &startTimeIndexes, vector<unordered_map<vector<int>, bool, VectorHasher>> &invalidStartTimes, auto start, bool FGG) {
    vector<vector<vector<int>>> startTimes;
    auto stop_outer = high_resolution_clock::now();
    auto duration_outer = duration_cast<seconds>(stop_outer - start);
    if (duration_outer.count() >= TIME_LIMIT) {
        return startTimes;
    }
    vector<EtttScheduleGraphNode> nextLevelNodes;
    vector<int> temp;
    int lastIndexForMergePhase;
    while (!currentLevelNodes.empty()) {
        vector<EtttScheduleGraphNode> currentNodeChildren;
        unordered_map<vector<int>, vector<int>, VectorHasher> hashTableOfNodeIdxsBasedOnPath;
        set<int, greater<>> nodeIndexesToErase;
        lastIndexForMergePhase = 0;
        vector<pair<int, int>> fixedTtJobs = fixationPhase(currentLevelNodes);
        if ((int)fixedTtJobs.size() == 1 && fixedTtJobs[0].first == NIL) {
            return startTimes;
        }
        int numDoneTtJobsTemp = currentLevelNodes[0].numDoneTtJobs + 1;
        for (auto & fixedTtJob : fixedTtJobs) {
            vector<int> startTimesTemp = currentLevelNodes[0].ttStartTimes;
            startTimesTemp[startTimeIndexes[currentLevelNodes[0].applicableTtJobs[fixedTtJob.first].taskId][currentLevelNodes[0].applicableTtJobs[fixedTtJob.first].periodId][currentLevelNodes[0].applicableTtJobs[fixedTtJob.first].precedenceId]] = fixedTtJob.second;
            if (invalidStartTimes[numDoneTtJobsTemp].count(startTimesTemp) == 1) {
                if (FGG) {
                    continue;
                } else {
                    return startTimes;
                }
            }
            for (auto & currentNode : currentLevelNodes) {
                EtttScheduleGraphNode nextNode = expandVertexTt(ttJobs, startTimeIndexes, currentNode, fixedTtJob.first, fixedTtJob.second);
                nextLevelNodes.push_back(nextNode);
            }
            startTimes = generateScheduleGraphEtttRecursively(etJobs, ttJobs, nextLevelNodes, startTimeIndexes, invalidStartTimes, start, FGG);
            if (!startTimes.empty()) {
                return startTimes;
            }
            auto stop_inner = high_resolution_clock::now();
            auto duration_inner = duration_cast<seconds>(stop_inner - start);
            if (duration_inner.count() >= TIME_LIMIT) {
                return startTimes;
            }
            invalidStartTimes[nextLevelNodes[0].numDoneTtJobs].insert(make_pair(nextLevelNodes[0].ttStartTimes, true));
            nextLevelNodes.clear();
        }
        for (auto & currentNode : currentLevelNodes) {
            currentNodeChildren = expansionPhase(etJobs, currentNode);
            for (auto & currentNodeChild : currentNodeChildren) {
                if (currentNodeChild.deadlineMiss) {
                    return startTimes;
                }
            }
            auto stop_inner = high_resolution_clock::now();
            auto duration_inner = duration_cast<seconds>(stop_inner - start);
            if (duration_inner.count() >= TIME_LIMIT) {
                return startTimes;
            }
            nextLevelNodes.insert(nextLevelNodes.end(), currentNodeChildren.begin(), currentNodeChildren.end());
            mergePhase(nextLevelNodes, lastIndexForMergePhase, hashTableOfNodeIdxsBasedOnPath, nodeIndexesToErase);
            lastIndexForMergePhase = (int)nextLevelNodes.size();
            currentNodeChildren.clear();
            currentNodeChildren.shrink_to_fit();
        }
        for (auto & nodeIndex : nodeIndexesToErase) {
            swap(nextLevelNodes[nodeIndex], nextLevelNodes.back());
            nextLevelNodes.pop_back();
        }
        if (nextLevelNodes.empty()) {
            break;
        }
        currentLevelNodes = nextLevelNodes;
        nextLevelNodes.clear();
        nextLevelNodes.shrink_to_fit();
    }
    if (!currentLevelNodes[0].applicableTtJobs.empty()){
        return startTimes;
    }
    return transformStartTimes(startTimeIndexes, currentLevelNodes[0].ttStartTimes);
}

vector<pair<int, int>> fixationPhase(vector<EtttScheduleGraphNode> &currentLevelNodes) {
    vector<pair<int, int>> fixedTtJobs;
    if (currentLevelNodes[0].applicableTtJobs.empty()) {
        return fixedTtJobs;
    }
    int tMin = numeric_limits<int>::max();
    vector<int> ttJobsStartTimes((int)currentLevelNodes[0].applicableTtJobs.size(), 0);
    vector<int> ttJobIndexesToSearch;
    for (int i = 0; i < (int)currentLevelNodes[0].applicableTtJobs.size(); i++) {
        ttJobIndexesToSearch.push_back(i);
    }
    for (auto & currentNode : currentLevelNodes) {
        for (auto & etJob : currentNode.applicableEtJobs) {
            tMin = min(tMin, max(etJob.rMin, currentNode.processorIntervals[etJob.processorId].first));
        }
        if (ttJobIndexesToSearch.empty()) {
            return fixedTtJobs;
        }
        for (int j = 0; j < (int)ttJobIndexesToSearch.size(); j++) {
            ttJobsStartTimes[j] = max(ttJobsStartTimes[j], max(currentNode.applicableTtJobs[ttJobIndexesToSearch[j]].r, currentNode.processorIntervals[currentNode.applicableTtJobs[ttJobIndexesToSearch[j]].processorId].second));
            // deadline miss detection
            if (ttJobsStartTimes[j] + currentNode.applicableTtJobs[ttJobIndexesToSearch[j]].c > currentNode.applicableTtJobs[ttJobIndexesToSearch[j]].deadline) {
                fixedTtJobs.emplace_back(NIL, NIL);
                return fixedTtJobs;
            }
            if (ttJobsStartTimes[j] > tMin) {
                ttJobsStartTimes.erase(ttJobsStartTimes.begin() + j);
                ttJobIndexesToSearch.erase(ttJobIndexesToSearch.begin() + j);
                j--;
            }
        }
    }
    for (int i = 0; i < (int)ttJobsStartTimes.size(); i++) {
        fixedTtJobs.emplace_back(ttJobIndexesToSearch[i], ttJobsStartTimes[i]);
    }
    return fixedTtJobs;
}

vector<EtttScheduleGraphNode> expansionPhase(vector<vector<vector<EtJob>>> &etJobs, EtttScheduleGraphNode &currentNode) {
    vector<EtttScheduleGraphNode> currentNodeChildren;
    if (currentNode.applicableEtJobs.empty()) {
        return currentNodeChildren;
    }
    int numProcessors = (int)currentNode.processorIntervals.size();
    int tMinEt = numeric_limits<int>::max();
    int tMaxEt = numeric_limits<int>::max();
    vector<vector<int>> applicableEtJobIdxsOnProcessors(numProcessors);
    set<int> timesOfNewEtEventSet;
    for (int i = 0; i < currentNode.applicableEtJobs.size(); i++) {
        int newTMinEt = max(currentNode.applicableEtJobs[i].rMin, currentNode.processorIntervals[currentNode.applicableEtJobs[i].processorId].first);
        int timeOfNewEtEvent = newTMinEt;
        timesOfNewEtEventSet.insert(timeOfNewEtEvent);
        int newTMaxEt = max(currentNode.applicableEtJobs[i].rMax, currentNode.processorIntervals[currentNode.applicableEtJobs[i].processorId].second);
        timeOfNewEtEvent = max(currentNode.applicableEtJobs[i].rMax, currentNode.processorIntervals[currentNode.applicableEtJobs[i].processorId].first);
        timesOfNewEtEventSet.insert(timeOfNewEtEvent);
        tMinEt = min(tMinEt, newTMinEt);
        tMaxEt = min(tMaxEt, newTMaxEt);
        applicableEtJobIdxsOnProcessors[currentNode.applicableEtJobs[i].processorId].push_back(i);
    }
    timesOfNewEtEventSet.insert(tMaxEt + 1);
    auto itr = timesOfNewEtEventSet.find(tMaxEt + 1);
    vector<int> timesOfNewEtEvent(timesOfNewEtEventSet.begin(), itr);
    timesOfNewEtEvent.push_back(tMaxEt + 1);
    int numOfEtEvents = (int)timesOfNewEtEvent.size();
    vector<set<int>> processorIdsOfCorrespondingEtEvent(numOfEtEvents);
    for (int i = 0; i < currentNode.applicableEtJobs.size(); i++) {
        int timeOfNewEtEvent = max(currentNode.applicableEtJobs[i].rMin, currentNode.processorIntervals[currentNode.applicableEtJobs[i].processorId].first);
        if (tMinEt <= timeOfNewEtEvent && timeOfNewEtEvent <= tMaxEt) {
            auto iterator = find(timesOfNewEtEvent.begin(), timesOfNewEtEvent.end(), timeOfNewEtEvent);
            int index = iterator - timesOfNewEtEvent.begin();
            processorIdsOfCorrespondingEtEvent[index].insert(currentNode.applicableEtJobs[i].processorId);
        }
        timeOfNewEtEvent = max(currentNode.applicableEtJobs[i].rMax, currentNode.processorIntervals[currentNode.applicableEtJobs[i].processorId].first);
        if (tMinEt <= timeOfNewEtEvent && timeOfNewEtEvent <= tMaxEt) {
            auto iterator = find(timesOfNewEtEvent.begin(), timesOfNewEtEvent.end(), timeOfNewEtEvent);
            int index = iterator - timesOfNewEtEvent.begin();
            processorIdsOfCorrespondingEtEvent[index].insert(currentNode.applicableEtJobs[i].processorId);
        }
    }
    for (int processorId = 0; processorId < numProcessors; processorId++) {
        processorIdsOfCorrespondingEtEvent[numOfEtEvents - 1].insert(processorId);
    }
    vector<int> certainlyEligibleJobIdxs(numProcessors, NIL);
    vector<int> newCEJobCandidateIdxs(numProcessors);
    vector<vector<int>> possiblyEligibleJobsIdxs(numProcessors);
    for (int eventIndex = 0; eventIndex < numOfEtEvents; eventIndex++) {
        for (auto & processorId : processorIdsOfCorrespondingEtEvent[eventIndex]) {
            if (timesOfNewEtEvent[eventIndex] == (tMaxEt + 1)) {
                newCEJobCandidateIdxs[processorId] = numeric_limits<int>::max();
            } else {
                newCEJobCandidateIdxs[processorId] = NIL;
                for (auto & i : applicableEtJobIdxsOnProcessors[processorId]) {
                    if (newCEJobCandidateIdxs[processorId] == NIL) {
                        if (currentNode.applicableEtJobs[i].rMax <= timesOfNewEtEvent[eventIndex] && timesOfNewEtEvent[eventIndex] >= currentNode.processorIntervals[currentNode.applicableEtJobs[i].processorId].first) {
                            newCEJobCandidateIdxs[processorId] = i;
                        }
                    } else {
                        if (currentNode.applicableEtJobs[i].rMax <= timesOfNewEtEvent[eventIndex] && timesOfNewEtEvent[eventIndex] >= currentNode.processorIntervals[currentNode.applicableEtJobs[i].processorId].first && compare(currentNode.applicableEtJobs[i], currentNode.applicableEtJobs[newCEJobCandidateIdxs[processorId]])) {
                            newCEJobCandidateIdxs[processorId] = i;
                        }
                    }
                }
            }
            if (newCEJobCandidateIdxs[processorId] != NIL && newCEJobCandidateIdxs[processorId] != certainlyEligibleJobIdxs[processorId]) {
                if (certainlyEligibleJobIdxs[processorId] != NIL) {
                    EtttScheduleGraphNode newChild = expandVertexEt(etJobs, currentNode, certainlyEligibleJobIdxs[processorId], max(currentNode.processorIntervals[processorId].first, currentNode.applicableEtJobs[certainlyEligibleJobIdxs[processorId]].rMin), (timesOfNewEtEvent[eventIndex] - 1));
                    currentNodeChildren.push_back(newChild);
                }
                auto idxIterator = find(possiblyEligibleJobsIdxs[processorId].begin(), possiblyEligibleJobsIdxs[processorId].end(), newCEJobCandidateIdxs[processorId]);
                if (idxIterator != possiblyEligibleJobsIdxs[processorId].end()) {
                    possiblyEligibleJobsIdxs[processorId].erase(idxIterator);
                }
                certainlyEligibleJobIdxs[processorId] = newCEJobCandidateIdxs[processorId];
                for (int i = ((int)possiblyEligibleJobsIdxs[processorId].size() - 1); i >= 0; i--) {
                    if (!isPossiblyEligible(currentNode, possiblyEligibleJobsIdxs[processorId][i], newCEJobCandidateIdxs[processorId], timesOfNewEtEvent[eventIndex])) {
                        EtttScheduleGraphNode newChild = expandVertexEt(etJobs, currentNode, possiblyEligibleJobsIdxs[processorId][i], max(currentNode.processorIntervals[processorId].first, currentNode.applicableEtJobs[possiblyEligibleJobsIdxs[processorId][i]].rMin), (timesOfNewEtEvent[eventIndex] - 1));
                        currentNodeChildren.push_back(newChild);
                        possiblyEligibleJobsIdxs[processorId].erase(possiblyEligibleJobsIdxs[processorId].begin() + i);
                    }
                }
            }
            if (newCEJobCandidateIdxs[processorId] != numeric_limits<int>::max()) {
                for (auto & i : applicableEtJobIdxsOnProcessors[processorId]) {
                    if (isPossiblyEligible(currentNode, i, newCEJobCandidateIdxs[processorId], timesOfNewEtEvent[eventIndex]) && find(possiblyEligibleJobsIdxs[processorId].begin(), possiblyEligibleJobsIdxs[processorId].end(), i) == possiblyEligibleJobsIdxs[processorId].end()) {
                        possiblyEligibleJobsIdxs[processorId].push_back(i);
                    }
                }
            }
        }
    }
    return currentNodeChildren;
}

EtttScheduleGraphNode expandVertexTt(vector<vector<vector<TtJob>>> &ttJobs, vector<vector<vector<int>>> &startTimeIndexes, EtttScheduleGraphNode &currentNode, int addedJobIdx, int startTime) {
    int nextLevelId = currentNode.levelId + 1;
    int nextNumDoneTtJobs = currentNode.numDoneTtJobs + 1;
    bool deadlineMiss = false;
    TtJob addedJob = currentNode.applicableTtJobs[addedJobIdx];
    vector<pair<int, int>> nextProcessorIntervals = currentNode.processorIntervals;
    nextProcessorIntervals[addedJob.processorId].first = startTime + addedJob.c;
    nextProcessorIntervals[addedJob.processorId].second = startTime + addedJob.c;
    vector<EtJob> nextApplicableEtJobs = currentNode.applicableEtJobs;
    vector<TtJob> nextApplicableTtJobs = currentNode.applicableTtJobs;
    vector<int> newDoneEtJobs = currentNode.doneEtJobs;
    vector<int> newTtStartTimes = currentNode.ttStartTimes;
    newTtStartTimes[startTimeIndexes[addedJob.taskId][addedJob.periodId][addedJob.precedenceId]] = startTime;
    // deadline miss detection - should never occur here, taken care of in the fixation phase
    if (addedJob.deadline < nextProcessorIntervals[addedJob.processorId].second) {
        deadlineMiss = true;
    }
    if ((addedJob.precedenceId + 1) != ttJobs[addedJob.taskId][addedJob.periodId].size()) {
        TtJob newApplicableTtJob = ttJobs[addedJob.taskId][addedJob.periodId][addedJob.precedenceId + 1];
        newApplicableTtJob.r = nextProcessorIntervals[addedJob.processorId].second;
        nextApplicableTtJobs[addedJobIdx] = newApplicableTtJob;
    } else if ((addedJob.periodId + 1) != ttJobs[addedJob.taskId].size()) {
        TtJob newApplicableTtJob = ttJobs[addedJob.taskId][addedJob.periodId + 1][0];
        nextApplicableTtJobs[addedJobIdx] = newApplicableTtJob;
    } else {
        nextApplicableTtJobs.erase(nextApplicableTtJobs.begin() + addedJobIdx);
    }
    EtttScheduleGraphNode nextNode(nextLevelId, nextNumDoneTtJobs, deadlineMiss, nextProcessorIntervals, nextApplicableEtJobs, nextApplicableTtJobs, newDoneEtJobs, newTtStartTimes);
    return nextNode;
}

EtttScheduleGraphNode expandVertexEt(vector<vector<vector<EtJob>>> &etJobs, EtttScheduleGraphNode &currentNode, int addedJobIdx, int earliestStartTime, int latestStartTime) {
    int nextLevelId = currentNode.levelId + 1;
    int nextNumDoneTtJobs = currentNode.numDoneTtJobs;
    bool deadlineMiss = false;
    EtJob addedJob = currentNode.applicableEtJobs[addedJobIdx];
    vector<pair<int, int>> nextProcessorIntervals = currentNode.processorIntervals;
    nextProcessorIntervals[addedJob.processorId].first = earliestStartTime + addedJob.cMin;
    nextProcessorIntervals[addedJob.processorId].second = latestStartTime + addedJob.cMax;
    vector<EtJob> nextApplicableEtJobs = currentNode.applicableEtJobs;
    vector<TtJob> nextApplicableTtJobs = currentNode.applicableTtJobs;
    vector<int> newDoneEtJobs = currentNode.doneEtJobs;
    newDoneEtJobs[addedJob.taskId]++;
    vector<int> newTtStartTimes = currentNode.ttStartTimes;
    // deadline miss detection
    if (addedJob.deadline < nextProcessorIntervals[addedJob.processorId].second) {
        deadlineMiss = true;
    }
    if ((addedJob.precedenceId + 1) != etJobs[addedJob.taskId][addedJob.periodId].size()) {
        EtJob newApplicableEtJob = etJobs[addedJob.taskId][addedJob.periodId][addedJob.precedenceId + 1];
        newApplicableEtJob.rMin = nextProcessorIntervals[addedJob.processorId].first;
        newApplicableEtJob.rMax = nextProcessorIntervals[addedJob.processorId].second;
        newApplicableEtJob.cMin = addedJob.cMin;
        newApplicableEtJob.cMax = addedJob.cMax;
        nextApplicableEtJobs[addedJobIdx] = newApplicableEtJob;
    } else if ((addedJob.periodId + 1) != etJobs[addedJob.taskId].size()) {
        EtJob newApplicableEtJob = etJobs[addedJob.taskId][addedJob.periodId + 1][0];
        nextApplicableEtJobs[addedJobIdx] = newApplicableEtJob;
    } else {
        nextApplicableEtJobs.erase(nextApplicableEtJobs.begin() + addedJobIdx);
    }
    EtttScheduleGraphNode nextNode(nextLevelId, nextNumDoneTtJobs, deadlineMiss, nextProcessorIntervals, nextApplicableEtJobs, nextApplicableTtJobs, newDoneEtJobs, newTtStartTimes);
    return nextNode;
}

bool isPossiblyEligible(EtttScheduleGraphNode &currentNode, int possiblyEligibleJobIdx, int newCEJobCandidateIdx, int t) {
    if (newCEJobCandidateIdx == numeric_limits<int>::max()) {
        return false;
    } else if (currentNode.applicableEtJobs[possiblyEligibleJobIdx].rMin <= t && t < currentNode.applicableEtJobs[possiblyEligibleJobIdx].rMax && t >= currentNode.processorIntervals[currentNode.applicableEtJobs[possiblyEligibleJobIdx].processorId].first) {
        if (newCEJobCandidateIdx == NIL || compare(currentNode.applicableEtJobs[possiblyEligibleJobIdx], currentNode.applicableEtJobs[newCEJobCandidateIdx])) {
            return true;
        }
    }
    return false;
}

void mergePhase(vector<EtttScheduleGraphNode> &nextLevelNodes, int lastIndex, unordered_map<vector<int>, vector<int>, VectorHasher> &hashTableOfNodeIdxsBasedOnPath, set<int, greater<>> &nodeIndexesToErase) {
    vector<int> temp;
    int numOfIdxs;
    int terminatingIdx;
    int currentIdx;
    int j;
    vector<int> currentIdxs;
    for (int i = lastIndex; i < (int)nextLevelNodes.size(); i++) {
        hashTableOfNodeIdxsBasedOnPath.insert(make_pair(nextLevelNodes[i].doneEtJobs, temp));
        set<int, greater<>> indexesToErase;
        currentIdxs = hashTableOfNodeIdxsBasedOnPath[nextLevelNodes[i].doneEtJobs];
        numOfIdxs = (int)currentIdxs.size();
        terminatingIdx = numOfIdxs;
        currentIdx = 0;
        while (currentIdx != terminatingIdx) {
            currentIdx = currentIdx % numOfIdxs;
            j = currentIdxs[currentIdx];
            if (j == NIL) {
                if (currentIdx == terminatingIdx) {
                    break;
                }
                currentIdx++;
                continue;
            }
            if (isIntersectionOfProcessorIntervals(nextLevelNodes[i].processorIntervals, nextLevelNodes[j].processorIntervals) && isMergeableSetOfReleaseIntervals(nextLevelNodes[i].applicableEtJobs, nextLevelNodes[j].applicableEtJobs, nextLevelNodes[i].processorIntervals, nextLevelNodes[j].processorIntervals)) {
                for (int k = 0; k < nextLevelNodes[i].applicableEtJobs.size(); k++) {
                    if (nextLevelNodes[i].applicableEtJobs[k].precedenceId > 0) {
                        if (nextLevelNodes[i].applicableEtJobs[k].rMin != nextLevelNodes[j].applicableEtJobs[k].rMin || nextLevelNodes[i].applicableEtJobs[k].rMax != nextLevelNodes[j].applicableEtJobs[k].rMax) {
                            nextLevelNodes[i].applicableEtJobs[k].rMin = min(max(nextLevelNodes[i].applicableEtJobs[k].rMin, nextLevelNodes[i].processorIntervals[nextLevelNodes[i].applicableEtJobs[k].processorId].first), max(nextLevelNodes[j].applicableEtJobs[k].rMin, nextLevelNodes[j].processorIntervals[nextLevelNodes[j].applicableEtJobs[k].processorId].first));
                            nextLevelNodes[i].applicableEtJobs[k].rMax = min(max(nextLevelNodes[i].applicableEtJobs[k].rMax, nextLevelNodes[i].processorIntervals[nextLevelNodes[i].applicableEtJobs[k].processorId].first), max(nextLevelNodes[j].applicableEtJobs[k].rMax, nextLevelNodes[j].processorIntervals[nextLevelNodes[j].applicableEtJobs[k].processorId].first));
                        }
                    }
                }
                for (int k = 0; k < nextLevelNodes[i].processorIntervals.size(); k++) {
                    nextLevelNodes[i].processorIntervals[k].first = min(nextLevelNodes[i].processorIntervals[k].first, nextLevelNodes[j].processorIntervals[k].first);
                    nextLevelNodes[i].processorIntervals[k].second = max(nextLevelNodes[i].processorIntervals[k].second, nextLevelNodes[j].processorIntervals[k].second);
                }
                nodeIndexesToErase.insert(j);
                indexesToErase.insert(currentIdx);
                currentIdxs[currentIdx] = NIL;
                terminatingIdx = currentIdx;
            }
            currentIdx++;
        }
        adjustHashTableEttt(hashTableOfNodeIdxsBasedOnPath[nextLevelNodes[i].doneEtJobs], indexesToErase, i);
    }
}

void adjustHashTableEttt(vector<int> &indexesOfHashTable, set<int, greater<>> &indexesToErase, int indexToAdd) {
    for (auto & index : indexesToErase) {
        indexesOfHashTable.erase(indexesOfHashTable.begin() + index);
    }
    indexesOfHashTable.push_back(indexToAdd);
}