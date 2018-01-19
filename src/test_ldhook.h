#ifndef _TEST_LDHOOK_H
#define _TEST_LDHOOK_H

#include "debugmenot.h"

int register_test_ldhook(struct test_chain *, unsigned int);

#define TEST_ID_LDHOOK   5
#define TEST_NAME_LDHOOK "ldhook"
#define TEST_DESC_LDHOOK "Application checks for breakpoint in _dl_debug_state"

#endif
