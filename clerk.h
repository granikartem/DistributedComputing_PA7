//
// Created by ubuntu on 10/31/23.
//

#ifndef PA7_CLERK_H
#define PA7_CLERK_H

#include "process.h"
#include "util.h"
#include "banking.h"
#include <string.h>

void clerk_routine(ProcessDescription pd);

int useful_job(ProcessDescription * pd);

void handle_transfer(ProcessDescription * pd, Message * msg);

#endif //PA7_CLERK_H
