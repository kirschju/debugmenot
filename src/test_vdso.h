#ifndef _TEST_ENV_H
#define _TEST_ENV_H

#include "debugmenot.h"

int register_test_vdso(struct test_chain *, unsigned int);

#define TEST_ID_VDSO   2
#define TEST_NAME_VDSO "vdso"
#define TEST_DESC_VDSO "Application measures distance of vdso and stack."

#endif
