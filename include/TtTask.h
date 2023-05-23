#ifndef ETTT_DEDICATED_MULTICORE_TTTASK_H
#define ETTT_DEDICATED_MULTICORE_TTTASK_H

#include <vector>
using namespace std;

class TtTask {
public:
    TtTask(int id, int period, int deadline, int r, int c, vector<int> &jobSequence);
    int id;
    int period;
    int deadline;
    int r;
    int c;
    vector<int> jobSequence;
};

#endif