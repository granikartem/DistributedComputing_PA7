#ifndef PA1_LOGGING_H
#define PA1_LOGGING_H

#include <stdio.h>
#include "common.h"
#include "pa1.h"
#include "process.h"

static const char * const log_pipe_opened_fmt =
        "Pipe from process %1d (pid %5d) OPENED to process %1d \n";

void log_pipe_opening(FILE * pipe_file, int start, int dst);

void log_process_startup(ProcessDescription * pd);

void log_start_message_receival(ProcessDescription * pd);

void log_process_completion(ProcessDescription * pd);

void log_completion_message_receival(ProcessDescription * pd);

#endif //PA1_LOGGING_H
