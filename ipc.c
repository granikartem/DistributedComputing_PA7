#define _DEFAULT_SOURCE
#include <errno.h>
#include "ipc.h"
#include "process.h"
#include "vector.h"


/** Send a message to the process specified by id.
 *
 * Pipes are non-blocking: if the pipe is full, write() is retried.
 *
 * @param self    Any data structure implemented by students to perform I/O
 * @param dst     ID of recepient
 * @param msg     Message to send
 *
 * @return 0 on success, any non-zero value on error
 */
int send(void * self, local_id dst, const Message * msg){
    ProcessDescription * pd = (ProcessDescription *) self;
    Message message = *msg;
    increment_vector_time(&message.s_header);
    size_t len = sizeof(MessageHeader) + message.s_header.s_payload_len;
    ssize_t written;
    while((written = write(pd->pipes[pd->ld][dst].write, &message, len)) < 0){
        if(errno != EAGAIN) return 1;
    }
    if((size_t) written != len) return 1;
    return 0;
}

//------------------------------------------------------------------------------

/** Send multicast message.
 *
 * Send msg to all other processes including parrent.
 * Should stop on the first error.
 *
 * @param self    Any data structure implemented by students to perform I/O
 * @param msg     Message to multicast.
 *
 * @return 0 on success, any non-zero value on error
 */
int send_multicast(void * self, const Message * msg){
    ProcessDescription * pd = (ProcessDescription *) self;
    for(local_id i = 0; i < pd->size; i++){
        if(i != pd->ld) {
            if(send(pd, i, msg) != 0) return 1;
        }
    }
    return 0;
}

//------------------------------------------------------------------------------

/** Receive a message from the process specified by id.
 *
 * Never blocks: the header is read first, the payload is read only after it.
 *
 * @param self    Any data structure implemented by students to perform I/O
 * @param from    ID of the process to receive message from
 * @param msg     Message structure allocated by the caller
 *
 * @return 0 on success, non-zero if there is no message in the pipe yet
 */
int receive(void * self, local_id from, Message * msg){
    ProcessDescription * pd = (ProcessDescription *) self;
    int fd = pd->pipes[from][pd->ld].read;
    if(read(fd, &msg->s_header, sizeof(MessageHeader)) != sizeof(MessageHeader)) return 1;
    size_t len = msg->s_header.s_payload_len;
    size_t received = 0;
    while(received < len){
        ssize_t n = read(fd, msg->s_payload + received, len - received);
        if(n > 0){
            received += n;
        }else if(n == 0 || errno != EAGAIN){
            return 1;
        }
    }
    update_vector_time(&msg->s_header);
    return 0;
}

//------------------------------------------------------------------------------

/** Receive a message from any process.
 *
 * Polls every incoming pipe once. The sender id is saved to last_sender.
 * If all pipes are empty, sleeps a little so that waiting processes
 * don't burn CPU needed by the process in the critical section.
 *
 * @param self    Any data structure implemented by students to perform I/O
 * @param msg     Message structure allocated by the caller
 *
 * @return 0 on success, non-zero if there are no messages at the moment
 */
int receive_any(void * self, Message * msg){
    ProcessDescription * pd = (ProcessDescription *) self;
    for(local_id i = 0; i < pd->size; i++){
        if(i != pd->ld && receive(pd, i, msg) == 0){
            pd->last_sender = i;
            return 0;
        }
    }
    usleep(200);
    return 1;
}
