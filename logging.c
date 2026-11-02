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

void log_snapshot(ProcessDescription * pd, const timestamp_t * time, balance_t balance, balance_t * balance_array, balance_t pending, balance_t * pending_array){
    char line[MAX_PAYLOAD_LEN];
    int len = sprintf(line, "Vector time: [");
    for(local_id i = 0; i < pd->size; i++){
        len += sprintf(line + len, "%s%d", i ? ", " : "", time[i]);
    }
    len += sprintf(line + len, "] Total balance: $%d. Pending: $%d. Process balances(pending in): [", balance, pending);
    for(local_id i = 1; i < pd->size; i++){
        len += sprintf(line + len, "%s%d(%d)", i > 1 ? ", " : "", balance_array[i], pending_array[i]);
    }
    sprintf(line + len, "]\n");
    printf("%s", line);
    fprintf(pd->events_log, "%s", line);
}

static const char * const log_empty_sent_fmt =
        "%d: process %1d sent EMPTY to process %1d\n";

static const char * const log_empty_received_fmt =
        "%d: process %1d received EMPTY from process %1d\n";

static int verbose_logging = 0;

void set_verbose_logging(int enabled){
    verbose_logging = enabled;
}

void log_empty_sending(timestamp_t timestamp, ProcessDescription * pd, local_id dst){
    if(!verbose_logging) return;
    printf(log_empty_sent_fmt, timestamp, pd->ld, dst);
    fprintf(pd->events_log, log_empty_sent_fmt, timestamp, pd->ld, dst);
}

void log_empty_receival(timestamp_t timestamp, ProcessDescription * pd, local_id src){
    if(!verbose_logging) return;
    printf(log_empty_received_fmt, timestamp, pd->ld, src);
    fprintf(pd->events_log, log_empty_received_fmt, timestamp, pd->ld, src);
}
