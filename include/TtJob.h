#ifndef ETTT_DEDICATED_MULTICORE_TTJOB_H
#define ETTT_DEDICATED_MULTICORE_TTJOB_H

class TtJob {
public:
    TtJob(int taskId, int periodId, int precedenceId, int processorId, int deadline, int r, int c, int startTime);
    int taskId;
    int periodId;
    int precedenceId;
    int processorId;
    int deadline;
    int r;
    int c;
    int startTime;
};

#endif