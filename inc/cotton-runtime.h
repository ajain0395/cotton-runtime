#ifndef __cotton_runtime_h__
#define __cotton_runtime_h__
#endif
#include "cotton.h"

namespace cotton{
    static void make_key();
    void push_task_to_runtime(void *ptr,int index);
    void *worker_routine(void *ptr);
    void *grab_task_from_runtime();
    void lock_deq(int index);
    void unlock_deq(int index);
    void lock_finish();
    void unlock_finish();
    void find_and_execute_task();
    void execute_task(void *task);
    int thread_pool_size();
    int createthread(int index);
    int threadjoin(int index);
    int* getspecific();
    void setspecific(int *value);
    bool isempty(int indexi);
    void freeall();
}
