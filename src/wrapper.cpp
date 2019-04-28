//References
//https://www.systutorials.com/docs/linux/man/2-perf_event_open/#lbAL
#include "wrapperinterface.h"
#include "cotton.h"

namespace wrapperinterface
{
    //after evaluating the value of alpha
    /* we will assign (1 - alpha)*Nrem to CPU that will be distributed among other threads using work-stealing implemented in previous assignment and 
       the proxy thread will offload (aplha) * Nrem task to GPU according to gpu_profile_size in each iteration and wait for both CPU and GPU to finish task
       task will be distributed from global_task_deq
     */

    int edp(float energy, float time)//objective function energy delay product
    {
        return energy * time;
    }
    int edpp(float energy, float time)//objective function energy delay product square
    {
        return energy * time * time;
    }
    void *onlineprofiling(void *ptr)//accepts parameter i.e pointer to funtion to be profiled
    {
        profiling_results_t *results = NULL;
        results = new profiling_results_t;
        results->cpu_time = 0;//store cpu  time
        results->gpu_time = 0;//store gpu time
        results->l1_miss_rate = 0;//store l3 miss rate
        results->llc_miss_rate = 0;//store llc miss rate
        results->total_instr_retired = 0;//store total instructions executed
        return results;
    }
    float get_l1_miss_rate(void *ptr)//get l1 miss rate
    {
        return 0;
    }
    float get_llc_miss_rate(void *ptr)//get l3 miss rate
    {
        return 0;
    }
    float get_instr_retired(void *ptr)//get total instructions retired
    {
        return 0;
    }
    void *get_tcg(void *rc,void *rg,void *N, void *alpha)//compute and return tcg for a fiven value of alpha
    {
        return NULL;
    }

    void *get_throughput(void *time,void *N)//calculate throughput of CPU/GPU
    {
        return NULL;
    }
    void *get_rg(void *time,void *N)//calculate throughput of GPU
    {
        return NULL;
    }
    /*accepts pointer to cpu function,
      opencl kernel function
      N number of parallel task
      global table reference to store value of gpu offload percentage
      objective function for enery use */
    void *eas(void *functionptr,void *oclk,int *N,void *table, void *objfun)
    {
        float *result =getalpha(functionptr,table);
        float alpha = 0.0;
        int Nrem = N;
        int shared_counter = 0;
        if(alpha != NULL)
        {
            alpha = *result;
            Nrem = N;
        }
        else
        {
            if( N < get_gpu_profile_size())
            {
                alpha = 0;
                set_alpha(functionptr,table,alpha);
                return;
            }
            shared_counter = N;
            Nrem = N;
            while(Nrem > N/2)
            {
                profiling_results_t *profiling_result =(profiling_results_t*)onlineprofiling(functionptr);
                characterize_workload(profiling_result);
                float rc = get_throughput(profiling_result->cpu->time,profiling_result->total_instr_retired)
                    float rg = get_throughput(profiling_result->gpu->time,profiling_result->total_instr_retired)
                    float energy = get_energy();

                float min_obj_res = MAX_POSSIBLE_VALUE;
                for(float alph_tmp = 0.0; alpha <=1.0; alph_tmp += 0.1)
                {
                    float time = get_tcg(*rc,*rg,N,*alph_tmp);
                    float obj_res = objfun(energy,time);
                    if(obj_res < min_obj_res)
                    {
                        min_obj_res = obj_res;
                        alpha = alph_tmp;
                    }
                }
                shared_counter = profiling_result->total_instr_retired;
                Nrem = shared_counter;
            }
        }

        return *alpha;
    }
    void *get_gpu_profile_size()//returs the gpu profile size for specific GPU
    {
        return NULL;
    }
    int check_alpha(void *ptr,void *table)//check and return alpha if already exist for a given function
    {
        int result = -1;
        for(int i = 0; i < table->size;i++)
        {
            if(ptr == table->function_type[i])
            {
                return i;
            }
        }

        return result;
    }
    float *getalpha(void *ptr,void *table)//return alpha if already exist for a given function
    {
        int result = check_alpha(ptr,table);
        if(result != NOT_FOUND)
        {
            return &(table->alpha[result]);
        }
        else
        {
            return NULL;
        }
    }
    void set_alpha(void *ptr, void *table, float alpha)
    {
        int res = check_alpha(ptr,table);
        if(res == NOT_FOUND)
        {
            //insert alpha at end of table
        }
        else
        {
            //update value of alpha in table
        }
    }
    void characterize_workload(void *ptr)//characterize workload on the basis of profiling results
    {
        //print result on the basis of characterization predicted from profiling data and fitting the curve
    }

    /* 
     * perf_event_open() returns a file descriptor that allows measuring performance information.
     * @param hw_event used to specify hardware event
     * @param group_fd used to allows event groups to be created.
     */
    static long perf_event_open(struct perf_event_attr *hw_event, pid_t pid, int cpu, int group_fd, unsigned long flags) 
    { 
        int ret;
        ret = syscall(__NR_perf_event_open, hw_event, pid, cpu, 
                group_fd, flags); 
        return ret;
    }

    void retired_instruc()//functin to count Retired instructions.
    {	
        struct perf_event_attr pe; 
        long long count; 
        int fd;


        memset(&pe, 0, sizeof(struct perf_event_attr)); 
        pe.type = PERF_TYPE_HARDWARE; 
        pe.size = sizeof(struct perf_event_attr); 
        pe.config = PERF_COUNT_HW_BRANCH_INSTRUCTIONS; 
        pe.disabled = 1; 
        pe.exclude_kernel = 1; 
        pe.exclude_hv = 1;

        fd = perf_event_open(&pe, 0, -1, -1, 0); 
        if (fd == -1) { 
            fprintf(stderr, "Error opening leader %llx\n", pe.config);
            exit(EXIT_FAILURE); 
        }


        ioctl(fd, PERF_EVENT_IOC_RESET, 0); 
        ioctl(fd, PERF_EVENT_IOC_ENABLE, 0);


        printf("count number of retired instructions\n");


        ioctl(fd, PERF_EVENT_IOC_DISABLE, 0); 
        read(fd, &count, sizeof(long long));


        printf("retired instructions %lld\n", count);


        close(fd); 
    }  


}
