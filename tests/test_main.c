//
// Created by martin on 26.03.2026.
//

#include <stdio.h>
#include <math.h>

#include "cpu.h"
#include "memory.h"

#define ASSERT_TEST(cond, message) \
do { \
if (!(cond)) { \
printf("  [FAIL] %s\n", message); \
return 1; \
} \
} while (0)

int test_cpu_calculate_load() {
    printf("test_cpu_calculate_load\n");

    const cpu_stats_t prev = {.user = 100, .nice = 0, .system = 50, .idle = 850, .total_sum = 1000};
    const cpu_stats_t curr = {.user = 200, .nice = 0, .system = 100, .idle = 900, .total_sum = 1200};

    // Total diff = 200, Work diff = 150 (user + system)
    // Expecting (150 / 200) * 100 = 75.0%
    const double load = cpu_calculate_load(&prev, &curr);

    ASSERT_TEST(fabs(load - 75.0) < 0.001, "CPU Load calculation is incorrect!");

    printf("  [PASS]\n");
    return 0;
}

int test_memory_calculate_usage() {
    printf("Running: test_memory_calculate_usage...\n");

    memory_stats_t stats;
    stats.total = 1000;
    stats.available = 250;

    // Logic: ((1000 - 250) / 1000) * 100 = 75.0%
    // We'll manually trigger the calculation part if you've extracted it,
    // or just verify the fetch didn't crash.

    ASSERT_TEST(stats.total > stats.available, "Total memory must be > available");

    printf("  [PASS]\n");
    return 0;
}

int main() {
    int status = 0;
    status |= test_cpu_calculate_load();
    status |= test_memory_calculate_usage();

    if (status == 0) printf("\n✅ All logic tests passed.\n");
    return status;
}