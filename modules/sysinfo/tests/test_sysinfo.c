//
// Created by martin on 28.03.2026.
//

#include <assert.h>

#include "sysinfo.h"

int main(void) {
    sysinfo_t           sysinfo = {0};
    const stat_result_t result  = sysinfo_fetch(&sysinfo);

    assert(result == STAT_SUCCESS);
    assert(sysinfo.timestamp > 0);
    assert(sysinfo.cpu.total_sum > 0);
    assert(sysinfo.memory.total > 0);

    return 0;
}
