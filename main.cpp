#include <iostream>
#include <string>
#include <cstring>
#include <chrono>
#include "include/instanceGenerator.h"
#include "include/EtScheduleGraphTest.h"
#include "include/bruteForceEtttSchedulabilityTest.h"
#include "include/EtttScheduleGraphTest.h"

using namespace std::chrono;
long long numPossibleScenarios;
long long numScenariosCheckedInBruteForceRun;

int main(int argc, char **argv) {
    if (argc <= 1) {
        cerr << "No input parameters provided." << endl;
        return NIL;
    }

    if (!strcmp(argv[1], "ET_P")) {
        if (argc != 5) {
            cerr << "Wrong number of input parameters: " << argc << ", expected 5." << endl;
            return NIL;
        }
        vector<EtInstance> instances = loadEtInstances(argv[2]);
        int numInstance = stoi(argv[4]);
        int numEtTasks;
        int numEtJobs;
        int hyperperiod;
        bool retScheduleGraph;
        ofstream out;
        out.open(argv[3] + to_string(numInstance) + ".dat");
        numEtTasks = instances[numInstance].etTasks.size();
        numEtJobs = 0;
        hyperperiod = getHyperperiodEt(instances[numInstance].etTasks);
        for (auto &etTask : instances[numInstance].etTasks) {
            numEtJobs += (int) etTask.jobSequence.size() * (hyperperiod / etTask.period);
        }
        out << numEtTasks << " " << numEtJobs << " ";
        auto start = high_resolution_clock::now();
        try {
            retScheduleGraph = runScheduleGraphEt(instances[numInstance].etTasks, instances[numInstance].numProcessors);
        } catch (bad_alloc &e) {
            auto stop = high_resolution_clock::now();
            auto duration = duration_cast<microseconds>(stop - start);
            out << NIL + NIL << " " << (double) duration.count() / 1000 << endl;
            out.close();
            return 0;
        }
        auto stop = high_resolution_clock::now();
        auto duration = duration_cast<microseconds>(stop - start);
        if (duration.count() >= TIME_LIMIT * 1000 * 1000) {
            out << NIL << " " << (double) (TIME_LIMIT * 1000) << endl;
            out.close();
            return 0;
        }
        out << retScheduleGraph << " " << (double) duration.count() / 1000 << endl;
        out.close();
    } else if (!strcmp(argv[1], "ETTT_P")) {
        if (argc != 6) {
            cerr << "Wrong number of input parameters: " << argc << ", expected 6." << endl;
            return NIL;
        }
        bool FGG;
        if (!strcmp(argv[5], "FGG-EB")) {
            FGG = false;
        } else {
            FGG = true;
        }
        vector<EtttInstance> instances = loadEtttInstances(argv[2]);
        int numInstance = stoi(argv[4]);
        int numEtTasks;
        int numEtJobs;
        int numTtTasks;
        int numTtJobs;
        int hyperperiod;
        vector<vector<vector<int>>> startTimesScheduleGraph;
        ofstream out;
        out.open(argv[3] + to_string(numInstance) + ".dat");
        numEtTasks = instances[numInstance].etTasks.size();
        numTtTasks = instances[numInstance].ttTasks.size();
        numEtJobs = 0;
        numTtJobs = 0;
        hyperperiod = getHyperperiodEttt(instances[numInstance].etTasks, instances[numInstance].ttTasks);
        for (auto &etTask : instances[numInstance].etTasks) {
            numEtJobs += (int) etTask.jobSequence.size() * (hyperperiod / etTask.period);
        }
        for (auto &ttTask : instances[numInstance].ttTasks) {
            numTtJobs += (int) ttTask.jobSequence.size() * (hyperperiod / ttTask.period);
        }
        out << numTtTasks << " " << numTtJobs << " ";
        auto start = high_resolution_clock::now();
        try {
            startTimesScheduleGraph = runScheduleGraphEttt(instances[numInstance].etTasks, instances[numInstance].ttTasks, instances[numInstance].numProcessors, FGG);
        } catch (bad_alloc &e) {
            auto stop = high_resolution_clock::now();
            auto duration = duration_cast<microseconds>(stop - start);
            out << NIL + NIL << " " << (double) duration.count() / 1000 << " " << numEtJobs << " " << numEtJobs << endl;
            out.close();
            return 0;
        }
        auto stop = high_resolution_clock::now();
        auto duration = duration_cast<microseconds>(stop - start);
        if (duration.count() >= TIME_LIMIT * 1000 * 1000) {
            out << NIL << " " << (double) (TIME_LIMIT * 1000) << " " << numEtTasks << " " << numEtJobs << endl;
            out.close();
            return 0;
        }
        out << !startTimesScheduleGraph.empty() << " " << (double) duration.count() / 1000 << " " << numEtTasks << " " << numEtJobs << endl;
        out.close();
    } else if (!strcmp(argv[1], "ET_C")) {
        if (argc != 3) {
            cerr << "Wrong number of input parameters: " << argc << ", expected 3." << endl;
            return NIL;
        }
        vector<EtInstance> instances = loadEtInstances(argv[2]);
        bool retScheduleGraph;
        bool retBruteForce;
        int numSchedulableBruteForce = 0;
        int numSchedulableScheduleGraph = 0;
        int numNotOK = 0;
        int numError = 0;
        for (int numInstance = 0; numInstance < instances.size(); numInstance++) {
            retScheduleGraph = runScheduleGraphEt(instances[numInstance].etTasks, instances[numInstance].numProcessors);
            retBruteForce = runBruteForceEt(instances[numInstance].etTasks, instances[numInstance].numProcessors);
            if (retBruteForce && retScheduleGraph) {
                numSchedulableBruteForce++;
                numSchedulableScheduleGraph++;
                cout << "----------INSTANCE " << numInstance << ": BOTH BF AND SAG SCHEDULABLE----------" << endl;
            } else if (retBruteForce) {
                numSchedulableBruteForce++;
                numNotOK++;
                cout << "----------INSTANCE " << numInstance << ": BF SCHEDULABLE, SAG UNSCHEDULABLE----------" << endl;
            } else if (retScheduleGraph) {
                numSchedulableScheduleGraph++;
                numError++;
                cout << "----------INSTANCE " << numInstance << ": BF UNSCHEDULABLE, SAG SCHEDULABLE----------" << endl;
            } else {
                cout << "----------INSTANCE " << numInstance << ": BOTH BF AND SAG UNSCHEDULABLE----------" << endl;
            }
        }
        cout << "TOTAL NUMBER OF INSTANCES: " << instances.size() << endl;
        cout << "NUMBER OF INSTANCES DECLARED SCHEDULABLE BY BRUTE FORCE: " << numSchedulableBruteForce << endl;
        cout << "NUMBER OF INSTANCES DECLARED SCHEDULABLE BY SCHEDULE GRAPH: " << numSchedulableScheduleGraph << endl;
        cout << "NUMBER OF INSTANCES DECLARED SCHEDULABLE BY BRUTE FORCE AND UNSCHEDULABLE BY SCHEDULE GRAPH: " << numNotOK << endl;
        cout << "NUMBER OF INSTANCES DECLARED UNSCHEDULABLE BY BRUTE FORCE AND SCHEDULABLE BY SCHEDULE GRAPH: " << numError << endl;
    } else if (!strcmp(argv[1], "ETTT_C")) {
        if (argc != 4) {
            cerr << "Wrong number of input parameters: " << argc << ", expected 4." << endl;
            return NIL;
        }
        bool FGG;
        if (!strcmp(argv[3], "FGG-EB")) {
            FGG = false;
        } else {
            FGG = true;
        }
        vector<EtttInstance> instances = loadEtttInstances(argv[2]);
        vector<vector<vector<int>>> startTimesBruteForce;
        vector<vector<vector<int>>> startTimesFixationGraph;
        int numSchedulableBruteForce = 0;
        int numSchedulableFixationGraph = 0;
        int numNotOK = 0;
        int numError = 0;
        for (int numInstance = 0; numInstance < instances.size(); numInstance++) {
            startTimesBruteForce = runBruteForceEttt(instances[numInstance].etTasks, instances[numInstance].ttTasks, instances[numInstance].numProcessors);
            startTimesFixationGraph = runScheduleGraphEttt(instances[numInstance].etTasks, instances[numInstance].ttTasks, instances[numInstance].numProcessors, FGG);
            if (!startTimesBruteForce.empty() && !startTimesFixationGraph.empty()) {
                numSchedulableBruteForce++;
                numSchedulableFixationGraph++;
                cout << "----------INSTANCE " << numInstance << ": BOTH BF AND FG SCHEDULABLE----------" << endl;
            } else if (!startTimesBruteForce.empty()) {
                numSchedulableBruteForce++;
                numNotOK++;
                cout << "----------INSTANCE " << numInstance << ": BF SCHEDULABLE, FG UNSCHEDULABLE----------" << endl;
            } else if (!startTimesFixationGraph.empty()) {
                numSchedulableFixationGraph++;
                numError++;
                cout << "----------INSTANCE " << numInstance << ": BF UNSCHEDULABLE, FG SCHEDULABLE----------" << endl;
            } else {
                cout << "----------INSTANCE " << numInstance << ": BOTH BF AND FG UNSCHEDULABLE----------" << endl;
            }
            cout << "BRUTE FORCE:" << endl;
            printStartTimes(startTimesBruteForce);
            cout << "FIXATION GRAPH:" << endl;
            printStartTimes(startTimesFixationGraph);
        }
        cout << "TOTAL NUMBER OF INSTANCES: " << instances.size() << endl;
        cout << "NUMBER OF INSTANCES DECLARED SCHEDULABLE BY BRUTE FORCE: " << numSchedulableBruteForce << endl;
        cout << "NUMBER OF INSTANCES DECLARED SCHEDULABLE BY FIXATION GRAPH: " << numSchedulableFixationGraph << endl;
        cout << "NUMBER OF INSTANCES DECLARED SCHEDULABLE BY BRUTE FORCE AND UNSCHEDULABLE BY FIXATION GRAPH: " << numNotOK << endl;
        cout << "NUMBER OF INSTANCES DECLARED UNSCHEDULABLE BY BRUTE FORCE AND SCHEDULABLE BY FIXATION GRAPH: " << numError << endl;
    } else if (!strcmp(argv[1], "ET_G")) {
        if (argc != 4) {
            cerr << "Wrong number of input parameters: " << argc << ", expected 4." << endl;
            return NIL;
        }
        int numVerts = stoi(argv[3]);
        generateEtInstances(argv[2], numVerts, true);
    } else if (!strcmp(argv[1], "ETTT_G")) {
        if (argc != 4) {
            cerr << "Wrong number of input parameters: " << argc << ", expected 4." << endl;
            return NIL;
        }
        int numVerts = stoi(argv[3]);
        generateEtttInstances(argv[2], numVerts, true);
    } else {
        cerr << "Unknown first input paramter." << endl;
        return NIL;
    }
    return 0;
}