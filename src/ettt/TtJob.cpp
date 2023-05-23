#include "../../include/TtJob.h"

TtJob::TtJob(int taskId, int periodId, int precedenceId, int processorId, int deadline, int r, int c, int startTime) {
    this->taskId = taskId;
    this->periodId = periodId;
    this->precedenceId = precedenceId;
    this->processorId = processorId;
    this->deadline = deadline;
    this->r = r;
    this->c = c;
    this->startTime = startTime;
}