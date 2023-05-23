#include "../../include/EtTask.h"

EtTask::EtTask(int id, int period, int deadline, int rMin, int rMax, int cMin, int cMax, int priority,
               vector<int> &jobSequence) {
    this->id = id;
    this->period = period;
    this->deadline = deadline;
    this->rMin = rMin;
    this->rMax = rMax;
    this->cMin = cMin;
    this->cMax = cMax;
    this->priority = priority;
    this->jobSequence = jobSequence;
}