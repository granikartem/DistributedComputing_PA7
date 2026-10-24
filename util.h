#ifndef PA1_UTIL_H
#define PA1_UTIL_H

#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <getopt.h>
#include <wait.h>
#include "process.h"
#include "common.h"
#include "logging.h"
#include "mutex.h"
#include "work.h"

local_id parse(int argc, char* argv[], int * mutexl);

ProcessDescription init_process_description(local_id n, int mutexl);

void close_unused_pipes(ProcessDescription * pd);

void child_routine(ProcessDescription pd);

int parent_routine(ProcessDescription pd);

void send_everyone(MessageType msg_type, ProcessDescription * pd);

void receive_from_everyone(MessageType msg_type, ProcessDescription * pd);

void receive_all_done(ProcessDescription * pd);

#endif //PA1_UTIL_H
