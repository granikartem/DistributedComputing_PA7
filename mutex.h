#ifndef PA1_MUTEX_H
#define PA1_MUTEX_H

#include "process.h"

void init_forks(ProcessDescription * pd);

void handle_message(ProcessDescription * pd, local_id from, const Message * msg);

#endif //PA1_MUTEX_H
