#include <string.h>
#include <errno.h>
#include "util.h"

local_id parse(int argc, char* argv[], int * mutexl){
    int n = 0;
    int a;
    struct option long_options[] = {
            {"mutexl", no_argument, NULL, 'm'},
            {NULL, 0, NULL, 0}
    };
    *mutexl = 0;
    while((a = getopt_long(argc, argv, "p:", long_options, NULL)) != -1){
        if(a == 'p'){
            n = atoi(optarg);
        }else if(a == 'm'){
            *mutexl = 1;
        }else{
            exit(EXIT_FAILURE);
        }
    }
    if(n < 1 || n > MAX_PROCESS_ID){
        exit(EXIT_FAILURE);
    }
    return (local_id) n;
}

static int set_nonblocking(int fd){
    int flags = fcntl(fd, F_GETFL);
    if(flags == -1) return -1;
    return fcntl(fd, F_SETFL, flags | O_NONBLOCK);
}

ProcessDescription init_process_description(local_id n, int mutexl){
    ProcessDescription pd;
    pd.ld = 0;
    pd.size = n + 1;
    pd.mutexl = mutexl;
    pd.events_log = fopen(events_log, "a+");
    FILE * pipe_log;
    pipe_log = fopen(pipes_log, "a+");
    pd.pipes = (my_pipe ** ) malloc(sizeof(my_pipe *) * (n + 1));
    for(int i = 0; i < n + 1; i++){
        pd.pipes[i] = (my_pipe *) malloc(sizeof(my_pipe) * (n + 1));
    }
    for(int i = 0; i < n + 1; i++){
        for(int j = 0; j < n + 1; j++){
            if(i != j) {
                int p[2];
                if(pipe(p) == 0 && set_nonblocking(p[0]) == 0 && set_nonblocking(p[1]) == 0) {
                    pd.pipes[i][j].read = p[0];
                    pd.pipes[i][j].write = p[1];
                    log_pipe_opening(pipe_log, i, j);
                }else{
                    exit(1);
                }
            }
        }
    }
    fclose(pipe_log);
    return pd;
}

void close_unused_pipes(ProcessDescription * pd){
    local_id n = pd->size;
    local_id ld = pd->ld;
    for(local_id i = 0; i < n; i++){
        for (local_id j = 0; j < n; j++){
            if(i == ld){
                if(close(pd->pipes[i][j].read)) exit(1);
            }else if(j == ld){
                if(close(pd->pipes[i][j].write)) exit(1);
            }else if(i != j){
                if(close(pd->pipes[i][j].read)) exit(1);
                if(close(pd->pipes[i][j].write)) exit(1);
            }
        }
    }
}

void close_used_pipes(ProcessDescription * pd){
    local_id n = pd->size;
    local_id ld = pd->ld;
    for(local_id i = 0; i < n; i++){
        if(i != ld){
            if(close(pd->pipes[ld][i].write)) exit(2);
            if(close(pd->pipes[i][ld].read)) exit(2);
        }
    }
    for(local_id i = 0; i < n; i++){
        free(pd->pipes[i]);
    }
    free(pd->pipes);
}
void child_routine(ProcessDescription pd){
    close_unused_pipes(&pd);
    init_forks(&pd);
    log_process_startup(&pd);
    send_everyone(STARTED, &pd);
    receive_from_everyone(STARTED, &pd);
    log_start_message_receival(&pd);
    do_work(&pd);
    log_process_completion(&pd);
    send_everyone(DONE, &pd);
    receive_all_done(&pd);
    log_completion_message_receival(&pd);
    close_used_pipes(&pd);
    fclose(pd.events_log);
    exit(0);
}

int parent_routine(ProcessDescription pd){
    close_unused_pipes(&pd);
    receive_from_everyone(STARTED, &pd);
    log_start_message_receival(&pd);
    receive_from_everyone(DONE, &pd);
    log_completion_message_receival(&pd);
    int status;
    int exit_code = 0;
    pid_t pid;
    pid = wait(&status);
    while(pid > 0){
        if(status != 0){
            exit_code = 1;
        }
        pid = wait(&status);
    }
    close_used_pipes(&pd);
    fclose(pd.events_log);
    return exit_code;
}

void send_everyone(MessageType msg_type, ProcessDescription * pd){
    MessageHeader header = {.s_magic = MESSAGE_MAGIC, .s_type = msg_type};
    Message msg;
    int len = 0;
    if(msg_type == STARTED){
        len = sprintf(msg.s_payload, log_started_fmt, pd->ld, getpid(), getppid());
    }else if(msg_type == DONE) {
        len = sprintf(msg.s_payload, log_done_fmt, pd->ld);
    }
    header.s_payload_len = len;
    msg.s_header = header;
    if(send_multicast(pd, &msg)){
        printf("multicast error\n");
        exit(1);
    }
}

void receive_from_everyone(MessageType msg_type, ProcessDescription * pd){
    local_id n = pd->size;
    for(local_id i = 1; i < n; i++){
        if(i != pd->ld){
            Message msg;
            while(receive(pd, i, &msg)){}
            if(msg.s_header.s_type != msg_type){
                printf("process %d failed to receive message from %d\n", pd->ld, i);
                printf("wrong message type\n");
                exit(1);
            }
        }
    }
}

void receive_all_done(ProcessDescription * pd){
    Message msg;
    while(pd->done_received < pd->size - 2){
        if(receive_any(pd, &msg) == 0){
            handle_message(pd, pd->last_sender, &msg);
        }
    }
}
