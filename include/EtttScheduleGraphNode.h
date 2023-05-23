#ifndef ETTT_DEDICATED_MULTICORE_ETTTSCHEDULEGRAPHNODE_H
#define ETTT_DEDICATED_MULTICORE_ETTTSCHEDULEGRAPHNODE_H

#include <vector>
#include "EtJob.h"
#include "TtJob.h"
using namespace std;

class EtttScheduleGraphNode {
public:
    EtttScheduleGraphNode(int levelId, int numDoneTtJobs, bool deadlineMiss, vector<pair<int, int>> &processorIntervals, vector<EtJob> &applicableEtJobs,
                          vector<TtJob> &applicableTtJobs, vector<int> &doneEtJobs, vector<int> &ttStartTimes);
    int levelId;
    int numDoneTtJobs;
    bool deadlineMiss;
    vector<pair<int, int>> processorIntervals;
    vector<EtJob> applicableEtJobs;
    vector<TtJob> applicableTtJobs;
    vector<int> doneEtJobs;
    vector<int> ttStartTimes;
};

#endif