//
// Created by ubuntu on 10/31/23.
//

#include "clerk.h"

void clerk_routine(ProcessDescription pd){
    init_vector_time(pd.ld);
    close_unused_pipes(&pd);
    log_process_startup(get_vector_time(), &pd);
    send_everyone(STARTED, &pd);
    receive_from_everyone(STARTED, &pd);
    log_start_message_receival(get_vector_time(), &pd);
    int dones_left = pd.size - 2 - useful_job(&pd);
    log_process_completion(get_vector_time(), &pd);
    send_everyone(DONE, &pd);
    receive_dones(&pd, dones_left);
    log_completion_message_receival(get_vector_time(), &pd);
    close_used_pipes(&pd);
    fclose(pd.events_log);
    exit(0);
}

int useful_job(ProcessDescription * pd) {
    int dones_received = 0;
    BalanceSnapshot snapshot = {.active = 0};
    while (1) {
        Message msg;
        while (receive_any(pd, &msg)) {
        }
        track_snapshot(pd, &snapshot, &msg);
        switch (msg.s_header.s_type) {
            case STOP:
                return dones_received;
            case TRANSFER:
                handle_transfer(pd, &msg);
                break;
            case DONE:
                dones_received++;
                break;
            case SNAPSHOT_VTIME:
                handle_snapshot_vtime(pd, &snapshot, &msg);
                break;
            case EMPTY:
                break;
            default:
                printf("%d\n", msg.s_header.s_type);
                printf("Message type not supported\n");
                exit(1);
        }
    }
}

void receive_dones(ProcessDescription * pd, int dones_left){
    while (dones_left > 0) {
        Message msg;
        while (receive_any(pd, &msg)) {
        }
        switch (msg.s_header.s_type) {
            case DONE:
                dones_left--;
                break;
            case TRANSFER:
                handle_transfer(pd, &msg);
                break;
            default:
                printf("Message of type %d lost\n", msg.s_header.s_type);
                exit(1);
        }
    }
}

void handle_transfer(ProcessDescription * pd, Message * msg){
    TransferOrder transfer_order;
    memcpy(&transfer_order, msg->s_payload, msg->s_header.s_payload_len);
    if(pd->ld == transfer_order.s_src){
        pd->balance -= transfer_order.s_amount;
        send(pd, transfer_order.s_dst, msg);
        log_transfer_initiation(get_vector_time(), pd, transfer_order.s_amount, transfer_order.s_dst);
    }else if(pd->ld == transfer_order.s_dst){
        pd->balance += transfer_order.s_amount;
        log_transfer_receival(get_vector_time(), pd, transfer_order.s_amount, transfer_order.s_src);
        MessageHeader ack_header = {.s_magic = MESSAGE_MAGIC, .s_type = ACK, .s_payload_len = 0};
        Message ack_msg = {.s_header = ack_header, .s_payload = ""};
        send(pd, 0, &ack_msg);
    }else{
        printf("Wrong process received transfer message");
        exit(1);
    }
}

void handle_snapshot_vtime(ProcessDescription * pd, BalanceSnapshot * snapshot, Message * msg){
    BalanceSnapshot initial = {.active = 1, .open_channels = pd->size - 2};
    memcpy(initial.time, msg->s_payload, msg->s_header.s_payload_len);
    *snapshot = initial;
    MessageHeader ack_header = {.s_magic = MESSAGE_MAGIC, .s_type = SNAPSHOT_ACK, .s_payload_len = 0};
    Message ack_msg = {.s_header = ack_header, .s_payload = ""};
    send(pd, PARENT_ID, &ack_msg);
}


void track_snapshot(ProcessDescription * pd, BalanceSnapshot * snapshot, Message * msg){
    if(!snapshot->active) return;
    int after_t = msg->s_header.s_local_timevector[PARENT_ID] >= snapshot->time[PARENT_ID];
    if(after_t && !snapshot->recorded){
        snapshot->recorded = 1;
        snapshot->balance = pd->balance;
        MessageHeader empty_header = {.s_magic = MESSAGE_MAGIC, .s_type = EMPTY, .s_payload_len = 0};
        Message empty_msg = {.s_header = empty_header, .s_payload = ""};
        for(local_id i = 1; i < pd->size; i++){
            if(i != pd->ld) send(pd, i, &empty_msg);
        }
    }
    local_id from = pd->last_sender;
    if(snapshot->recorded && from != PARENT_ID && !snapshot->closed[from]){
        if(after_t){
            snapshot->closed[from] = 1;
            snapshot->open_channels--;
        }else if(msg->s_header.s_type == TRANSFER){
            TransferOrder transfer_order;
            memcpy(&transfer_order, msg->s_payload, msg->s_header.s_payload_len);
            snapshot->pending += transfer_order.s_amount;
        }
    }
    if(snapshot->recorded && snapshot->open_channels == 0){
        BalanceState state = {.s_balance = snapshot->balance, .s_balance_pending_in = snapshot->pending};
        memcpy(state.s_timevector, snapshot->time, sizeof(state.s_timevector));
        MessageHeader state_header = {.s_magic = MESSAGE_MAGIC, .s_type = BALANCE_STATE, .s_payload_len = sizeof(BalanceState)};
        Message state_msg = {.s_header = state_header};
        memcpy(state_msg.s_payload, &state, sizeof(BalanceState));
        send(pd, PARENT_ID, &state_msg);
        snapshot->active = 0;
    }
}
