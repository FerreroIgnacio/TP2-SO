#ifndef TEST_H
#define TEST_H

#include "./test_util.h"
#include "./syscall.h"

uint64_t test_mm(uint64_t argc, char *argv[]);
int64_t test_processes(uint64_t argc, char *argv[]);
uint64_t test_prio(uint64_t argc, char *argv[]);
uint64_t test_sync(uint64_t argc, char *argv[]);

#endif
