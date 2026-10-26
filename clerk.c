//
// Created by ubuntu on 10/31/23.
//

#include "clerk.h"

void clerk_routine(ProcessDescription pd){
    close_unused_pipes(&pd);
    log_process_startup(get_lamport_time(), &pd);
    send_everyone(STARTED, &pd);
    receive_from_everyone(STARTED, &pd);
    log_start_message_receival(get_lamport_time(), &pd);
    int dones_left = pd.size - 2 - useful_job(&pd);
    log_process_completion(get_lamport_time(), &pd);
    send_everyone(DONE, &pd);
    receive_any_from_everyone(DONE, &pd, dones_left);
    log_completion_message_receival(get_lamport_time(), &pd);
    close_used_pipes(&pd);
    fclose(pd.events_log);
    exit(0);
}

int useful_job(ProcessDescription * pd) {
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
                handle_transfer(pd, &msg);
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

void handle_transfer(ProcessDescription * pd, Message * msg){
    TransferOrder transfer_order;
    memcpy(&transfer_order, msg->s_payload, msg->s_header.s_payload_len);
    if(pd->ld == transfer_order.s_src){
        increment_lamport_time();
        pd->balance -= transfer_order.s_amount;
        log_transfer_initiation(get_lamport_time(), pd, transfer_order.s_amount, transfer_order.s_dst);
        msg->s_header.s_local_time = get_lamport_time();
        send(pd, transfer_order.s_dst, msg);
    }else if(pd->ld == transfer_order.s_dst){
        pd->balance += transfer_order.s_amount;
        log_transfer_receival(get_lamport_time(), pd, transfer_order.s_amount, transfer_order.s_src);
        increment_lamport_time();
        MessageHeader ack_header = {.s_magic = MESSAGE_MAGIC, .s_type = ACK, .s_payload_len = 0, .s_local_time = get_lamport_time()};
        Message ack_msg = {.s_header = ack_header, .s_payload = ""};
        send(pd, 0, &ack_msg);
    }else{
        printf("Wrong process received transfer message");
        exit(1);
    }
}
