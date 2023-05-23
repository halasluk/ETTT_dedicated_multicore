#include "../../include/instanceParser.h"

void saveEtInstances(vector<EtInstance> &instances, const string& outputFile) {
    ofstream outData;
    outData.open(outputFile);
    int numInstances = instances.size();
    int numTasks;
    int numProcessors;
    int jobSequenceLength;
    outData << numInstances << endl;
    for (int instanceIdx = 0; instanceIdx < numInstances; instanceIdx++) {
        numTasks = instances[instanceIdx].etTasks.size();
        numProcessors = instances[instanceIdx].numProcessors;
        outData << numTasks << " " << numProcessors << endl;
        for (int taskIdx = 0; taskIdx < numTasks; taskIdx++) {
            outData << instances[instanceIdx].etTasks[taskIdx].id << " ";
            outData << instances[instanceIdx].etTasks[taskIdx].period << " ";
            outData << instances[instanceIdx].etTasks[taskIdx].deadline << " ";
            outData << instances[instanceIdx].etTasks[taskIdx].rMin << " ";
            outData << instances[instanceIdx].etTasks[taskIdx].rMax << " ";
            outData << instances[instanceIdx].etTasks[taskIdx].cMin << " ";
            outData << instances[instanceIdx].etTasks[taskIdx].cMax << " ";
            outData << instances[instanceIdx].etTasks[taskIdx].priority << " ";
            jobSequenceLength = instances[instanceIdx].etTasks[taskIdx].jobSequence.size();
            outData << jobSequenceLength << " ";
            for (int jobIdx = 0; jobIdx < jobSequenceLength; jobIdx++) {
                outData << instances[instanceIdx].etTasks[taskIdx].jobSequence[jobIdx] << " ";
            }
            outData << endl;
        }
    }
    outData.close();
}

void saveEtttInstances(vector<EtttInstance> &instances, const string& outputFile) {
    ofstream outData;
    outData.open(outputFile);
    int numInstances = instances.size();
    int numEtTasks;
    int numTtTasks;
    int numProcessors;
    int jobSequenceLength;
    outData << numInstances << endl;
    for (int instanceIdx = 0; instanceIdx < numInstances; instanceIdx++) {
        numEtTasks = instances[instanceIdx].etTasks.size();
        numTtTasks = instances[instanceIdx].ttTasks.size();
        numProcessors = instances[instanceIdx].numProcessors;
        outData << numEtTasks << " " << numTtTasks << " " << numProcessors << endl;
        for (int etTaskIdx = 0; etTaskIdx < numEtTasks; etTaskIdx++) {
            outData << instances[instanceIdx].etTasks[etTaskIdx].id << " ";
            outData << instances[instanceIdx].etTasks[etTaskIdx].period << " ";
            outData << instances[instanceIdx].etTasks[etTaskIdx].deadline << " ";
            outData << instances[instanceIdx].etTasks[etTaskIdx].rMin << " ";
            outData << instances[instanceIdx].etTasks[etTaskIdx].rMax << " ";
            outData << instances[instanceIdx].etTasks[etTaskIdx].cMin << " ";
            outData << instances[instanceIdx].etTasks[etTaskIdx].cMax << " ";
            outData << instances[instanceIdx].etTasks[etTaskIdx].priority << " ";
            jobSequenceLength = instances[instanceIdx].etTasks[etTaskIdx].jobSequence.size();
            outData << jobSequenceLength << " ";
            for (int jobIdx = 0; jobIdx < jobSequenceLength; jobIdx++) {
                outData << instances[instanceIdx].etTasks[etTaskIdx].jobSequence[jobIdx] << " ";
            }
            outData << endl;
        }
        for (int ttTaskIdx = 0; ttTaskIdx < numTtTasks; ttTaskIdx++) {
            outData << instances[instanceIdx].ttTasks[ttTaskIdx].id << " ";
            outData << instances[instanceIdx].ttTasks[ttTaskIdx].period << " ";
            outData << instances[instanceIdx].ttTasks[ttTaskIdx].deadline << " ";
            outData << instances[instanceIdx].ttTasks[ttTaskIdx].r << " ";
            outData << instances[instanceIdx].ttTasks[ttTaskIdx].c << " ";
            jobSequenceLength = instances[instanceIdx].ttTasks[ttTaskIdx].jobSequence.size();
            outData << jobSequenceLength << " ";
            for (int jobIdx = 0; jobIdx < jobSequenceLength; jobIdx++) {
                outData << instances[instanceIdx].ttTasks[ttTaskIdx].jobSequence[jobIdx] << " ";
            }
            outData << endl;
        }
    }
    outData.close();
}

vector<EtInstance> loadEtInstances(const string& inputFile) {
    ifstream inData;
    inData.open(inputFile);
    vector<EtInstance> instances;
    vector<EtTask> etTasks;
    int numInstances;
    int numTasks;
    int numProcessors;
    int jobSequenceLength;
    int id;
    int period;
    int deadline;
    int rMin;
    int rMax;
    int cMin;
    int cMax;
    int priority;
    int tempJob;
    vector<int> jobSequence;
    inData >> numInstances;
    for (int instanceIdx = 0; instanceIdx < numInstances; instanceIdx++) {
        inData >> numTasks;
        inData >> numProcessors;
        for (int taskIdx = 0; taskIdx < numTasks; taskIdx++) {
            inData >> id;
            inData >> period;
            inData >> deadline;
            inData >> rMin;
            inData >> rMax;
            inData >> cMin;
            inData >> cMax;
            inData >> priority;
            inData >> jobSequenceLength;
            for (int jobIdx = 0; jobIdx < jobSequenceLength; jobIdx++) {
                inData >> tempJob;
                jobSequence.push_back(tempJob);
            }
            EtTask etTask(id, period, deadline, rMin, rMax, cMin, cMax, priority, jobSequence);
            etTasks.push_back(etTask);
            jobSequence.clear();
        }
        EtInstance instance(etTasks, numProcessors);
        instances.push_back(instance);
        etTasks.clear();
    }
    inData.close();
    return instances;
}

vector<EtttInstance> loadEtttInstances(const string& inputFile) {
    ifstream inData;
    inData.open(inputFile);
    vector<EtttInstance> instances;
    vector<EtTask> etTasks;
    vector<TtTask> ttTasks;
    int numInstances;
    int numEtTasks;
    int numTtTasks;
    int numProcessors;
    int jobSequenceLength;
    int id;
    int period;
    int deadline;
    int r;
    int rMin;
    int rMax;
    int c;
    int cMin;
    int cMax;
    int priority;
    int tempJob;
    vector<int> jobSequence;
    inData >> numInstances;
    for (int instanceIdx = 0; instanceIdx < numInstances; instanceIdx++) {
        inData >> numEtTasks;
        inData >> numTtTasks;
        inData >> numProcessors;
        for (int taskIdx = 0; taskIdx < numEtTasks; taskIdx++) {
            inData >> id;
            inData >> period;
            inData >> deadline;
            inData >> rMin;
            inData >> rMax;
            inData >> cMin;
            inData >> cMax;
            inData >> priority;
            inData >> jobSequenceLength;
            for (int jobIdx = 0; jobIdx < jobSequenceLength; jobIdx++) {
                inData >> tempJob;
                jobSequence.push_back(tempJob);
            }
            EtTask etTask(id, period, deadline, rMin, rMax, cMin, cMax, priority, jobSequence);
            etTasks.push_back(etTask);
            jobSequence.clear();
        }
        for (int taskIdx = 0; taskIdx < numTtTasks; taskIdx++) {
            inData >> id;
            inData >> period;
            inData >> deadline;
            inData >> r;
            inData >> c;
            inData >> jobSequenceLength;
            for (int jobIdx = 0; jobIdx < jobSequenceLength; jobIdx++) {
                inData >> tempJob;
                jobSequence.push_back(tempJob);
            }
            TtTask ttTask(id, period, deadline, r, c, jobSequence);
            ttTasks.push_back(ttTask);
            jobSequence.clear();
        }
        EtttInstance instance(etTasks, ttTasks, numProcessors);
        instances.push_back(instance);
        etTasks.clear();
        ttTasks.clear();
    }
    inData.close();
    return instances;
}