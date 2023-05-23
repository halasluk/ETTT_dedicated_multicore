#include "../../include/TtTask.h"

TtTask::TtTask(int id, int period, int deadline, int r, int c, vector<int> &jobSequence) {
    this->id = id;
    this->period = period;
    this->deadline = deadline;
    this->r = r;
    this->c = c;
    this->jobSequence = jobSequence;
}