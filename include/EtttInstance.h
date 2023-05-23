#ifndef ETTT_DEDICATED_MULTICORE_ETTTINSTANCE_H
#define ETTT_DEDICATED_MULTICORE_ETTTINSTANCE_H

#include "EtTask.h"
#include "TtTask.h"

class EtttInstance {
public:
    EtttInstance(vector<EtTask> &etTasks, vector<TtTask> &ttTasks, int numProcessors);
    vector<EtTask> etTasks;
    vector<TtTask> ttTasks;
    int numProcessors;
};

#endif