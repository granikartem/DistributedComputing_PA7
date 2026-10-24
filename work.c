#include "work.h"
#include "pa2345.h"

void do_work(ProcessDescription * pd){
    int iterations = pd->ld * 5;
    char buf[MAX_PAYLOAD_LEN];
    for(int i = 1; i <= iterations; i++){
        snprintf(buf, sizeof(buf), log_loop_operation_fmt, pd->ld, i, iterations);
        if(pd->mutexl) request_cs(pd);
        print(buf);
        if(pd->mutexl) release_cs(pd);
    }
}
