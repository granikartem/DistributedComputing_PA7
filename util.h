#ifndef PA7_UTIL_H
#define PA7_UTIL_H

#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <getopt.h>
#include <wait.h>
#include "process.h"
#include "common.h"
#include "logging.h"
#include "vector.h"

static const char * const stop_msg_fmt =
        "%1d: process %1d asks you to STOP\n";


balance_t* parse(local_id * n, int argc, char* argv[]);

ProcessDescription init_process_description(local_id n);

void close_unused_pipes(ProcessDescription * pd);

void close_used_pipes(ProcessDescription * pd);

void send_everyone(MessageType msg_type, ProcessDescription * pd);

void receive_from_everyone(MessageType msg_type, ProcessDescription * pd);

#endif //PA7_UTIL_H
