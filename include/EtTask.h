#ifndef ETTT_DEDICATED_MULTICORE_ETTASK_H
#define ETTT_DEDICATED_MULTICORE_ETTASK_H

#include <vector>
using namespace std;

class EtTask {
public:
    EtTask(int id, int period, int deadline, int rMin, int rMax, int cMin, int cMax, int priority,
           vector<int> &jobSequence);
    int id;
    int period;
    int deadline;
    int rMin;
    int rMax;
    int cMin;
    int cMax;
    int priority;
    vector<int> jobSequence;
};

#endif