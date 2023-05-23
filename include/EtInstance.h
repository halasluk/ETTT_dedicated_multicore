#ifndef ETTT_DEDICATED_MULTICORE_ETINSTANCE_H
#define ETTT_DEDICATED_MULTICORE_ETINSTANCE_H

#include "EtTask.h"

class EtInstance {
public:
    EtInstance(vector<EtTask> &etTasks, int numProcessors);
    vector<EtTask> etTasks;
    int numProcessors;
};

#endif