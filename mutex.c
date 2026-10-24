#include <stdlib.h>
#include "mutex.h"
#include "pa2345.h"

static void send_empty(ProcessDescription * pd, local_id dst, MessageType msg_type){
    MessageHeader header = {.s_magic = MESSAGE_MAGIC, .s_type = msg_type, .s_payload_len = 0};
    Message msg;
    msg.s_header = header;
    if(send(pd, dst, &msg)){
        printf("process %d failed to send message to %d\n", pd->ld, dst);
        exit(1);
    }
}

static int has_all_forks(const ProcessDescription * pd){
    for(local_id i = 1; i < pd->size; i++){
        if(i != pd->ld && !pd->forks[i].has_fork) return 0;
    }
    return 1;
}

static void give_fork(ProcessDescription * pd, local_id to){
    pd->forks[to].has_fork = 0;
    send_empty(pd, to, CS_REPLY);
    if(pd->state == HUNGRY){
        pd->forks[to].has_request = 0;
        send_empty(pd, to, CS_REQUEST);
    }
}

void init_forks(ProcessDescription * pd){
    pd->state = THINKING;
    pd->done_received = 0;
    for(local_id i = 1; i < pd->size; i++){
        pd->forks[i].has_fork = pd->ld < i;
        pd->forks[i].dirty = 1;
        pd->forks[i].has_request = pd->ld > i;
    }
}

void handle_message(ProcessDescription * pd, local_id from, const Message * msg){
    Fork * fork = &pd->forks[from];
    switch(msg->s_header.s_type){
        case CS_REQUEST:
            fork->has_request = 1;
            if(fork->has_fork && (pd->state == THINKING || (pd->state == HUNGRY && fork->dirty))){
                give_fork(pd, from);
            }
            break;
        case CS_REPLY:
            fork->has_fork = 1;
            fork->dirty = 0;
            break;
        case DONE:
            pd->done_received++;
            break;
        default:
            break;
    }
}

int request_cs(const void * self){
    ProcessDescription * pd = (ProcessDescription *) self;
    pd->state = HUNGRY;
    for(local_id i = 1; i < pd->size; i++){
        if(i != pd->ld && !pd->forks[i].has_fork && pd->forks[i].has_request){
            pd->forks[i].has_request = 0;
            send_empty(pd, i, CS_REQUEST);
        }
    }
    Message msg;
    do{
        while(receive_any(pd, &msg) == 0){
            handle_message(pd, pd->last_sender, &msg);
        }
    }while(!has_all_forks(pd));
    pd->state = EATING;
    return 0;
}

int release_cs(const void * self){
    ProcessDescription * pd = (ProcessDescription *) self;
    pd->state = THINKING;
    for(local_id i = 1; i < pd->size; i++){
        if(i == pd->ld) continue;
        pd->forks[i].dirty = 1;
        if(pd->forks[i].has_fork && pd->forks[i].has_request){
            give_fork(pd, i);
        }
    }
    return 0;
}
