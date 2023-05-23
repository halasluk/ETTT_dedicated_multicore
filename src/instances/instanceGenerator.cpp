#include "../../include/instanceGenerator.h"

// ET RUNTIME EVAL
void generateEtInstances(const string& outputFile, int numVerts, bool random) {
    vector<EtInstance> instances;
    vector<vector<int>> jobSequences;
    vector<double> taskUtilization;
    double maxUtilization = 0.3;
    // HERE: hyperperiod is max of hyperperiod chosen by user and number of vertices (minus 1)
    // due to worst case scenario: random tree with n vertices can be a line with maximal job sequence
    // length n-1, task with such job sequence needs to have period at least n-1 not to be trivially unschedulable,
    // the rest is taken care of in procedure generateEtInstance();
    int hyperperiod = max(10000000, numVerts - 1);
    int minPeriod = 1000000;
    double variationPercentage;
    double releaseShiftPercentage;
    double deadlineShiftPercentage;
    int minPriority = 1;
    int maxPriority = 5;
    int numInstances = 1000;
    int numTasks;
    random_device seed;
    vector<int> possiblePeriods = getAllDivisorsOfNumBiggerThan(hyperperiod, minPeriod);
    NetworkTopologyGraph graph(numVerts, random);
    graph.generateRandomTreeTopologyGraph();
    for (int a = 0; a < 3; a++) {
        for (int b = 0; b < 3; b++) {
            numTasks = 0;
            if (a == 0) {
                variationPercentage = 0.0;
            } else if (a == 1) {
                variationPercentage = 0.3;
            } else if (a == 2) {
                variationPercentage = 0.6;
            }
            if (b == 0) {
                releaseShiftPercentage = 0.1;
                deadlineShiftPercentage = 0.1;
            } else if (b == 1) {
                releaseShiftPercentage = 0.2;
                deadlineShiftPercentage = 0.2;
            } else if (b == 2) {
                releaseShiftPercentage = 0.3;
                deadlineShiftPercentage = 0.3;
            }
            for (int i = 0; i < numInstances; i++) {
                if (i % 50 == 0) {
                    numTasks++;
                }
                jobSequences = graph.generateJobSequences(numTasks);
                graph.getRelaxedJobSequences(jobSequences);
                taskUtilization = getTaskUtilization(graph.numUsedProcessors, maxUtilization, jobSequences);
                EtInstance instance = generateEtInstance(jobSequences, possiblePeriods, taskUtilization, variationPercentage, releaseShiftPercentage, deadlineShiftPercentage, minPriority, maxPriority, seed, graph.numUsedProcessors);
                instances.push_back(instance);
            }
            saveEtInstances(instances, outputFile + to_string(a * 3 + b) + ".dat");
            instances.clear();
        }
    }
}

// ETTT RUNTIME EVAL
void generateEtttInstances(const string& outputFile, int numVerts, bool random) {
    vector<EtttInstance> instances;
    vector<vector<int>> jobSequences;
    vector<double> taskUtilization;
    double maxUtilization = 0.3;
    // HERE: hyperperiod is max of hyperperiod chosen by user and number of vertices (minus 1)
    // due to worst case scenario: random tree with n vertices can be a line with maximal job sequence
    // length n-1, task with such job sequence needs to have period at least n-1 not to be trivially unschedulable,
    // the rest is taken care of in procedure generateEtttInstance();
    int hyperperiod = max(10000000, numVerts - 1);
    int minPeriod = 1000000;
    double variationPercentage;
    double releaseShiftPercentage;
    double deadlineShiftPercentage;
    int minPriority = 1;
    int maxPriority = 4;
    int numInstances = 1000;
    int numEtTasks = 8;
    int numTtTasks;
    random_device seed;
    vector<int> possiblePeriods = getAllDivisorsOfNumBiggerThan(hyperperiod, minPeriod);
    NetworkTopologyGraph graph(numVerts, random);
    graph.generateRandomTreeTopologyGraph();
    for (int a = 0; a < 3; a++) {
        for (int b = 0; b < 3; b++) {
            numTtTasks = 0;
            if (a == 0) {
                variationPercentage = 0.0;
            } else if (a == 1) {
                variationPercentage = 0.3;
            } else if (a == 2) {
                variationPercentage = 0.6;
            }
            if (b == 0) {
                releaseShiftPercentage = 0.1;
                deadlineShiftPercentage = 0.1;
            } else if (b == 1) {
                releaseShiftPercentage = 0.2;
                deadlineShiftPercentage = 0.2;
            } else if (b == 2) {
                releaseShiftPercentage = 0.3;
                deadlineShiftPercentage = 0.3;
            }
            for (int i = 0; i < numInstances; i++) {
                if (i % 50 == 0) {
                    numTtTasks++;
                }
                jobSequences = graph.generateJobSequences(numEtTasks + numTtTasks);
                graph.getRelaxedJobSequences(jobSequences);
                taskUtilization = getTaskUtilization(graph.numUsedProcessors, maxUtilization, jobSequences);
                EtttInstance instance = generateEtttInstance(jobSequences, possiblePeriods, numEtTasks, taskUtilization, variationPercentage, releaseShiftPercentage, deadlineShiftPercentage, minPriority, maxPriority, seed, graph.numUsedProcessors);
                instances.push_back(instance);
            }
            saveEtttInstances(instances, outputFile + to_string(a * 3 + b) + ".dat");
            instances.clear();
        }
    }
}

vector<double> getTaskUtilization(int numProcessors, double maxUtilization, vector<vector<int>> &jobSequences) {
    vector<double> taskUtilization(jobSequences.size(), 0);
    vector<int> taskCountsOnProcessors(numProcessors, 0);
    vector<double> temp(numProcessors, 1);
    vector<vector<double>> taskProcessorMatrix(jobSequences.size(), temp);
    for (auto & task : jobSequences) {
        for (auto & id : task) {
            taskCountsOnProcessors[id]++;
        }
    }
    for (int i = 0; i < jobSequences.size(); i++) {
        for (auto & j : jobSequences[i]) {
            taskProcessorMatrix[i][j] = maxUtilization / taskCountsOnProcessors[j];
        }
    }
    for (int i = 0; i < taskUtilization.size(); i++) {
        taskUtilization[i] = *min_element(taskProcessorMatrix[i].begin(), taskProcessorMatrix[i].end());
    }
    return taskUtilization;
}

EtInstance generateEtInstance(vector<vector<int>> &jobSequences, vector<int> &possiblePeriods,
                              vector<double> &taskUtilization, double variationPercentage, double releaseShiftPercentage,
                              double deadlineShiftPercentage, int minPriority, int maxPriority, random_device &seed, int numProcessors) {
    int cMin;
    int rMax;
    int rMin;
    int deadline;
    int priority;
    int chosenPeriodIdx;
    vector<EtTask> etTasks;
    vector<int> taskPeriods;
    taskPeriods.reserve(jobSequences.size());
    for (int i = 0; i < jobSequences.size(); i++) {
        chosenPeriodIdx = (int)(seed() % possiblePeriods.size());
        taskPeriods.push_back(possiblePeriods[chosenPeriodIdx]);
        while (possiblePeriods[chosenPeriodIdx] < jobSequences[i].size()) {
            chosenPeriodIdx++;
            taskPeriods[i] = possiblePeriods[chosenPeriodIdx];
        }
    }
    vector<int> cMax = getCMax(jobSequences, taskPeriods, taskUtilization, seed);
    for (int i = 0; i < jobSequences.size(); i++) {
        cMin = getIntFromInterval(max((int)round(cMax[i] - ((cMax[i] - 1) * variationPercentage)), 1), cMax[i], seed);
        rMax = getIntFromInterval(0, min((int)round(taskPeriods[i] * releaseShiftPercentage), (taskPeriods[i] - (cMax[i] * (int)jobSequences[i].size()))), seed);
        rMin = getIntFromInterval(max((int)round(rMax - (rMax * variationPercentage)), 0), rMax, seed);
        deadline = getIntFromInterval(max((int)round(taskPeriods[i] - (taskPeriods[i] * deadlineShiftPercentage)), (rMax + (cMax[i] * (int)jobSequences[i].size()))), taskPeriods[i], seed);
        priority = getIntFromInterval(minPriority, maxPriority, seed);
        EtTask etTask(i, taskPeriods[i], deadline, rMin, rMax, cMin, cMax[i], priority, jobSequences[i]);
        etTasks.push_back(etTask);
    }
    EtInstance instance(etTasks, numProcessors);
    return instance;
}

EtttInstance generateEtttInstance(vector<vector<int>> &jobSequences, vector<int> &possiblePeriods, int numEtTasks,
                                  vector<double> &taskUtilization, double variationPercentage, double releaseShiftPercentage,
                                  double deadlineShiftPercentage, int minPriority, int maxPriority, random_device &seed, int numProcessors) {
    int cMin;
    int r;
    int rMax;
    int rMin;
    int deadline;
    int priority;
    int chosenPeriodIdx;
    vector<EtTask> etTasks;
    vector<TtTask> ttTasks;
    vector<int> taskPeriods;
    taskPeriods.reserve(jobSequences.size());
    for (int i = 0; i < jobSequences.size(); i++) {
        chosenPeriodIdx = (int)(seed() % possiblePeriods.size());
        taskPeriods.push_back(possiblePeriods[chosenPeriodIdx]);
        while (possiblePeriods[chosenPeriodIdx] < jobSequences[i].size()) {
            chosenPeriodIdx++;
            taskPeriods[i] = possiblePeriods[chosenPeriodIdx];
        }
    }
    vector<int> cMax = getCMax(jobSequences, taskPeriods, taskUtilization, seed);
    for (int i = 0; i < numEtTasks; i++) {
        cMin = getIntFromInterval(max((int)round(cMax[i] - ((cMax[i] - 1) * variationPercentage)), 1), cMax[i], seed);
        rMax = getIntFromInterval(0, min((int)round(taskPeriods[i] * releaseShiftPercentage), (taskPeriods[i] - (cMax[i] * (int)jobSequences[i].size()))), seed);
        rMin = getIntFromInterval(max((int)round(rMax - (rMax * variationPercentage)), 0), rMax, seed);
        deadline = getIntFromInterval(max((int)round(taskPeriods[i] - (taskPeriods[i] * deadlineShiftPercentage)), (rMax + (cMax[i] * (int)jobSequences[i].size()))), taskPeriods[i], seed);
        priority = getIntFromInterval(minPriority, maxPriority, seed);
        EtTask etTask(i, taskPeriods[i], deadline, rMin, rMax, cMin, cMax[i], priority, jobSequences[i]);
        etTasks.push_back(etTask);
    }
    for (int i = numEtTasks; i < jobSequences.size(); i++) {
        r = getIntFromInterval(0, min((int)round(taskPeriods[i] * releaseShiftPercentage), (taskPeriods[i] - (cMax[i] * (int)jobSequences[i].size()))), seed);
        deadline = getIntFromInterval(max((int)round(taskPeriods[i] - (taskPeriods[i] * deadlineShiftPercentage)), (r + (cMax[i] * (int)jobSequences[i].size()))), taskPeriods[i], seed);
        TtTask ttTask(i - numEtTasks, taskPeriods[i], deadline, r, cMax[i], jobSequences[i]);
        ttTasks.push_back(ttTask);
    }
    EtttInstance instance(etTasks, ttTasks, numProcessors);
    return instance;
}

int getIntFromInterval(int min, int max, random_device &seed) {
    if (max < min) {
        max = min;
    }
    int ret = (int)(seed() % ((max + 1) - min));
    return ret + min;
}

vector<int> getCMax(vector<vector<int>> &jobSequences, vector<int> &taskPeriods, vector<double> &taskUtilization, random_device &seed) {
    int cMax;
    vector<int> allCMax;
    for (int i = 0; i < jobSequences.size(); i++) {
        cMax = getIntFromInterval(1, min((int)jobSequences[i].size() * (int)round(taskPeriods[i] * taskUtilization[i]), taskPeriods[i]) / (int)jobSequences[i].size(), seed);
        allCMax.push_back(cMax);
    }
    return allCMax;
}

vector<int> getAllDivisors(int num) {
    vector<int> divisors;
    for (int i = 2; i <= (num / 2); i++) {
        if (num % i == 0) {
            divisors.push_back(i);
        }
    }
    divisors.push_back(num);
    return divisors;
}

vector<int> getAllDivisorsOfNumBiggerThan(int num, int limit) {
    vector<int> divisors;
    divisors = getAllDivisors(num);
    while (divisors[0] < limit) {
        divisors.erase(divisors.begin());
    }
    return divisors;
}