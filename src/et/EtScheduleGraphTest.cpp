#include "../../include/EtScheduleGraphTest.h"

bool runScheduleGraphEt(vector<EtTask> &etTasks, int numProcessors) {
    auto start = high_resolution_clock::now();
    int hyperperiod = getHyperperiodEt(etTasks);
    vector<vector<vector<EtJob>>> etJobs = getEtJobsFromEtTasks(etTasks, hyperperiod);
    // check worst case test first to speed up the algorithm
    if (!runWorstCase(etJobs, numProcessors)) {
        return false;
    }
    return generateScheduleGraphEt(etJobs, numProcessors, start);
}

bool generateScheduleGraphEt(vector<vector<vector<EtJob>>> &etJobs, int numProcessors, auto start) {
    int levelId = 0;
    int lastIndexForMergePhase;
    bool deadlineMiss = false;
    vector<EtScheduleGraphNode> currentLevelNodes;
    vector<pair<int, int>> processorIntervals(numProcessors);
    vector<EtJob> applicableJobs;
    applicableJobs.reserve(etJobs.size());
    for (auto & etTaskSet : etJobs) {
        applicableJobs.push_back(etTaskSet[0][0]);
    }
    vector<int> doneJobsFromTask((int)etJobs.size(), 0);
    EtScheduleGraphNode root(levelId, deadlineMiss, processorIntervals, applicableJobs, doneJobsFromTask);
    currentLevelNodes.push_back(root);
    vector<EtScheduleGraphNode> nextLevelNodes;
    while (!currentLevelNodes.empty()) {
        vector<EtScheduleGraphNode> currentNodeChildren;
        unordered_map<vector<int>, vector<int>, VectorHasher> hashTableOfNodeIdxsBasedOnPath;
        set<int, greater<>> nodeIndexesToErase;
        lastIndexForMergePhase = 0;
        for (auto & currentNode : currentLevelNodes) {
            currentNodeChildren = expansionPhase(etJobs, currentNode);
            for (auto & currentNodeChild : currentNodeChildren) {
                if (currentNodeChild.deadlineMiss) {
                    return false;
                }
            }
            auto stop = high_resolution_clock::now();
            auto duration = duration_cast<seconds>(stop - start);
            if (duration.count() >= TIME_LIMIT) {
                return false;
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
        currentLevelNodes = nextLevelNodes;
        nextLevelNodes.clear();
        nextLevelNodes.shrink_to_fit();
    }
    return true;
}

vector<EtScheduleGraphNode> expansionPhase(vector<vector<vector<EtJob>>> &etJobs, EtScheduleGraphNode &currentNode) {
    vector<EtScheduleGraphNode> currentNodeChildren;
    if (currentNode.applicableJobs.empty()) {
        return currentNodeChildren;
    }
    int numProcessors = (int)currentNode.processorIntervals.size();
    int tMin = numeric_limits<int>::max();
    int tMax = numeric_limits<int>::max();
    vector<vector<int>> applicableJobIdxsOnProcessors(numProcessors);
    set<int> timesOfNewEventSet;
    for (int i = 0; i < currentNode.applicableJobs.size(); i++) {
        int newTMin = max(currentNode.applicableJobs[i].rMin, currentNode.processorIntervals[currentNode.applicableJobs[i].processorId].first);
        int timeOfNewEvent = newTMin;
        timesOfNewEventSet.insert(timeOfNewEvent);
        int newTMax = max(currentNode.applicableJobs[i].rMax, currentNode.processorIntervals[currentNode.applicableJobs[i].processorId].second);
        timeOfNewEvent = max(currentNode.applicableJobs[i].rMax, currentNode.processorIntervals[currentNode.applicableJobs[i].processorId].first);
        timesOfNewEventSet.insert(timeOfNewEvent);
        tMin = min(tMin, newTMin);
        tMax = min(tMax, newTMax);
        applicableJobIdxsOnProcessors[currentNode.applicableJobs[i].processorId].push_back(i);
    }
    timesOfNewEventSet.insert(tMax + 1);
    auto itr = timesOfNewEventSet.find(tMax + 1);
    vector<int> timesOfNewEvent(timesOfNewEventSet.begin(), itr);
    timesOfNewEvent.push_back(tMax + 1);
    int numOfEvents = (int)timesOfNewEvent.size();
    vector<set<int>> processorIdsOfCorrespondingEvent(numOfEvents);
    for (int i = 0; i < currentNode.applicableJobs.size(); i++) {
        int timeOfNewEvent = max(currentNode.applicableJobs[i].rMin, currentNode.processorIntervals[currentNode.applicableJobs[i].processorId].first);
        if (tMin <= timeOfNewEvent && timeOfNewEvent <= tMax) {
            auto iterator = find(timesOfNewEvent.begin(), timesOfNewEvent.end(), timeOfNewEvent);
            int index = iterator - timesOfNewEvent.begin();
            processorIdsOfCorrespondingEvent[index].insert(currentNode.applicableJobs[i].processorId);
        }
        timeOfNewEvent = max(currentNode.applicableJobs[i].rMax, currentNode.processorIntervals[currentNode.applicableJobs[i].processorId].first);
        if (tMin <= timeOfNewEvent && timeOfNewEvent <= tMax) {
            auto iterator = find(timesOfNewEvent.begin(), timesOfNewEvent.end(), timeOfNewEvent);
            int index = iterator - timesOfNewEvent.begin();
            processorIdsOfCorrespondingEvent[index].insert(currentNode.applicableJobs[i].processorId);
        }
    }
    for (int processorId = 0; processorId < numProcessors; processorId++) {
        processorIdsOfCorrespondingEvent[numOfEvents - 1].insert(processorId);
    }
    vector<int> certainlyEligibleJobIdxs(numProcessors, NIL);
    vector<int> newCEJobCandidateIdxs(numProcessors);
    vector<vector<int>> possiblyEligibleJobsIdxs(numProcessors);
    for (int eventIndex = 0; eventIndex < numOfEvents; eventIndex++) {
        for (auto & processorId : processorIdsOfCorrespondingEvent[eventIndex]) {
            if (timesOfNewEvent[eventIndex] == (tMax + 1)) {
                newCEJobCandidateIdxs[processorId] = numeric_limits<int>::max();
            } else {
                newCEJobCandidateIdxs[processorId] = NIL;
                for (auto & i : applicableJobIdxsOnProcessors[processorId]) {
                    if (newCEJobCandidateIdxs[processorId] == NIL) {
                        if (currentNode.applicableJobs[i].rMax <= timesOfNewEvent[eventIndex] && timesOfNewEvent[eventIndex] >= currentNode.processorIntervals[currentNode.applicableJobs[i].processorId].first) {
                            newCEJobCandidateIdxs[processorId] = i;
                        }
                    } else {
                        if (currentNode.applicableJobs[i].rMax <= timesOfNewEvent[eventIndex] && timesOfNewEvent[eventIndex] >= currentNode.processorIntervals[currentNode.applicableJobs[i].processorId].first && compare(currentNode.applicableJobs[i], currentNode.applicableJobs[newCEJobCandidateIdxs[processorId]])) {
                            newCEJobCandidateIdxs[processorId] = i;
                        }
                    }
                }
            }
            if (newCEJobCandidateIdxs[processorId] != NIL && newCEJobCandidateIdxs[processorId] != certainlyEligibleJobIdxs[processorId]) {
                if (certainlyEligibleJobIdxs[processorId] != NIL) {
                    EtScheduleGraphNode newChild = expandVertex(etJobs, currentNode, certainlyEligibleJobIdxs[processorId], max(currentNode.processorIntervals[processorId].first, currentNode.applicableJobs[certainlyEligibleJobIdxs[processorId]].rMin), (timesOfNewEvent[eventIndex] - 1));
                    currentNodeChildren.push_back(newChild);
                }
                auto idxIterator = find(possiblyEligibleJobsIdxs[processorId].begin(), possiblyEligibleJobsIdxs[processorId].end(), newCEJobCandidateIdxs[processorId]);
                if (idxIterator != possiblyEligibleJobsIdxs[processorId].end()) {
                    possiblyEligibleJobsIdxs[processorId].erase(idxIterator);
                }
                certainlyEligibleJobIdxs[processorId] = newCEJobCandidateIdxs[processorId];
                for (int i = ((int)possiblyEligibleJobsIdxs[processorId].size() - 1); i >= 0; i--) {
                    if (!isPossiblyEligible(currentNode, possiblyEligibleJobsIdxs[processorId][i], newCEJobCandidateIdxs[processorId], timesOfNewEvent[eventIndex])) {
                        EtScheduleGraphNode newChild = expandVertex(etJobs, currentNode, possiblyEligibleJobsIdxs[processorId][i], max(currentNode.processorIntervals[processorId].first, currentNode.applicableJobs[possiblyEligibleJobsIdxs[processorId][i]].rMin), (timesOfNewEvent[eventIndex] - 1));
                        currentNodeChildren.push_back(newChild);
                        possiblyEligibleJobsIdxs[processorId].erase(possiblyEligibleJobsIdxs[processorId].begin() + i);
                    }
                }
            }
            if (newCEJobCandidateIdxs[processorId] != numeric_limits<int>::max()) {
                for (auto & i : applicableJobIdxsOnProcessors[processorId]) {
                    if (isPossiblyEligible(currentNode, i, newCEJobCandidateIdxs[processorId], timesOfNewEvent[eventIndex]) && find(possiblyEligibleJobsIdxs[processorId].begin(), possiblyEligibleJobsIdxs[processorId].end(), i) == possiblyEligibleJobsIdxs[processorId].end()) {
                        possiblyEligibleJobsIdxs[processorId].push_back(i);
                    }
                }
            }
        }
    }
    return currentNodeChildren;
}

EtScheduleGraphNode expandVertex(vector<vector<vector<EtJob>>> &etJobs, EtScheduleGraphNode &currentNode, int addedJobIdx, int earliestStartTime, int latestStartTime) {
    int nextLevelId = currentNode.levelId + 1;
    bool deadlineMiss = false;
    EtJob addedJob = currentNode.applicableJobs[addedJobIdx];
    vector<int> newDoneJobsFromTask = currentNode.doneJobsFromTask;
    newDoneJobsFromTask[addedJob.taskId]++;
    vector<pair<int, int>> nextProcessorIntervals = currentNode.processorIntervals;
    nextProcessorIntervals[addedJob.processorId].first = earliestStartTime + addedJob.cMin;
    nextProcessorIntervals[addedJob.processorId].second = latestStartTime + addedJob.cMax;
    vector<EtJob> nextApplicableJobs = currentNode.applicableJobs;
    // deadline miss detection
    if (addedJob.deadline < nextProcessorIntervals[addedJob.processorId].second) {
        deadlineMiss = true;
    }
    if ((addedJob.precedenceId + 1) != etJobs[addedJob.taskId][addedJob.periodId].size()) {
        EtJob newApplicableJob = etJobs[addedJob.taskId][addedJob.periodId][addedJob.precedenceId + 1];
        newApplicableJob.rMin = nextProcessorIntervals[addedJob.processorId].first;
        newApplicableJob.rMax = nextProcessorIntervals[addedJob.processorId].second;
        newApplicableJob.cMin = addedJob.cMin;
        newApplicableJob.cMax = addedJob.cMax;
        nextApplicableJobs[addedJobIdx] = newApplicableJob;
    } else if ((addedJob.periodId + 1) != etJobs[addedJob.taskId].size()) {
        EtJob newApplicableJob = etJobs[addedJob.taskId][addedJob.periodId + 1][0];
        nextApplicableJobs[addedJobIdx] = newApplicableJob;
    } else {
        nextApplicableJobs.erase(nextApplicableJobs.begin() + addedJobIdx);
    }
    EtScheduleGraphNode nextNode(nextLevelId, deadlineMiss, nextProcessorIntervals, nextApplicableJobs, newDoneJobsFromTask);
    return nextNode;
}

bool isPossiblyEligible(EtScheduleGraphNode &currentNode, int possiblyEligibleJobIdx, int newCEJobCandidateIdx, int t) {
    if (newCEJobCandidateIdx == numeric_limits<int>::max()) {
        return false;
    } else if (currentNode.applicableJobs[possiblyEligibleJobIdx].rMin <= t && t < currentNode.applicableJobs[possiblyEligibleJobIdx].rMax && t >= currentNode.processorIntervals[currentNode.applicableJobs[possiblyEligibleJobIdx].processorId].first) {
        if (newCEJobCandidateIdx == NIL || compare(currentNode.applicableJobs[possiblyEligibleJobIdx], currentNode.applicableJobs[newCEJobCandidateIdx])) {
            return true;
        }
    }
    return false;
}

void mergePhase(vector<EtScheduleGraphNode> &nextLevelNodes, int lastIndex, unordered_map<vector<int>, vector<int>, VectorHasher> &hashTableOfNodeIdxsBasedOnPath, set<int, greater<>> &nodeIndexesToErase) {
    vector<int> temp;
    int numOfIdxs;
    int terminatingIdx;
    int currentIdx;
    int j;
    vector<int> currentIdxs;
    for (int i = lastIndex; i < (int)nextLevelNodes.size(); i++) {
        hashTableOfNodeIdxsBasedOnPath.insert(make_pair(nextLevelNodes[i].doneJobsFromTask, temp));
        set<int, greater<>> indexesToErase;
        currentIdxs = hashTableOfNodeIdxsBasedOnPath[nextLevelNodes[i].doneJobsFromTask];
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
            if (isIntersectionOfProcessorIntervals(nextLevelNodes[i].processorIntervals, nextLevelNodes[j].processorIntervals) && isMergeableSetOfReleaseIntervals(nextLevelNodes[i].applicableJobs, nextLevelNodes[j].applicableJobs, nextLevelNodes[i].processorIntervals, nextLevelNodes[j].processorIntervals)) {
                for (int k = 0; k < nextLevelNodes[i].applicableJobs.size(); k++) {
                    if (nextLevelNodes[i].applicableJobs[k].precedenceId > 0) {
                        if (nextLevelNodes[i].applicableJobs[k].rMin != nextLevelNodes[j].applicableJobs[k].rMin || nextLevelNodes[i].applicableJobs[k].rMax != nextLevelNodes[j].applicableJobs[k].rMax) {
                            nextLevelNodes[i].applicableJobs[k].rMin = min(max(nextLevelNodes[i].applicableJobs[k].rMin, nextLevelNodes[i].processorIntervals[nextLevelNodes[i].applicableJobs[k].processorId].first), max(nextLevelNodes[j].applicableJobs[k].rMin, nextLevelNodes[j].processorIntervals[nextLevelNodes[j].applicableJobs[k].processorId].first));
                            nextLevelNodes[i].applicableJobs[k].rMax = min(max(nextLevelNodes[i].applicableJobs[k].rMax, nextLevelNodes[i].processorIntervals[nextLevelNodes[i].applicableJobs[k].processorId].first), max(nextLevelNodes[j].applicableJobs[k].rMax, nextLevelNodes[j].processorIntervals[nextLevelNodes[j].applicableJobs[k].processorId].first));
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
        adjustHashTable(hashTableOfNodeIdxsBasedOnPath[nextLevelNodes[i].doneJobsFromTask], indexesToErase, i);
    }
}

void adjustHashTable(vector<int> &indexesOfHashTable, set<int, greater<>> &indexesToErase, int indexToAdd) {
    for (auto & index : indexesToErase) {
        indexesOfHashTable.erase(indexesOfHashTable.begin() + index);
    }
    indexesOfHashTable.push_back(indexToAdd);
}