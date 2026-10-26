//
// Created by ubuntu on 10/31/23.
//

#include "clerk.h"

void clerk_routine(ProcessDescription pd){
    close_unused_pipes(&pd);
    BalanceHistory balance_history = {.s_id = pd.ld, .s_history_len = 0};
    BalanceState start_state = {.s_balance = pd.balance, .s_time = get_lamport_time(), .s_balance_pending_in = 0};
    balance_history.s_history[balance_history.s_history_len] = start_state;
    balance_history.s_history_len++;
    log_process_startup(get_lamport_time(), &pd);
    send_everyone(STARTED, &pd);
    receive_from_everyone(STARTED, &pd);
    log_start_message_receival(get_lamport_time(), &pd);
    int dones_left = pd.size - 2 - useful_job(&pd, &balance_history);
    log_process_completion(get_lamport_time(), &pd);
    send_everyone(DONE, &pd);
    receive_any_from_everyone(DONE, &pd, dones_left);
    log_completion_message_receival(get_lamport_time(), &pd);
    BalanceState end_state = {.s_balance = pd.balance, .s_time = get_lamport_time(), .s_balance_pending_in = 0};
    balance_update(&end_state, &balance_history);
    increment_lamport_time();
    MessageHeader bh_header = {.s_magic = MESSAGE_MAGIC, .s_type = BALANCE_HISTORY, .s_local_time = get_lamport_time(), .s_payload_len = sizeof(balance_history.s_history_len) +
            sizeof(balance_history.s_id) + sizeof(BalanceState) * balance_history.s_history_len};
    Message bh_msg = {.s_header = bh_header};
    memcpy(&bh_msg.s_payload, &balance_history, sizeof(balance_history.s_history_len) +
                                                 sizeof(balance_history.s_id) + sizeof(BalanceState) * balance_history.s_history_len);
    send(&pd, 0, &bh_msg);
    close_used_pipes(&pd);
    fclose(pd.events_log);
    exit(0);
}

int useful_job(ProcessDescription * pd, BalanceHistory * bh) {
    int dones_received = 0;
    while (1) {
        Message msg;
        while (receive_any(pd, &msg)) {
        }
        update_lamport_time(msg.s_header.s_local_time);
        switch (msg.s_header.s_type) {
            case STOP:
                return dones_received;
            case TRANSFER:
                handle_transfer(pd, &msg, bh);
                break;
            case DONE:
                dones_received++;
                break;
            default:
                printf("%d\n", msg.s_header.s_type);
                printf("Message type not supported\n");
                exit(1);
        }
    }
}

void handle_transfer(ProcessDescription * pd, Message * msg, BalanceHistory * bh){
    BalanceState new_state;
    TransferOrder transfer_order;
    memcpy(&transfer_order, msg->s_payload, msg->s_header.s_payload_len);
    if(pd->ld == transfer_order.s_src){
        increment_lamport_time();
        balance_t new_balance = pd->balance - transfer_order.s_amount;
        pd->balance = new_balance;
        new_state.s_balance = new_balance;
        new_state.s_time = get_lamport_time();
        new_state.s_balance_pending_in = transfer_order.s_amount;
        log_transfer_initiation(get_lamport_time(), pd, transfer_order.s_amount, transfer_order.s_dst);
        msg->s_header.s_local_time = get_lamport_time();
        send(pd, transfer_order.s_dst, msg);
    }else if(pd->ld == transfer_order.s_dst){
        balance_t new_balance = pd->balance + transfer_order.s_amount;
        pd->balance = new_balance;
        new_state.s_balance = new_balance;
        new_state.s_time = get_lamport_time();
        new_state.s_balance_pending_in = 0;
        log_transfer_receival(get_lamport_time(), pd, transfer_order.s_amount, transfer_order.s_src);
        increment_lamport_time();
        MessageHeader ack_header = {.s_magic = MESSAGE_MAGIC, .s_type = ACK, .s_payload_len = 0, .s_local_time = get_lamport_time()};
        Message ack_msg = {.s_header = ack_header, .s_payload = ""};
        send(pd, 0, &ack_msg);
    }else{
        printf("Wrong process received transfer message");
        exit(1);
    }
    balance_update(&new_state, bh);
}

void balance_update(BalanceState * state, BalanceHistory * history){
    timestamp_t old_timestamp = history->s_history[history->s_history_len - 1].s_time + 1;
    balance_t old_balance = history->s_history[history->s_history_len - 1].s_balance;
    for(timestamp_t t = old_timestamp; t < state->s_time; t++){
        BalanceState inter_state = {.s_time = t, .s_balance = old_balance, .s_balance_pending_in = 0};
        history->s_history[history->s_history_len] = inter_state;
        history->s_history_len++;
    }
    history->s_history[history->s_history_len] = * state;
    history->s_history_len++;
}

