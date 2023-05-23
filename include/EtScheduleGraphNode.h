#ifndef ETTT_DEDICATED_MULTICORE_ETSCHEDULEGRAPHNODE_H
#define ETTT_DEDICATED_MULTICORE_ETSCHEDULEGRAPHNODE_H

#include <vector>
#include "EtJob.h"
using namespace std;

class EtScheduleGraphNode {
public:
    EtScheduleGraphNode(int levelId, bool deadlineMiss, vector<pair<int, int>> &processorIntervals, vector<EtJob> &applicableJobs,
                        vector<int> &doneJobsFromTask);
    int levelId;
    bool deadlineMiss;
    vector<pair<int, int>> processorIntervals;
    vector<EtJob> applicableJobs;
    vector<int> doneJobsFromTask;
};

#endif