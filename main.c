#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include "process.h"
#include "util.h"

int main(int argc, char* argv[]) {

    int mutexl;
    local_id n = parse(argc, argv, &mutexl);
    ProcessDescription pd = init_process_description(n, mutexl);
    for(local_id i = 1; i <= n;  i++){
        switch(fork()){
            case 0:
                pd.ld = i;
                ProcessDescription child_pd = {.pipes = pd.pipes, .events_log = pd.events_log, .ld = i, .size = pd.size, .mutexl = pd.mutexl};
                child_routine(child_pd);
            case -1:
                printf("Error spawning child process\n");
                exit(1);
            default:
                break;
        }
    }
    return parent_routine(pd);
}


