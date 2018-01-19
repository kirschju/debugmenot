#ifndef _TEST_NOASLR_H
#define _TEST_NOASLR_H

#include "debugmenot.h"

int register_test_noaslr(struct test_chain *, unsigned int);

#define TEST_ID_NOASLR   3
#define TEST_NAME_NOASLR "noaslr"
#define TEST_DESC_NOASLR "Application checks base address of ELF and shared " \
                               "libraries for hard-coded values used by GDB."

#endif
