#include <assert.h>
#include <stdio.h>

#include "ctm_sound.h"

void test_play_sound(void) {
    assert(ctm_play_sound("/home/martin/Nedlastinger/Wilhelm Scream Remastered.wav") == 0);
    printf("%s passed\n", __func__);
}

int main(void) {
    test_play_sound();
    printf("All tests passed\n");
}
