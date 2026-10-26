#include "vector.h"

static VectorClock vector_clock;

static int should_increment(const MessageHeader * header){
    return header->s_type != SNAPSHOT_VTIME && header->s_type != SNAPSHOT_ACK;
}

void init_vector_time(local_id ld){
    vector_clock.ld = ld;
}

timestamp_t get_vector_time(void){
    return vector_clock.time[vector_clock.ld];
}

void copy_vector_time(timestamp_t * dst){
    for(int i = 0; i < MAX_PROCESS_ID; i++){
        dst[i] = vector_clock.time[i];
    }
}

void increment_vector_time(MessageHeader * header){
    if(should_increment(header)){
        vector_clock.time[vector_clock.ld]++;
    }
    for(int i = 0; i < MAX_PROCESS_ID; i++){
        header->s_local_timevector[i] = vector_clock.time[i];
    }
}

void update_vector_time(const MessageHeader * header){
    if(!should_increment(header)){
        return;
    }
    for(int i = 0; i < MAX_PROCESS_ID; i++){
        if(header->s_local_timevector[i] > vector_clock.time[i]){
            vector_clock.time[i] = header->s_local_timevector[i];
        }
    }
    vector_clock.time[vector_clock.ld]++;
}
