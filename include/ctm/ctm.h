#ifndef CTM_CPU_METRICS_H
#define CTM_CPU_METRICS_H

typedef struct CPU         CPU;
typedef struct CPUSampler  CPUSampler;
typedef struct Memory      Memory;
typedef struct Process     Process;
typedef struct ProcessList ProcessList;

CPU*          cpu_alloc(void);
int           cpu_capture(CPU* cpu);
CPU*          cpu_get(void);
void          cpu_free(CPU* cpu);
size_t        cpu_size(void);
unsigned long cpu_get_total_time(const CPU* cpu);
unsigned long cpu_get_idle_time(const CPU* cpu);
unsigned long cpu_get_user_usage(const CPU* current, const CPU* previous);
unsigned long cpu_get_system_usage(const CPU* current, const CPU* previous);
unsigned long cpu_get_total_usage(const CPU* current, const CPU* previous);

CPUSampler* cpu_sampler_create(unsigned int interval_ms);
void        cpu_sampler_destroy(CPUSampler* sampler);
int         cpu_sampler_start(CPUSampler* sampler);
long        cpu_sampler_get_avg_usage(CPUSampler* sampler, size_t window);

Memory*       memory_get(void);
void          memory_free(Memory* memory);
unsigned long memory_get_total(const Memory* memory);
unsigned long memory_get_free(const Memory* memory);
unsigned long memory_get_available(const Memory* memory);

Process*     process_get(unsigned int pid);
void         process_free(Process* process);
unsigned int process_get_pid(const Process* process);
const char*  process_get_name(const Process* process);
const char*  process_get_state(const Process* process);
const char*  process_get_user(const Process* process);
unsigned int process_get_total_time(const Process* process);

ProcessList* process_list_get(void);
void         process_list_free(ProcessList* process_list);
size_t       process_list_get_count(const ProcessList* process_list);
Process*     process_list_get_process(const ProcessList* process_list, unsigned int pid);

#endif
