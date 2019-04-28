////volatile front rear 
///push remove lock
#include "cotton-runtime.h"
#include <cstring>
#include <iostream>
#include <pthread.h>
#include <stdlib.h>

#define DEQ_SIZE 10000

namespace cotton{

    typedef struct _thread_key_data_
    {
        int index;
    }thread_data_t;


    typedef struct _thread_deq_
    {
       volatile int front;
        void **deq;
       volatile int rear;
    }thread_deq_t;


    /*
       init_runtime();
       start_finish();
       async (S1);
       S2;
       end_finish();
       finalize_runtime();
     */

    pthread_t *threads; //number of threads
    pthread_mutex_t mutexfinish,*mutexlocks; //locks for each task pool bucket
    thread_data_t *deqindex ; // index of each thread for worker routine key deq mapping
    thread_deq_t *threads_queues ;//threads deques
    pthread_key_t key;//keys for for all thread specific index fetching
    static pthread_once_t key_once = PTHREAD_ONCE_INIT;
    volatile int finish_counter = 0;//counter for task remaining in queues
    volatile bool shutdown = false; // flag for worker routines polling enable/disable

    static void make_key()
    {
        (void) pthread_key_create(&key, NULL);
    }
    /*
       insert into deq:
       insert task to task pool deq of thread
       */
    void insert_into_deq(int index, void *value)
    {
       // lock_deq(index);
        if ((threads_queues[index].rear == DEQ_SIZE-1)) 
        { 
            printf("\nQueue is Full"); 
	    freeall();
	    abort();
            return; 
        } 

        else if (threads_queues[index].front == -1) /* Insert First Element */
        {
            threads_queues[index].deq[0] = value;
            threads_queues[index].front = threads_queues[index].rear = 0;
        } 
/*
        else if (threads_queues[index].rear == DEQ_SIZE-1 && threads_queues[index].front != 0) 
        { 
            threads_queues[index].rear = 0; 
            threads_queues[index].deq[threads_queues[index].rear] = value;
        } 
*/
        else
        { 
            threads_queues[index].deq[threads_queues[index].rear + 1] = value;
            threads_queues[index].rear++; 
        }
       // unlock_deq(index);
    }
    /*
       steal from deq:
       steal from the deq of other thread if calling thread's deq is empty
       */
    void *steal_from_deq(int index)
    {
        lock_deq(index);
        if (isempty(index)) 
        { 
            printf("\nQueue of thread[%d] is Empty", index);
            unlock_deq(index);
            return NULL; 
        }

        void *data = threads_queues[index].deq[threads_queues[index].front]; 
        threads_queues[index].deq[threads_queues[index].front] = NULL; 
    /*    if (threads_queues[index].front == threads_queues[index].rear) 
        { 
            threads_queues[index].front = -1; 
            threads_queues[index].rear = -1; 
        } 
        else if (threads_queues[index].front == DEQ_SIZE-1) 
            threads_queues[index].front = 0; 
        else*/

        threads_queues[index].front++; 

        unlock_deq(index);
        return data; 
    }
    /*
       pop from deq:
       pop a task from deq of calling thread
       */
    void *pop_from_deq(int index)
    {
        lock_deq(index);//lock thread deq from access
        if (isempty(index))
        { 
            printf("\nQueue of thread[%d] is Empty", index);//if deq is empty message
            unlock_deq(index);
            return NULL; 
        } 

        void *data = threads_queues[index].deq[threads_queues[index].rear]; 
        threads_queues[index].deq[threads_queues[index].rear] = NULL; 
        if (threads_queues[index].front == threads_queues[index].rear)
        { 
            threads_queues[index].front = -1;
            threads_queues[index].rear = -1; 
        } 
        /*
        else if (threads_queues[index].rear == 0) 
            threads_queues[index].rear = DEQ_SIZE - 1; 
        */
        else
            threads_queues[index].rear--;

        unlock_deq(index);
        return data; 

    }

    /*
       init runtime:
       Initialize runtime and associated resources
     */
    void init_runtime()
    {
        int size = thread_pool_size();
        threads = new pthread_t[size - 1];
        mutexlocks = new pthread_mutex_t[size];
        deqindex = new thread_data_t[size];
        threads_queues = new thread_deq_t[size]; // allocating memory to queues for threads including main thread

        (void) pthread_once(&key_once, make_key);

        for(int i = 0; i<size ;i++)
        {
            pthread_mutex_init(&mutexlocks[i], NULL);
            deqindex[i].index = i;
            threads_queues[i].deq = (void**)malloc(sizeof(void*) * DEQ_SIZE); //creating deq for each thread
            threads_queues[i].front = -1;
            threads_queues[i].rear = -1;
        }
        for(int i = 0; i<size - 1 ;i++)
        {
            createthread(i);//create thread and provide task
            //create thread and pass it to worker routine
        }
        if (getspecific() == NULL)
        {
            int *val = (int*)malloc(sizeof(int));
            *val = size -1;
            setspecific(val);//set thread specific data to key
        }
    }

    /*
       create thread:
       create thread and assign it its routine
       */
    int createthread(int index)
    {
        return pthread_create(&threads[index],
                NULL,
                worker_routine,
                &deqindex[index]
                );
    }

    int threadjoin(int index)
    {
          return pthread_join(threads[index],NULL);
    }

    int* getspecific()
    {
        return (int*)pthread_getspecific(key);
    }

    void setspecific(int *value)
    {
        (void) pthread_setspecific(key, value);
    }

    void start_finish()
    {
        finish_counter = 0;//reset
    }

    void async(std::function<void()> &&lambda) //accepts a C++11 lambda function
    {
        int *getvalue = getspecific();
        int index = *getvalue;
        lock_finish();
        finish_counter++;//concurrent access
        unlock_finish();
        //task size retrieval
        int task_size = sizeof(lambda);
        //copy	task	on	heap
        void *p =(void*)malloc(task_size);
        memcpy(p,&lambda,task_size);
        //thread-safe	push_task_to_runtime
        //index of shelf
        push_task_to_runtime(p,index);
        return;
    }

    void push_task_to_runtime(void *ptr,int index)
    {
        //push task
        insert_into_deq(index, ptr);
    }

    void end_finish()
    {
        while(finish_counter !=	0)
        {
            find_and_execute_task();
        }
    }

    /*
       worker routine:
       task to be performed by every thread
       */
    void *worker_routine(void *indexdata)
    {

        thread_data_t *data = (thread_data_t*)indexdata;
        if (getspecific() == NULL) 
        {
            int *val;
            val = (int*)malloc(sizeof(int));
            *val = data->index;
            setspecific(val);
        }
        while(!shutdown)
        {
            find_and_execute_task();
        }
        if(getspecific() != NULL)
        {
            free(getspecific());
            setspecific(NULL);
        }
    }

    /*
        isempty:
        check if deq is empty or not
       */
    bool isempty(int indexi)
    {
        if(threads_queues[indexi].front == -1 || threads_queues[indexi].front > threads_queues[indexi].rear)
        {
            return true;
        }
        return false;
    }

    void *grab_task_from_runtime() //grab task from thread deque or steal from victims deque
    {
        int *getvalue = getspecific();
        if(getvalue != NULL)
        {
            int indexi = *getvalue;
            if(!isempty(indexi))
            {
                return pop_from_deq(indexi);
            }
            else 
            {
                int randomindex;
                do
                {
                    randomindex = rand() % thread_pool_size();
                }while(randomindex == indexi);
                if(!isempty(randomindex))
                {
                    return steal_from_deq(randomindex);
                }
                return NULL;
            }
        }
        return NULL;
    }

    void lock_deq(int index)
    {
        pthread_mutex_lock(&mutexlocks[index]);
    }

    void unlock_deq(int index)
    {
        pthread_mutex_unlock(&mutexlocks[index]);
    }
    //lock aquire
    void lock_finish()
    {
        pthread_mutex_lock(&mutexfinish);
    }

    //unlock
    void unlock_finish()
    {
        pthread_mutex_unlock(&mutexfinish);
    }

    /*
       find and execute task:
       find task in deq of thread if deq is empty steal from deq of other thread
       */
    void find_and_execute_task()
    {
        //grab_from_runtime	is	thread-safe
        void *task;
        task=grab_task_from_runtime();
        if(task	!=	NULL)
        {
            execute_task(task);
            free(task);
            task = NULL;
            lock_finish();
            finish_counter--;
            unlock_finish();
        }
    }

    void execute_task(void *task)
    {
        (*(std::function<void()> *)task) ();

        //   *((sigrout_t*) task) ();
        //  task();
    }

    int thread_pool_size()
    {
        int envsize =atoi(std::getenv("COTTON_WORKERS"));
        return (envsize>1)?envsize:1;
    }
    void freeall()
    {
        int size=thread_pool_size();
        for (int i = 0; i < size; i++)
        {
            for(int j = 0; j < DEQ_SIZE;j++)
            {
                if(threads_queues[i].deq[j] != NULL)
                {
                    free(threads_queues[i].deq[j]);
                    threads_queues[i].deq[j] = NULL;
                }
            }
            if(threads_queues[i].deq != NULL)
            {
                free(threads_queues[i].deq);
                threads_queues[i].deq = NULL;
            }
        }
        delete threads;
        delete mutexlocks;
        delete deqindex;
        delete threads_queues;
    }

    /*
    Finalize runtime:
    for releasing all the resources aquired by all the threads and wait for threads to finish their routine
       */
    void finalize_runtime()
    {
        //all	spinning	workers
        //will	exit	worker_routine
        shutdown=true;
        int	size=thread_pool_size();
        //	master	waits	for	helpers	to	join
        for(int i= 0 ;	i<size - 1;	i++)
        {
            threadjoin(i);
        }
	freeall();
    }
}
