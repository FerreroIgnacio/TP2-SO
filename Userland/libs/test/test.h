#ifndef TEST_H
#define TEST_H

#include "./test_util.h"
#include "./syscall.h"

int test_mm(int max_memory);
int test_processes(int max_processes);
int test_prio(int max_val);
int test_sync(int iter, int sem);

#endif
