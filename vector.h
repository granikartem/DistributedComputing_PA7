#ifndef PA7_VECTOR_H
#define PA7_VECTOR_H

#include "banking.h"

typedef struct {
    local_id ld;
    timestamp_t time[MAX_PROCESS_ID];
} VectorClock;

// get_vector_time() is declared in banking.h and returns own component of the vector

void init_vector_time(local_id ld);

void copy_vector_time(timestamp_t * dst);

// SNAPSHOT_VTIME and SNAPSHOT_ACK are not events: they carry the vector but don't change it

void increment_vector_time(MessageHeader * header);

void update_vector_time(const MessageHeader * header);

#endif //PA7_VECTOR_H
