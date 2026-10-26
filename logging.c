#include "logging.h"

void log_pipe_opening(FILE * pipe_file, int start, int dst){
    fprintf(pipe_file, log_pipe_opened_fmt, start, getpid(), dst);
}

void log_process_startup(timestamp_t timestamp, ProcessDescription * pd){
    printf(log_started_fmt, timestamp, pd->ld, getpid(), getppid(), pd->balance);
    fprintf(pd->events_log, log_started_fmt, timestamp, pd->ld, getpid(), getppid(), pd->balance);
}

void log_start_message_receival(timestamp_t timestamp, ProcessDescription * pd){
    printf(log_received_all_started_fmt, timestamp, pd->ld);
    fprintf(pd->events_log, log_received_all_started_fmt, timestamp, pd->ld);
}

void log_process_completion(timestamp_t timestamp, ProcessDescription * pd){
    printf(log_done_fmt, timestamp, pd->ld, pd->balance);
    fprintf(pd->events_log, log_done_fmt, timestamp, pd->ld, pd->balance);
}

void log_completion_message_receival(timestamp_t timestamp, ProcessDescription * pd){
    printf(log_received_all_done_fmt, timestamp, pd->ld);
    fprintf(pd->events_log, log_received_all_done_fmt, timestamp, pd->ld);
}

void log_transfer_initiation(timestamp_t timestamp, ProcessDescription * pd, balance_t payload, local_id dst){
    printf(log_transfer_out_fmt, timestamp, pd->ld, payload, dst);
    fprintf(pd->events_log, log_transfer_out_fmt, timestamp, pd->ld, payload, dst);
}

void log_transfer_receival(timestamp_t timestamp, ProcessDescription * pd, balance_t payload, local_id src){
    printf(log_transfer_in_fmt, timestamp, pd->ld, payload, src);
    fprintf(pd->events_log, log_transfer_in_fmt, timestamp, pd->ld, payload, src);
}
