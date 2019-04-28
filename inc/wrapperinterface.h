#ifndef __wrapper_interface_h__
#define __wrapper_interface_h__
#endif

#define SUCCESS 0
#define FAILURE 1
#define MAX_POSSIBLE_VALUE 10000000
#define NOT_FOUND -1

namespace wrapperinterface
{

    typedef struct _profiling_return_
    {
        float cpu_time;//store cpu  time
        float gpu_time;//store gpu time
        float l1_miss_rate;//store l3 miss rate
        float llc_miss_rate;//store llc miss rate
        int total_instr_retired;//store total instructions executed
    }profiling_results_t;

    typedef struct _global_deq_ //this deq contains all parallel task that are present in the application
    {
        volatile int N;//Size of global deq
        void **deq; // global deq for all functions

    }global_task_deq_t;

    typedef struct _global_table_
    {
        void *function_type;//method array
        float *alpha;//alpha corresponding to method array
        int size;//size of array
    }global_alpha_table_t;

    void *get_tcg(void *rc,void *rg,void *N, void *alpha);//compute and return tcg for a fiven value of alpha

    void *get_throughput(void *time,void *N);//calculate throughput of CPU/GPU


    float get_l1_miss_rate(void *ptr);//get l1 miss rate

    float get_llc_miss_rate(void *ptr);//get l3 miss rate

    float get_instr_retired(void *ptr);//get total instructions retired

    void *edp(float energy, float time);//objective function energy delay product

    void *edpp(float energy, float time);//objective function energy delay product square

    void *onlineprofiling(void *ptr);//accepts parameter i.e pointer to funtion to be profiled

    void *eas(void *functionptr,void *oclk,int *N,void *table, void *objfun);/*accepts pointer to cpu function,
                                                                               opencl kernel function
                                                                               N number of parallel task
                                                                               global table reference to store value of gpu offload percentage
                                                                               objective function for enery use */

    void *get_gpu_profile_size();//returs the gpu profile size for specific GPU

    int check_alpha(void *ptr,void *table);//check alpha if already exist for a given function

    void *getalpha(void *ptr,void *table);//return alpha if already exist for a given function

    void set_alpha(void *ptr,void *table, float alpha);//update or insert value of alpha in global table

    void characterize_workload(void *ptr);//characterize workload on the basis of profiling results

    void *get_energy();//return the power required by the CPU

    void retired_instruc();//functin to count Retired instructions
    /* 
     * perf_event_open() returns a file descriptor that allows measuring performance information.
     * @param hw_event used to specify hardware event
     * @param group_fd used to allows event groups to be created.
     */
    static long perf_event_open(struct perf_event_attr *hw_event, pid_t pid, int cpu, int group_fd, unsigned long flags);

}
