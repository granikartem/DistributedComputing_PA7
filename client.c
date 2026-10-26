//
// Created by ubuntu on 10/31/23.
//

#include "client.h"

void update_all_history(AllHistory * history, ProcessDescription * pd);

int client_routine(ProcessDescription pd){
    pd.ld = 0;
    pd.type = CLIENT;
    pd.head = NULL;
    pd.tail = NULL;
    close_unused_pipes(&pd);
    AllHistory all_history = {.s_history_len = pd.size - 1};
    receive_from_everyone(STARTED, &pd);
    log_start_message_receival(get_lamport_time(), &pd);
    bank_robbery(&pd, pd.size - 1);
    send_everyone(STOP, &pd);
    receive_from_everyone(DONE, &pd);
    log_completion_message_receival(get_lamport_time(), &pd);
    for (int i = 1; i < pd.size; ++i) {
        Message msg;
        while(receive(&pd, i, &msg)){
        }
        if(msg.s_header.s_type == BALANCE_HISTORY){
            memcpy(&all_history.s_history[i - 1], msg.s_payload, msg.s_header.s_payload_len);
            update_lamport_time(msg.s_header.s_local_time);
        }else{
            printf("%d\n", msg.s_header.s_type);
            printf("Not BALANCE HISTORY RECEIVED\n");
            exit(1);
        }
    }
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
    update_all_history(&all_history, &pd);
    print_history(&all_history);
    return exit_code;
}

void transfer(void * parent_data, local_id src, local_id dst, balance_t amount){
    ProcessDescription * pd = (ProcessDescription *) parent_data;
    TransferOrder transfer_order = {.s_src = src, .s_dst = dst, .s_amount = amount};
    increment_lamport_time();
    if(amount != 0) {
        struct TransferList * new_node = malloc(sizeof(struct TransferList));
        new_node->s_src = src;
        new_node->s_dst = dst;
        new_node->s_amount = amount;
        new_node->start_time = get_lamport_time();
        new_node->next = NULL;
        if (pd->head == NULL) {
            pd->head = new_node;
        } else {
            pd->tail->next = new_node;
        }
        pd->tail = new_node;
    }
    MessageHeader start_msg_header = {.s_magic = MESSAGE_MAGIC, .s_type = TRANSFER, .s_payload_len = sizeof(TransferOrder), .s_local_time = get_lamport_time()};
    Message start_msg = {.s_header = start_msg_header};
    memcpy(start_msg.s_payload, &transfer_order, sizeof(TransferOrder));
    send(pd, src, &start_msg);
    Message ack_msg;
    while(receive(pd, dst, &ack_msg)){
    }
    if(ack_msg.s_header.s_type != ACK){
        printf("Not ACK received!\n");
        exit(2);
    }else{
        update_lamport_time(ack_msg.s_header.s_local_time);
    }
}

void update_all_history(AllHistory * history, ProcessDescription * pd){
    struct TransferList * curr = pd->head;
    while(curr != NULL){
        balance_t amount = curr->s_amount;
        timestamp_t time = curr->start_time;
        BalanceHistory src_history = history->s_history[curr->s_src - 1];
        BalanceHistory dst_history = history->s_history[curr->s_dst - 1];
        balance_t pending = src_history.s_history[time].s_balance_pending_in;
        while(src_history.s_history[time].s_balance_pending_in - pending != amount){
            pending = src_history.s_history[time].s_balance_pending_in;
            time++;
        }
        balance_t old_balance = dst_history.s_history[time].s_balance;
        time++;
        balance_t new_balance = dst_history.s_history[time].s_balance;
        while(new_balance - old_balance != amount){
            old_balance = new_balance;
            balance_t increase = src_history.s_history[time].s_balance_pending_in + amount;
            src_history.s_history[time].s_balance_pending_in = increase;
            time++;
            new_balance = dst_history.s_history[time].s_balance;
        }
        history->s_history[curr->s_src - 1] = src_history;
        history->s_history[curr->s_dst - 1] = dst_history;
        curr = curr->next;
    }
}
