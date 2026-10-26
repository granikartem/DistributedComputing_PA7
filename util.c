#include "util.h"

balance_t* parse(local_id * n, int argc, char* argv[]){
    int a = getopt(argc, argv, "p:");
    if(a == 'p'){
        *n = atoi(optarg);
        balance_t* balances = malloc(sizeof(balance_t) * *n);
        for (int i = 0; i < *n; ++i) {
            balances[i] = atoi(argv[3 + i]);
        }
        return balances;
    }else{
        exit(EXIT_FAILURE);
    }

}

ProcessDescription init_process_description(local_id n){
    ProcessDescription pd;
    pd.ld = 0;
    pd.size = n + 1;
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
                if(pipe(p) == 0) {
                    int flags = fcntl(p[0], F_GETFL);
                    fcntl(p[0], F_SETFL, flags | O_NONBLOCK);
                    flags = fcntl(p[1], F_GETFL);
                    fcntl(p[1], F_SETFL, flags | O_NONBLOCK);
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




void send_everyone(MessageType msg_type, ProcessDescription * pd){
    MessageHeader header = {.s_magic = MESSAGE_MAGIC, .s_type = msg_type};
    Message msg;
    int len = 0;
    switch(msg_type) {
        case STARTED:
            len = sprintf(msg.s_payload, log_started_fmt, get_vector_time(), pd->ld, getpid(), getppid(),
                          pd->balance);
            break;
        case DONE:
            len = sprintf(msg.s_payload, log_done_fmt, get_vector_time(), pd->ld, pd->balance);
            break;
        case STOP:
            len = sprintf(msg.s_payload, stop_msg_fmt, get_vector_time(), pd->ld);
            break;
        default:
            printf("unsupported message type");
            exit(1);
    }
    header.s_payload_len = len;
    msg.s_header = header;
    if(send_multicast(pd, &msg)){
        printf("multicast error\n");
        exit(1);
    }
}

void receive_any_from_everyone(MessageType msg_type, ProcessDescription * pd, local_id amount){
    while(amount > 0) {
        Message msg;
        if(!receive_any(pd, &msg)) {
            if(msg.s_header.s_type == msg_type){
                amount--;
            }else{
                printf("Message of type %d lost\n", msg.s_header.s_type);
                exit(1);
            }
        }
    }
}

void receive_from_everyone(MessageType msg_type, ProcessDescription * pd){
    local_id n = pd-> size;
    for(int i = 1; i < n; i++){
        if(i != pd->ld){
            Message msg;
            //int c = 0;
            while(receive(pd, i, &msg)){
                //c++
            }
            if(msg.s_header.s_type != msg_type){
                printf("wrong message received\n");
                exit(1);
            }
        }
    }
}
