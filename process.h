//
// Created by ubuntu on 9/25/23.
//

#ifndef PA1_PROCESS_H
#define PA1_PROCESS_H

#include <unistd.h>
#include <stdio.h>
#include "ipc.h"

typedef struct{
    int write;
    int read;
} my_pipe;

typedef enum {
    THINKING,
    HUNGRY,
    EATING
} PhilosopherState;

typedef struct {
    int has_fork;
    int dirty;
    int has_request;
} Fork;

typedef struct {
    my_pipe** pipes;
    local_id ld;
    local_id size;
    FILE * events_log;
    int mutexl;
    local_id last_sender;
    int done_received;
    PhilosopherState state;
    Fork forks[MAX_PROCESS_ID + 1];
} ProcessDescription;

#endif //PA1_PROCESS_H
