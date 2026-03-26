//
// Created by martin on 26.03.2026.
//

#ifndef CLUELESS_TASK_MGR_COMMON_H
#define CLUELESS_TASK_MGR_COMMON_H

/**
 * @brief Standard return codes for the library.
 */
typedef enum {
    STAT_SUCCESS = 0,
    STAT_ERR_OPEN = 1,
    STAT_ERR_PARSE = 2
} stat_result_t;

typedef enum {
    UNIT_KB = 0,
    UNIT_MB = 1,
    UNIT_GB = 2,
    UNIT_TB = 3
} data_unit_t;

#endif //CLUELESS_TASK_MGR_COMMON_H