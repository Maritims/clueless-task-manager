//
// Created by martin on 28.03.2026.
//

#include <assert.h>

#include "ctm_sysinfo.h"

int main(void) {
    CtmSysStats sysinfo = {0};
    const int   result  = ctm_sysinfo_fetch(&sysinfo);

    assert(result == 0);
    assert(sysinfo.timestamp > 0);
    assert(sysinfo.cpu.total_sum > 0);
    assert(sysinfo.memory.total > 0);
    assert(sysinfo.processes.count > 0);

    return 0;
}
