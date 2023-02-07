#include<stdio.h>
#include<stdlib.h>
#include<sys/types.h>
#include<sys/wait.h>
#include<unistd.h>
#include<time.h>
#include<signal.h>
#include<sched.h>
#include<sys/syscall.h>
#include<stdint.h>
#include<string.h>
#include<errno.h>

#define ROW (100)
#define COL ROW

#define MILLION 1000000

struct sched_attr {
       	uint32_t size;
       	uint32_t sched_policy;
       	uint64_t sched_flags;
	/* SCHED_NORMAL, SCHED_BATCH */
	int32_t sched_nice;
	/* SCHED_FIFO, SCHED_RR */
	uint32_t sched_priority;
	/* SCHED_DEADLINE (nsec) */
	uint64_t sched_runtime;
	uint64_t sched_deadline;
	uint64_t sched_period;
};

int count;
int cpu_id;
int nth;
struct timespec begin;

pid_t init_pid;
pid_t child_pid; // fork에 사용할 pid

static int sched_setattr(pid_t pid, const struct sched_attr *attr, unsigned int flags);
// Signal Handling
void sig_handler(int signo);
struct timespec time_diff(struct timespec start, struct timespec end)
{
    struct timespec temp;
	if ((end.tv_nsec-start.tv_nsec)<0) {
		temp.tv_sec = end.tv_sec-start.tv_sec-1;
		temp.tv_nsec = 1000000000+end.tv_nsec-start.tv_nsec;
	} else {
		temp.tv_sec = end.tv_sec-start.tv_sec;
		temp.tv_nsec = end.tv_nsec-start.tv_nsec;
	}
	return temp;
}

int calc(int time, int cpu){

    int matrixA[ROW][COL];
    int matrixB[ROW][COL];
    int matrixC[ROW][COL];
    int i, j, k;

    cpu_id = cpu;
    struct timespec last_time, current_time;
    clock_gettime(CLOCK_MONOTONIC, &begin);
    last_time = begin;
    
    while(1){
        for(i=0; i<ROW; i++){
            for(j=0; j<COL; j++){
                for(k=0; k<COL; k++){
                    matrixC[i][j] += matrixA[i][k] * matrixB[k][j];
                }
            }
        }
        count++;
	//to check 100ms
        clock_gettime(CLOCK_MONOTONIC, &current_time);
	struct timespec temp1 = time_diff(last_time, current_time);
        if((temp1.tv_nsec / MILLION) >= 100){
            printf("PROCESS #%02d count = %02d %ld\n", cpu_id, count, temp1.tv_nsec / MILLION);
	    last_time = current_time;
        }
	// to check processed time
	struct timespec temp2 = time_diff(begin, current_time);
	if(temp2.tv_sec >= time){
	       	printf("DONE! PROCESS #%02d count = %06d %ld\n", nth, count, temp2.tv_sec * 1000 + temp2.tv_nsec / MILLION);
		break;
	}

    }
    return 0;
}

int main(int argc, char* argv[]){
    (void)(argc);
    
    int num_process;
    int time_process;
    num_process = atoi(argv[1]);
    time_process = atoi(argv[2]);
    
    // set scheduling attribute
    struct sched_attr my_sched;
    memset(&my_sched, 0, sizeof(my_sched));
    my_sched.size = sizeof(struct sched_attr);
    my_sched.sched_policy = SCHED_RR;
    my_sched.sched_priority = 10;

    init_pid = getpid();
    sched_setattr(init_pid, &my_sched, 0); 
    
    int i;
    // fork child processes
    for(i=0; i<num_process; i++){
        child_pid = fork();
        if(child_pid) {
            break;
        }
        printf("Creating Process: #%d\n",i);
    }
    // to listen signal
    signal(SIGINT, sig_handler);
    nth = i - 1;
    if(getpid() == init_pid){
        while(wait(NULL) > 0);
        return 0;
    }
    // execute calculations
    calc(time_process, nth);
    return 0;
}

void sig_handler(int signo)
{
    (void)(signo);
    struct timespec current_time;
    //if the process has a child, kill it first
    if(child_pid){
        kill(child_pid, SIGINT);
    }
    // if it is not the initial process, print the counts and execution time
    if(getpid() != init_pid){
        struct timespec temp;
        clock_gettime(CLOCK_MONOTONIC, &current_time);
        temp = time_diff(begin, current_time);
        printf("DONE! PROCESS #%02d count = %06d %ld\n", nth, count, temp.tv_sec * 1000 + temp.tv_nsec / MILLION);
    }
    // wait for its child to exit first
    while(wait(NULL) > 0);
    exit(0);
}

static int sched_setattr(pid_t pid, const struct sched_attr *attr, unsigned int flags)
{
    return syscall(SYS_sched_setattr, pid, attr, flags);
}

