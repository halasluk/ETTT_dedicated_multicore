#include "../../include/EtScheduleGraphNode.h"

EtScheduleGraphNode::EtScheduleGraphNode(int levelId, bool deadlineMiss, vector<pair<int, int>> &processorIntervals, vector<EtJob> &applicableJobs,
                                         vector<int> &doneJobsFromTask) {
    this->levelId = levelId;
    this->deadlineMiss = deadlineMiss;
    this->processorIntervals = processorIntervals;
    this->applicableJobs = applicableJobs;
    this->doneJobsFromTask = doneJobsFromTask;
}