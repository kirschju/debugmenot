#ifndef _TEST_PTRACE_H
#define _TEST_PTRACE_H

#include "debugmenot.h"

int register_test_ptrace(struct test_chain *, unsigned int);

#define TEST_ID_PTRACE   1
#define TEST_NAME_PTRACE "ptrace"
#define TEST_DESC_PTRACE "Application tries to debug itself by calling " \
                                           "ptrace(PTRACE_TRACEME, ...)"

#endif
