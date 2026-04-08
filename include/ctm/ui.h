#ifndef UI_H
#define UI_H

#ifndef INSIDE_CTM_H
#error "Only ctm/ctm.h can be included directly!"
#endif

int ui_start(int argc, char** argv);
int ui_update(void* user_data);

#endif
