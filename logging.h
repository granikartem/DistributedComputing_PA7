#ifndef PA7_LOGGING_H
#define PA7_LOGGING_H

#include <stdio.h>
#include "common.h"
#include "pa2345.h"
#include "process.h"

static const char * const log_pipe_opened_fmt =
        "Pipe from process %1d (pid %5d) OPENED to process %1d \n";

void log_pipe_opening(FILE * pipe_file, int start, int dst);

void log_process_startup(timestamp_t timestamp, ProcessDescription * pd);

void log_start_message_receival(timestamp_t timestamp, ProcessDescription * pd);

void log_process_completion(timestamp_t timestamp, ProcessDescription * pd);

void log_completion_message_receival(timestamp_t timestamp, ProcessDescription * pd);

void log_transfer_initiation(timestamp_t timestamp, ProcessDescription * pd, balance_t payload, local_id dst);

void log_transfer_receival(timestamp_t timestamp, ProcessDescription * pd, balance_t payload, local_id src);

void log_snapshot(ProcessDescription * pd, const timestamp_t * time, balance_t balance, balance_t * balance_array, balance_t pending, balance_t * pending_array);

/** Enables EMPTY send/receive lines (-v). Set before fork, children inherit it. */
void set_verbose_logging(int enabled);

void log_empty_sending(timestamp_t timestamp, ProcessDescription * pd, local_id dst);

void log_empty_receival(timestamp_t timestamp, ProcessDescription * pd, local_id src);

#endif //PA7_LOGGING_H
