#include "../../include/bruteForceEtSchedulabilityTest.h"

bool runBruteForceEt(vector<EtTask> &etTasks, int numProcessors) {
    auto start = high_resolution_clock::now();
    int hyperperiod = getHyperperiodEt(etTasks);
    vector<vector<vector<EtJob>>> etJobs = getEtJobsFromEtTasks(etTasks, hyperperiod);
    numPossibleScenarios = getNumberOfPossibleScenarios(etTasks, hyperperiod);
    return assignJobs(etJobs, 0, 0, numProcessors, start);
}

bool runWorstCase(vector<vector<vector<EtJob>>> &etJobs, int numProcessors) {
    for (auto & currentTask : etJobs) {
        for (auto & currentPeriod : currentTask) {
            for (int currentPrecedenceId = 0; currentPrecedenceId < currentPeriod.size(); currentPrecedenceId++) {
                if (currentPrecedenceId == 0) {
                    currentPeriod[currentPrecedenceId].r = currentPeriod[currentPrecedenceId].rMax;
                }
                currentPeriod[currentPrecedenceId].c = currentPeriod[0].cMax;
            }
        }
    }
    return scenarioSimulation(etJobs, numProcessors);
}

bool assignJobs(vector<vector<vector<EtJob>>> &etJobs, int currentTaskId, int currentPeriodId, int numProcessors, auto start) {
    auto stop_outer = high_resolution_clock::now();
    auto duration_outer = duration_cast<seconds>(stop_outer - start);
    if (duration_outer.count() >= TIME_LIMIT) {
        return false;
    }
    if (currentTaskId == etJobs.size()) {
        numScenariosCheckedInBruteForceRun++;
        return scenarioSimulation(etJobs, numProcessors);
    }
    int sequenceLength = (int)etJobs[currentTaskId][currentPeriodId].size();
    vector<int> executionTimes(sequenceLength, etJobs[currentTaskId][currentPeriodId][0].cMax);
    vector<int> lastExecutionTimes(sequenceLength, etJobs[currentTaskId][currentPeriodId][0].cMin);
    executionTimes[sequenceLength - 1]++;
    do {
        getNextExecutionTimes(etJobs[currentTaskId][currentPeriodId][0], executionTimes, sequenceLength);
        for (int i = 0; i < etJobs[currentTaskId][currentPeriodId].size(); i++) {
            etJobs[currentTaskId][currentPeriodId][i].c = executionTimes[i];
        }
        for (int r = etJobs[currentTaskId][currentPeriodId][0].rMax; r >= etJobs[currentTaskId][currentPeriodId][0].rMin; r--) {
            etJobs[currentTaskId][currentPeriodId][0].r = r;
            bool result;
            if (etJobs[currentTaskId].size() == currentPeriodId + 1) {
                result = assignJobs(etJobs, currentTaskId + 1, 0, numProcessors, start);
            } else {
                result = assignJobs(etJobs, currentTaskId, currentPeriodId + 1, numProcessors, start);
            }
            if (!result) {
                return false;
            }
            auto stop_inner = high_resolution_clock::now();
            auto duration_inner = duration_cast<seconds>(stop_inner - start);
            if (duration_inner.count() >= TIME_LIMIT) {
                return false;
            }
        }
    } while (executionTimes != lastExecutionTimes);
    return true;
}

bool scenarioSimulation(vector<vector<vector<EtJob>>> &etJobs, int numProcessors) {
    vector<EtJob> applicableEtJobs;
    applicableEtJobs.reserve(etJobs.size());
    for (auto & etTaskSet : etJobs) {
        applicableEtJobs.push_back(etTaskSet[0][0]);
    }
    vector<int> processorTimeStamps(numProcessors, 0);
    while (!applicableEtJobs.empty()) {
        int minT = numeric_limits<int>::max();
        for (auto & applicableEtJob : applicableEtJobs) {
            minT = min(minT, max(applicableEtJob.r, processorTimeStamps[applicableEtJob.processorId]));
        }
        for (auto & processorTimeStamp : processorTimeStamps) {
            processorTimeStamp = max(processorTimeStamp, minT);
        }
        vector<int> pickedJobIds = edfFpPolicy(applicableEtJobs, processorTimeStamps);
        // detection of deadline miss
        if (processorTimeStamps[etJobs[pickedJobIds[0]][pickedJobIds[1]][pickedJobIds[2]].processorId] + etJobs[pickedJobIds[0]][pickedJobIds[1]][pickedJobIds[2]].c > etJobs[pickedJobIds[0]][pickedJobIds[1]][pickedJobIds[2]].deadline) {
            return false;
        }
        processorTimeStamps[etJobs[pickedJobIds[0]][pickedJobIds[1]][pickedJobIds[2]].processorId] += etJobs[pickedJobIds[0]][pickedJobIds[1]][pickedJobIds[2]].c;
        if (pickedJobIds[2] + 1 == etJobs[pickedJobIds[0]][pickedJobIds[1]].size()) {
            if (pickedJobIds[1] + 1 == etJobs[pickedJobIds[0]].size()) {
                continue;
            } else {
                applicableEtJobs.push_back(etJobs[pickedJobIds[0]][pickedJobIds[1] + 1][0]);
            }
        } else {
            EtJob nextJobInSequence = etJobs[pickedJobIds[0]][pickedJobIds[1]][pickedJobIds[2] + 1];
            if (nextJobInSequence.r == NIL) {
                nextJobInSequence.r = processorTimeStamps[etJobs[pickedJobIds[0]][pickedJobIds[1]][pickedJobIds[2]].processorId];
            }
            applicableEtJobs.push_back(nextJobInSequence);
        }
    }
    return true;
}