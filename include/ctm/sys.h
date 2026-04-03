//
// Created by martin on 03.04.2026.
//

#ifndef CLUELESS_TASK_MGR_CTM_HW_H
#define CLUELESS_TASK_MGR_CTM_HW_H
#include <stddef.h>
#include <time.h>

typedef struct CtmCpu          CtmCpu;
typedef struct CtmMemory       CtmMemory;
typedef struct CtmSystemInfo   CtmSystemInfo;
typedef struct CtmProcess      CtmProcess;
typedef struct CtmProcessArray CtmProcessArray;

CtmProcess*        CtmProcess_Create(pid_t pid, unsigned int uid, const char* name, const char* state, const char* username, unsigned long utime, unsigned long stime, unsigned long rss_kb, unsigned long long start_time);
void               CtmProcess_Destroy(CtmProcess* process);
CtmProcess*        CtmProcess_LoadFromKernel(pid_t pid, int include_all_users);
pid_t              CtmProcess_GetPid(const CtmProcess* process);
const char*        CtmProcess_GetName(const CtmProcess* process);
const char*        CtmProcess_GetState(const CtmProcess* process);
const char*        CtmProcess_GetUsername(const CtmProcess* process);
unsigned long long CtmProcess_GetTotalTime(const CtmProcess* process);
unsigned long long CtmProcess_GetCPUUsageScaled(const CtmProcess* process);

typedef enum {
    UNIT_KB = 0,
    UNIT_MB = 1,
    UNIT_GB = 2,
    UNIT_TB = 3
} CtmDataUnit;

CtmCpu*      CtmCpu_Create(void);
void         CtmCpu_Destroy(CtmCpu* handle);
CtmCpu*      CtmCpu_LoadFromKernel(void);
unsigned int CtmCpu_GetUsageScaled(CtmCpu* handle);

CtmMemory*    CtmMemory_Create(void);
void          CtmMemory_Destroy(CtmMemory* handle);
CtmMemory*    CtmMemory_LoadFromKernel(void);
unsigned long CtmMemory_GetAvailable(CtmMemory* handle);
unsigned long CtmMemory_GetFree(CtmMemory* handle);
unsigned long CtmMemory_GetTotal(CtmMemory* handle);
double        CtmMemory_GetUsageScaled(CtmMemory* handle);

CtmSystemInfo*   CtmSystemInfo_Create(void);
void             CtmSystemInfo_Destroy(CtmSystemInfo* handle);
CtmSystemInfo*   CtmSystemInfo_LoadFromKernel(void);
CtmCpu*          CtmSystemInfo_GetCpu(const CtmSystemInfo* handle);
CtmMemory*       CtmSystemInfo_GetMemory(const CtmSystemInfo* handle);
CtmProcessArray* CtmSystemInfo_GetProcesses(const CtmSystemInfo* handle);
time_t           CtmSystemInfo_GetTimestamp(const CtmSystemInfo* sys_stats);

CtmProcessArray*  CtmProcessArray_Create(void);
void              CtmProcessArray_Destroy(CtmProcessArray* handle);
CtmProcessArray*  CtmProcessArray_LoadFromKernel(int include_all_users);
size_t            CtmProcessArray_GetCount(const CtmProcessArray* array);
size_t            CtmProcessArray_GetCapacity(const CtmProcessArray* array);
const CtmProcess* CtmProcessArray_GetElement(const CtmProcessArray* array, size_t index);
const CtmProcess* CtmProcessArray_GetElements(const CtmProcessArray* array);
int               CtmProcessArray_Push(CtmProcessArray* handle, const CtmProcess* item);

#endif //CLUELESS_TASK_MGR_CTM_HW_H
