//
// Created by martin on 28.03.2026.
//

#ifndef CLUELESS_TASK_MGR_COMMON_H
#define CLUELESS_TASK_MGR_COMMON_H

typedef enum {
    STAT_SUCCESS     = 0,
    STAT_ERR_OPEN    = 1,
    STAT_ERR_PARSE   = 2,
    STAT_ERR_INVALID = 3,
    STAT_ERR_MALLOC  = 4
} stat_result_t;

#endif //CLUELESS_TASK_MGR_COMMON_H
