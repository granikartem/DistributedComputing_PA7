#include "banking.h"
#include "util.h"
#include "clerk.h"
#include "client.h"

int main(int argc, char * argv[]){
    local_id n;
    balance_t * balances = parse(&n, argc, argv);
    ProcessDescription pd = init_process_description(n);
    for(local_id i = 1; i <= n;  i++){
        switch(fork()){
            case 0:
                pd.ld = i;
                ProcessDescription clerk_pd = {.pipes = pd.pipes, .events_log = pd.events_log, .ld = i, .size = pd.size, .type = CLERK, .balance = balances[i - 1]};
                clerk_routine(clerk_pd);
            case -1:
                printf("Error spawning child process\n");
                exit(1);
            default:
                break;
        }
    }
    return client_routine(pd);
}
