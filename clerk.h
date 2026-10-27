//
// Created by ubuntu on 10/31/23.
//

#ifndef PA7_CLERK_H
#define PA7_CLERK_H

#include "process.h"
#include "util.h"
#include "banking.h"
#include <string.h>

typedef struct {
    int active;
    int recorded;
    timestamp_t time[MAX_PROCESS_ID];
    balance_t balance;
    balance_t pending;
    int closed[MAX_PROCESS_ID];
    local_id open_channels;
} BalanceSnapshot;

void clerk_routine(ProcessDescription pd);

int useful_job(ProcessDescription * pd);

void handle_transfer(ProcessDescription * pd, Message * msg);

void handle_snapshot_vtime(ProcessDescription * pd, BalanceSnapshot * snapshot, Message * msg);

void track_snapshot(ProcessDescription * pd, BalanceSnapshot * snapshot, Message * msg);

#endif //PA7_CLERK_H
