//
// Created by ubuntu on 9/25/23.
//

#ifndef PA3_PROCESS_H
#define PA3_PROCESS_H

#include <unistd.h>
#include <stdio.h>
#include "ipc.h"
#include "banking.h"

typedef enum{
    CLERK,
    CLIENT
} ProcessType;


struct TransferList {
    local_id   s_src;
    local_id   s_dst;
    balance_t  s_amount;
    timestamp_t start_time;
    struct TransferList * next;
}__attribute__((packed));

typedef struct{
    int write;
    int read;
} my_pipe;

typedef struct {
    local_id ld;
    local_id size;
    balance_t balance;
    ProcessType type;
    FILE * events_log;
    my_pipe** pipes;
    local_id last_sender;
    struct TransferList * head;
    struct TransferList * tail;
} __attribute__((packed)) ProcessDescription;

#endif //PA3_PROCESS_H
