#include "../../include/EtttInstance.h"

EtttInstance::EtttInstance(vector<EtTask> &etTasks, vector<TtTask> &ttTasks, int numProcessors) {
    this->etTasks = etTasks;
    this->ttTasks = ttTasks;
    this->numProcessors = numProcessors;
}