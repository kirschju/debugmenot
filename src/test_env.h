#ifndef _TEST_VDSO_H
#define _TEST_VDSO_H

#include "debugmenot.h"

int register_test_env(struct test_chain *, unsigned int);

#define TEST_ID_ENV   0
#define TEST_NAME_ENV "env"
#define TEST_DESC_ENV "Application checks existence of LINES and COLUMNS environment variables."

#endif
