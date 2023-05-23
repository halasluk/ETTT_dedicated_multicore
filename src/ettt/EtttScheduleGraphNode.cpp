#include "../../include/EtttScheduleGraphNode.h"

EtttScheduleGraphNode::EtttScheduleGraphNode(int levelId, int numDoneTtJobs, bool deadlineMiss, vector<pair<int, int>> &processorIntervals,
                                             vector<EtJob> &applicableEtJobs, vector<TtJob> &applicableTtJobs,
                                             vector<int> &doneEtJobs, vector<int> &ttStartTimes) {
    this->levelId = levelId;
    this->numDoneTtJobs = numDoneTtJobs;
    this->deadlineMiss = deadlineMiss;
    this->processorIntervals = processorIntervals;
    this->applicableEtJobs = applicableEtJobs;
    this->applicableTtJobs = applicableTtJobs;
    this->doneEtJobs = doneEtJobs;
    this->ttStartTimes = ttStartTimes;
}