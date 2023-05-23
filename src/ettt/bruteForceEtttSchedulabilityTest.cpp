#include "../../include/bruteForceEtttSchedulabilityTest.h"

vector<vector<vector<int>>> runBruteForceEttt(vector<EtTask> &etTasks, vector<TtTask> &ttTasks, int numProcessors) {
    auto start = high_resolution_clock::now();
    int hyperperiod = getHyperperiodEttt(etTasks, ttTasks);
    vector<vector<vector<EtJob>>> etJobs = getEtJobsFromEtTasks(etTasks, hyperperiod);
    vector<vector<vector<TtJob>>> ttJobs = getTtJobsFromTtTasks(ttTasks, hyperperiod);
    vector<pair<int, int>> temp;
    vector<vector<pair<int, int>>> fixedTimeSlots(numProcessors, temp);
    return assignStartTimes(etJobs, ttJobs, 0, 0, 0, fixedTimeSlots, start);
}

vector<vector<vector<int>>> assignStartTimes(vector<vector<vector<EtJob>>> &etJobs, vector<vector<vector<TtJob>>> &ttJobs, int currentTaskId, int currentPeriodId, int currentPrecedenceId, vector<vector<pair<int, int>>> &fixedTimeSlots, auto start) {
    vector<vector<vector<int>>> startTimes;
    auto stop_outer = high_resolution_clock::now();
    auto duration_outer = duration_cast<seconds>(stop_outer - start);
    if (duration_outer.count() >= TIME_LIMIT) {
        return startTimes;
    }
    if (currentTaskId == ttJobs.size()) {
        vector<vector<vector<EtJob>>> allEtJobs;
        vector<vector<vector<EtJob>>> fixedEtJobs = transformTtJobsToEtJobs(ttJobs, (int)etJobs.size());
        allEtJobs.insert(allEtJobs.end(), etJobs.begin(), etJobs.end());
        allEtJobs.insert(allEtJobs.end(), fixedEtJobs.begin(), fixedEtJobs.end());
        if (assignJobs(allEtJobs, 0, 0, (int)fixedTimeSlots.size(), start)) {
            return extractStartTimes(ttJobs);
        }
    } else {
        vector<vector<pair<int, int>>> nextFixedTimeSlots;
        bool overlap;
        for (int startTime = ttJobs[currentTaskId][currentPeriodId][currentPrecedenceId].r; startTime <= (ttJobs[currentTaskId][currentPeriodId][currentPrecedenceId].deadline - ttJobs[currentTaskId][currentPeriodId][currentPrecedenceId].c); startTime++) {
            overlap = false;
            for (auto & fixedInterval : fixedTimeSlots[ttJobs[currentTaskId][currentPeriodId][currentPrecedenceId].processorId]) {
                if (max(fixedInterval.first, startTime) <= min(fixedInterval.second - 1, startTime + ttJobs[currentTaskId][currentPeriodId][currentPrecedenceId].c - 1)) {
                    overlap = true;
                    break;
                }
            }
            if (overlap) {
                continue;
            }
            ttJobs[currentTaskId][currentPeriodId][currentPrecedenceId].startTime = startTime;
            nextFixedTimeSlots = fixedTimeSlots;
            nextFixedTimeSlots[ttJobs[currentTaskId][currentPeriodId][currentPrecedenceId].processorId].push_back(make_pair(ttJobs[currentTaskId][currentPeriodId][currentPrecedenceId].startTime, ttJobs[currentTaskId][currentPeriodId][currentPrecedenceId].startTime + ttJobs[currentTaskId][currentPeriodId][currentPrecedenceId].c));
            if (currentPrecedenceId + 1 == ttJobs[currentTaskId][currentPeriodId].size()) {
                if (currentPeriodId + 1 == ttJobs[currentTaskId].size()) {
                    startTimes = assignStartTimes(etJobs, ttJobs, currentTaskId + 1, 0, 0, nextFixedTimeSlots, start);
                } else {
                    startTimes = assignStartTimes(etJobs, ttJobs, currentTaskId, currentPeriodId + 1, 0, nextFixedTimeSlots, start);
                }
            } else {
                ttJobs[currentTaskId][currentPeriodId][currentPrecedenceId + 1].r = ttJobs[currentTaskId][currentPeriodId][currentPrecedenceId].startTime + ttJobs[currentTaskId][currentPeriodId][currentPrecedenceId].c;
                startTimes = assignStartTimes(etJobs, ttJobs, currentTaskId, currentPeriodId, currentPrecedenceId + 1, nextFixedTimeSlots, start);
            }
            if (!startTimes.empty()) {
                return startTimes;
            }
            auto stop_inner = high_resolution_clock::now();
            auto duration_inner = duration_cast<seconds>(stop_inner - start);
            if (duration_inner.count() >= TIME_LIMIT) {
                return startTimes;
            }
        }
    }
    return startTimes;
}