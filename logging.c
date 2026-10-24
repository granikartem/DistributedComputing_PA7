#include "logging.h"

void log_pipe_opening(FILE * pipe_file, int start, int dst){
    fprintf(pipe_file, log_pipe_opened_fmt, start, getpid(), dst);
}

void log_process_startup(ProcessDescription * pd){
    printf(log_started_fmt, pd->ld, getpid(), getppid());
    fprintf(pd->events_log, log_started_fmt, pd->ld, getpid(), getppid());
}

void log_start_message_receival(ProcessDescription * pd){
    printf(log_received_all_started_fmt, pd->ld);
    fprintf(pd->events_log, log_received_all_started_fmt, pd->ld);
}

void log_process_completion(ProcessDescription * pd){
    printf(log_done_fmt, pd->ld);
    fprintf(pd->events_log, log_done_fmt, pd->ld);
}

void log_completion_message_receival(ProcessDescription * pd){
    printf(log_received_all_done_fmt, pd->ld);
    fprintf(pd->events_log, log_received_all_done_fmt, pd->ld);
}

