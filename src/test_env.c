#define _DEFAULT_SOURCE
#include <stdio.h>
#include <stdlib.h>

#include "debugmenot.h"
#include "test_env.h"

static int detect(void)
{
    if (getenv("LINES") || getenv("COLUMNS"))
        return RESULT_YES; /* Debatable */
    else 
        return RESULT_NO;
}

static int cleanup(void)
{
    /* Nothing to be done */ 
    return 0;
}

int register_test_env(struct test_chain *all_tests, unsigned int test_bmp)
{
    struct test_chain *test;

    if (!(test_bmp & (1 << TEST_ID_ENV)))
        return 0;

    test = test_chain_alloc_new(all_tests);

    if (!test)
        return 1 << TEST_ID_ENV;

    test->detect = detect;
    test->description = TEST_DESC_ENV;
    test->name = TEST_NAME_ENV;
    test->cleanup = cleanup;

    return 0;
}
