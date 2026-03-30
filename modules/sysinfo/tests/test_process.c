//
// Created by martin on 27.03.2026.
//

#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#include "ctm_process.h"

int main(void) {
    CtmProcessArray     array  = {0};
    const pid_t         pid    = getpid();
    const int result = ctm_process_fetch_all(&array, 0);

    assert(result == 0);
    assert(array.capacity > 0);
    assert(array.count > 0);

    int found_self = 0;
    for (size_t i = 0; i < array.count; i++) {
        if (array.elements[i].pid == pid) {
            found_self = 1;

            printf("Found self! Name: %s, RSS: %lu KB, UID: %d, Username: %s\n", array.elements[i].name, array.elements[i].rss_kb, array.elements[i].uid, array.elements[i].username);

            assert(strlen(array.elements[i].name) > 0);
            assert(array.elements[i].rss_kb > 0);
            assert(array.elements[i].uid == getuid());
            break;
        }
    }

    assert(found_self && "The parser failed to find the current running process in /proc");
    ctm_process_array_destroy(&array);

    return 0;
}
