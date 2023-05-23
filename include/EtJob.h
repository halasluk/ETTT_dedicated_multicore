#ifndef ETTT_DEDICATED_MULTICORE_ETJOB_H
#define ETTT_DEDICATED_MULTICORE_ETJOB_H

class EtJob {
public:
    EtJob(int taskId, int periodId, int precedenceId, int processorId, int deadline, int r, int rMin,
          int rMax, int c, int cMin, int cMax, int priority);
    int taskId;
    int periodId;
    int precedenceId;
    int processorId;
    int deadline;
    int r;
    int rMin;
    int rMax;
    int c;
    int cMin;
    int cMax;
    int priority;
};

#endif