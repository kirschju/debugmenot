#ifndef _TEST_PARENT_H
#define _TEST_PARENT_H

#include "debugmenot.h"

int register_test_parent(struct test_chain *, unsigned int);

#define TEST_ID_PARENT   4
#define TEST_NAME_PARENT "parent"
#define TEST_DESC_PARENT "Application checks whether parent's name is gdb, strace or ltrace."

#endif
