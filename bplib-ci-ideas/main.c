#include "waitqueue.h"

#include <stdio.h>
#include <stdlib.h>

int main(int argc, char** argv)
{
    BPLib_WaitQueue_t q;
    int storage[5];

    if (!BPLib_CI_WaitQueueInit(&q, (void*)(storage), sizeof(int), 5))
    {
        printf("Queue didn't init\n");
        return -1;
    }

    int val = 4;
    int ret_val = -1;
    if (!BPLib_CI_WaitQueueTryPush(&q, &val, 0))
    {
        printf("Bad push\n");
    }

    if (!BPLib_CI_WaitQueueTryPull(&q, &ret_val, 0))
    {
        printf("Bad pull\n");
    }
    printf("Pulled %d\n", ret_val);


    return 0;
}
