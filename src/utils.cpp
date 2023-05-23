#include "../include/utils.h"

int gcd(int a, int b) {
    if (b == 0) {
        return a;
    } else {
        return gcd(b, a % b);
    }
}

int lcm(int a, int b) {
    return (int)(((long long)a * (long long)b) / gcd(a, b));
}

int getHyperperiodEt(vector<EtTask> &etTasks) {
    int tmpLcm = 1;
    for (auto & etTask : etTasks) {
        tmpLcm = lcm(tmpLcm, etTask.period);
    }
    return tmpLcm;
}

int getHyperperiodEttt(vector<EtTask> &etTasks, vector<TtTask> &ttTasks) {
    int tmpLcm = 1;
    for (auto & etTask : etTasks) {
        tmpLcm = lcm(tmpLcm, etTask.period);
    }
    for (auto & ttTask : ttTasks) {
        tmpLcm = lcm(tmpLcm, ttTask.period);
    }
    return tmpLcm;
}

vector<vector<vector<EtJob>>> getEtJobsFromEtTasks(vector<EtTask> &etTasks, int hyperperiod) {
    vector<EtJob> etJobsSamePeriod;
    vector<vector<EtJob>> etJobsSameTask;
    vector<vector<vector<EtJob>>> allEtJobs;
    for (auto & etTask : etTasks) {
        for (int periodId = 0; periodId < (hyperperiod / etTask.period); periodId++) {
            for (int precedenceId = 0; precedenceId < etTask.jobSequence.size(); precedenceId++) {
                EtJob etJob(etTask.id, periodId, precedenceId, etTask.jobSequence[precedenceId],
                            etTask.deadline + periodId * etTask.period - ((int)etTask.jobSequence.size() - (precedenceId + 1)) * etTask.cMax, NIL, NIL, NIL, NIL, NIL, NIL, etTask.priority);
                if (precedenceId == 0) {
                    etJob.rMin = etTask.rMin + periodId * etTask.period;
                    etJob.rMax = etTask.rMax + periodId * etTask.period;
                    etJob.cMin = etTask.cMin;
                    etJob.cMax = etTask.cMax;
                }
                etJobsSamePeriod.push_back(etJob);
            }
            etJobsSameTask.push_back(etJobsSamePeriod);
            etJobsSamePeriod.clear();
        }
        allEtJobs.push_back(etJobsSameTask);
        etJobsSameTask.clear();
    }
    return allEtJobs;
}

vector<vector<vector<TtJob>>> getTtJobsFromTtTasks(vector<TtTask> &ttTasks, int hyperperiod){
    vector<TtJob> ttJobsSamePeriod;
    vector<vector<TtJob>> ttJobsSameTask;
    vector<vector<vector<TtJob>>> allTtJobs;
    for (auto & ttTask : ttTasks) {
        for (int periodId = 0; periodId < (hyperperiod / ttTask.period); periodId++) {
            for (int precedenceId = 0; precedenceId < ttTask.jobSequence.size(); precedenceId++) {
                TtJob ttJob(ttTask.id, periodId, precedenceId, ttTask.jobSequence[precedenceId],
                            ttTask.deadline + periodId * ttTask.period - ((int)ttTask.jobSequence.size() - (precedenceId + 1)) * ttTask.c,
                            NIL, ttTask.c, NIL);
                if (precedenceId == 0) {
                    ttJob.r = ttTask.r + periodId * ttTask.period;
                }
                ttJobsSamePeriod.push_back(ttJob);
            }
            ttJobsSameTask.push_back(ttJobsSamePeriod);
            ttJobsSamePeriod.clear();
        }
        allTtJobs.push_back(ttJobsSameTask);
        ttJobsSameTask.clear();
    }
    return allTtJobs;
}

vector<vector<vector<EtJob>>> transformTtJobsToEtJobs(vector<vector<vector<TtJob>>> &ttJobs, int numEtTasks) {
    vector<EtJob> fixedEtJobsSamePeriod;
    vector<vector<EtJob>> fixedEtJobsSameTask;
    vector<vector<vector<EtJob>>> fixedEtJobs;
    for (auto & ttTaskSet : ttJobs) {
        for (auto & ttTaskPeriod : ttTaskSet) {
            for (auto & ttJob : ttTaskPeriod) {
                EtJob etJob(ttJob.taskId + numEtTasks, ttJob.periodId, ttJob.precedenceId, ttJob.processorId, ttJob.startTime + ttJob.c,
                            ttJob.startTime, ttJob.startTime, ttJob.startTime, ttJob.c, ttJob.c, ttJob.c, 0);
                fixedEtJobsSamePeriod.push_back(etJob);
            }
            fixedEtJobsSameTask.push_back(fixedEtJobsSamePeriod);
            fixedEtJobsSamePeriod.clear();
        }
        fixedEtJobs.push_back(fixedEtJobsSameTask);
        fixedEtJobsSameTask.clear();
    }
    return fixedEtJobs;
}

vector<vector<vector<int>>> extractStartTimes(vector<vector<vector<TtJob>>> &ttJobs) {
    vector<int> startTimesSamePeriod;
    vector<vector<int>> startTimesSameTask;
    vector<vector<vector<int>>> startTimes;
    for (auto & ttTaskSet : ttJobs) {
        for (auto & ttTaskPeriod : ttTaskSet) {
            for (auto & ttJob : ttTaskPeriod) {
                startTimesSamePeriod.push_back(ttJob.startTime);
            }
            startTimesSameTask.push_back(startTimesSamePeriod);
            startTimesSamePeriod.clear();
        }
        startTimes.push_back(startTimesSameTask);
        startTimesSameTask.clear();
    }
    return startTimes;
}

vector<vector<vector<int>>> getStartTimeIndexes(vector<vector<vector<TtJob>>> &ttJobs) {
    int index = 0;
    vector<int> startTimeIndexesSamePeriod;
    vector<vector<int>> startTimeIndexesSameTask;
    vector<vector<vector<int>>> startTimeIndexes;
    for (auto & ttTaskSet : ttJobs) {
        for (auto & ttTaskPeriod : ttTaskSet) {
            for (auto & ttJob : ttTaskPeriod) {
                startTimeIndexesSamePeriod.push_back(index);
                index++;
            }
            startTimeIndexesSameTask.push_back(startTimeIndexesSamePeriod);
            startTimeIndexesSamePeriod.clear();
        }
        startTimeIndexes.push_back(startTimeIndexesSameTask);
        startTimeIndexesSameTask.clear();
    }
    return startTimeIndexes;
}

vector<vector<vector<int>>> transformStartTimes(vector<vector<vector<int>>> &startTimeIndexes, vector<int> &startTimesVector) {
    int index = 0;
    vector<int> startTimesSamePeriod;
    vector<vector<int>> startTimesSameTask;
    vector<vector<vector<int>>> startTimes;
    for (auto & startTimeIndexesSameTask : startTimeIndexes) {
        for (auto & startTimeIndexesSamePeriod : startTimeIndexesSameTask) {
            for (auto & startTimeIndex : startTimeIndexesSamePeriod) {
                startTimesSamePeriod.push_back(startTimesVector[index]);
                index++;
            }
            startTimesSameTask.push_back(startTimesSamePeriod);
            startTimesSamePeriod.clear();
        }
        startTimes.push_back(startTimesSameTask);
        startTimesSameTask.clear();
    }
    return startTimes;
}

void printStartTimes(vector<vector<vector<int>>> &startTimes) {
    cout << "START TIMES: ";
    cout << "{";
    if (startTimes.empty()) {
        cout << "}" << endl;
        return;
    }
    for (auto & x : startTimes) {
        cout << "{";
        for (auto & y : x) {
            cout << "{";
            for (auto & startTime : y) {
                cout << startTime << ",";
            }
            cout << "\b \b},";
        }
        cout << "\b \b},";
    }
    cout << "\b \b}" << endl;
}

void getNextExecutionTimes(EtJob &etJob, vector<int> &executionTimes, int sequenceLength) {
    int idx = sequenceLength - 1;
    while (idx >= 0) {
        if (executionTimes[idx] == etJob.cMin) {
            executionTimes[idx] = etJob.cMax;
            idx--;
        } else {
            executionTimes[idx]--;
            break;
        }
    }
}

long long getNumberOfPossibleScenarios(vector<EtTask> &etTasks, int hyperperiod) {
    long long numberOfPossibleScenarios = 1;
    int numPossibleExecutionTimes;
    int numPossibleReleaseTimes;
    long long temp;
    for (auto & etTask : etTasks) {
        numPossibleExecutionTimes = etTask.cMax + 1 - etTask.cMin;
        numPossibleReleaseTimes = etTask.rMax + 1 - etTask.rMin;
        if ((long double)numeric_limits<long long>::max() < (long double)pow((long double)numPossibleReleaseTimes * (long double)pow((long double)numPossibleExecutionTimes, etTask.jobSequence.size()), hyperperiod / etTask.period)) {
            cerr << "WARNING: number of possible scenarios overflowed maximal value of long long int; the instance has too many scenarios to go through with brute force test in reasonable amount of time; returning -1" << endl;
            return (long long)NIL;
        }
        temp = (long long)pow((long long)numPossibleReleaseTimes * (long long)pow((long long)numPossibleExecutionTimes, etTask.jobSequence.size()), hyperperiod / etTask.period);
        if ((long double)numeric_limits<long long>::max() < (long double)numberOfPossibleScenarios * (long double)temp) {
            cerr << "WARNING: number of possible scenarios overflowed maximal value of long long int; the instance has too many scenarios to go through with brute force test in reasonable amount of time; returning -1" << endl;
            return (long long)NIL;
        }
        numberOfPossibleScenarios *= temp;
    }
    return numberOfPossibleScenarios;
}

vector<int> edfFpPolicy(vector<EtJob> &applicableEtJobs, vector<int> &processorTimeStamps) {
    vector<int> pickedEtJobIds(3);
    int currentlyPickedJob = NIL;
    int processorIdOfCurrentlyPickedJob;
    int processorIdOfChallengerJob;
    for (int i = 0; i < applicableEtJobs.size(); i++) {
        if (applicableEtJobs[i].r > processorTimeStamps[applicableEtJobs[i].processorId]) {
            continue;
        }
        if (currentlyPickedJob == NIL) {
            currentlyPickedJob = i;
            processorIdOfCurrentlyPickedJob = applicableEtJobs[currentlyPickedJob].processorId;
        } else {
            processorIdOfChallengerJob = applicableEtJobs[i].processorId;
            if (processorTimeStamps[processorIdOfChallengerJob] < processorTimeStamps[processorIdOfCurrentlyPickedJob] || (processorIdOfChallengerJob == processorIdOfCurrentlyPickedJob && compare(applicableEtJobs[i], applicableEtJobs[currentlyPickedJob]))) {
                currentlyPickedJob = i;
                processorIdOfCurrentlyPickedJob = processorIdOfChallengerJob;
            }
        }
    }
    pickedEtJobIds[0] = applicableEtJobs[currentlyPickedJob].taskId;
    pickedEtJobIds[1] = applicableEtJobs[currentlyPickedJob].periodId;
    pickedEtJobIds[2] = applicableEtJobs[currentlyPickedJob].precedenceId;
    applicableEtJobs.erase(applicableEtJobs.begin() + currentlyPickedJob);
    return pickedEtJobIds;
}

bool compare(EtJob &etJob1, EtJob &etJob2) {
    if (etJob1.priority < etJob2.priority) {
        return true;
    } else if (etJob1.priority == etJob2.priority) {
        if (etJob1.deadline < etJob2.deadline) {
            return true;
        } else if (etJob1.deadline == etJob2.deadline) {
            if (etJob1.taskId < etJob2.taskId) {
                return true;
            } else {
                return false;
            }
        }
        return false;
    }
    return false;
}

bool isIntersectionOfProcessorIntervals(vector<pair<int, int>> &targetIntervals, vector<pair<int, int>> &sourceIntervals) {
    for (int i = 0; i < targetIntervals.size(); i++) {
        if (max(targetIntervals[i].first, sourceIntervals[i].first) > min(targetIntervals[i].second, sourceIntervals[i].second)) {
            return false;
        }
    }
    return true;
}

bool isMergeableSetOfReleaseIntervals(vector<EtJob> &targetApplicableJobs, vector<EtJob> &sourceApplicableJobs, vector<pair<int, int>> &targetIntervals, vector<pair<int, int>> &sourceIntervals) {
    for (int i = 0; i < targetApplicableJobs.size(); i++) {
        if (targetApplicableJobs[i].precedenceId > 0) {
            if (targetApplicableJobs[i].rMin != sourceApplicableJobs[i].rMin || targetApplicableJobs[i].rMax != sourceApplicableJobs[i].rMax) {
                if (!validReleaseIntervals(max(targetIntervals[targetApplicableJobs[i].processorId].first, sourceIntervals[sourceApplicableJobs[i].processorId].first), targetApplicableJobs[i].rMin, targetApplicableJobs[i].rMax, sourceApplicableJobs[i].rMin, sourceApplicableJobs[i].rMax)) {
                    return false;
                }
            }
        }
    }
    return true;
}

bool validReleaseIntervals(int maxEFT, int targetRMin, int targetRMax, int sourceRMin, int sourceRMax) {
    if (targetRMax == sourceRMax) {
        if (targetRMin <= maxEFT && sourceRMin <= maxEFT) {
            return true;
        }
    } else {
        if (targetRMax <= maxEFT && sourceRMax <= maxEFT) {
            return true;
        }
    }
    return false;
}