#include "../../include/EtInstance.h"

EtInstance::EtInstance(vector<EtTask> &etTasks, int numProcessors) {
    this->etTasks = etTasks;
    this->numProcessors = numProcessors;
}