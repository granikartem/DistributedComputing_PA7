//
// Created by ubuntu on 10/31/23.
//

#include "client.h"

int client_routine(ProcessDescription pd){
    pd.ld = 0;
    init_vector_time(pd.ld);
    close_unused_pipes(&pd);
    receive_from_everyone(STARTED, &pd);
    log_start_message_receival(get_vector_time(), &pd);
    bank_robbery(&pd, pd.size - 1);
    send_everyone(STOP, &pd);
    receive_from_everyone(DONE, &pd);
    log_completion_message_receival(get_vector_time(), &pd);
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

void transfer(void * parent_data, local_id src, local_id dst, balance_t amount){
    ProcessDescription * pd = (ProcessDescription *) parent_data;
    TransferOrder transfer_order = {.s_src = src, .s_dst = dst, .s_amount = amount};
    MessageHeader start_msg_header = {.s_magic = MESSAGE_MAGIC, .s_type = TRANSFER, .s_payload_len = sizeof(TransferOrder)};
    Message start_msg = {.s_header = start_msg_header};
    memcpy(start_msg.s_payload, &transfer_order, sizeof(TransferOrder));
    send(pd, src, &start_msg);
    Message ack_msg;
    while(receive(pd, dst, &ack_msg)){
    }
    if(ack_msg.s_header.s_type != ACK){
        printf("Not ACK received!\n");
        exit(2);
    }
}

void total_sum_snapshot(void){
}
