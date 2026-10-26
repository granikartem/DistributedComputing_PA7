//
// Created by ubuntu on 10/31/23.
//

#ifndef PA3_CLERK_H
#define PA3_CLERK_H

#include "process.h"
#include "util.h"
#include "banking.h"
#include <string.h>

void clerk_routine(ProcessDescription pd);

int useful_job(ProcessDescription * pd, BalanceHistory * bh);

void handle_transfer(ProcessDescription * pd, Message * msg, BalanceHistory * bh);

void balance_update(BalanceState * state, BalanceHistory * history);

#endif //PA3_CLERK_H
