//
// Created by martin on 27.03.2026.
//

#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#include "process.h"

int main(void) {
    pid_t pid = getpid();
    printf("Test is running with PID: %d\n", pid);

    process_array_t array = {0};
    const stat_result_t result = process_fetch_all(&array);

    assert(result == STAT_SUCCESS);
    assert(array.count > 0);

    int found_self = 0;
    for (size_t i = 0; i < array.count; i++) {
        if (array.processes[i].pid == pid) {
            found_self = 1;
            
            printf("Found self! Name: %s, RSS: %lu KB\n", array.processes[i].name, array.processes[i].rss_kb);

            assert(strlen(array.processes[i].name) > 0);
            assert(array.processes[i].rss_kb > 0);
            break;
        }
    }

    assert(found_self && "The parser failed to find the current running process in /proc");
    
    process_array_free(&array);
    
    printf("Integration test passed: Real /proc data parsed successfully!\n");
    return 0;
}
