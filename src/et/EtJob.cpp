#include "../../include/EtJob.h"

EtJob::EtJob(int taskId, int periodId, int precedenceId, int processorId, int deadline, int r, int rMin,
             int rMax, int c, int cMin, int cMax, int priority) {
    this->taskId = taskId;
    this->periodId = periodId;
    this->precedenceId = precedenceId;
    this->processorId = processorId;
    this->deadline = deadline;
    this->r = r;
    this->rMin = rMin;
    this->rMax = rMax;
    this->c = c;
    this->cMin = cMin;
    this->cMax = cMax;
    this->priority = priority;
}