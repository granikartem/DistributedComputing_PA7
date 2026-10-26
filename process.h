//
// Created by ubuntu on 9/25/23.
//

#ifndef PA7_PROCESS_H
#define PA7_PROCESS_H

#include <unistd.h>
#include <stdio.h>
#include "ipc.h"
#include "banking.h"

typedef struct{
    int write;
    int read;
} my_pipe;

typedef struct {
    local_id ld;
    local_id size;
    balance_t balance;
    FILE * events_log;
    my_pipe** pipes;
    local_id last_sender;
} __attribute__((packed)) ProcessDescription;

#endif //PA7_PROCESS_H
