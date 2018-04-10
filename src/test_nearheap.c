#define _DEFAULT_SOURCE
#include <stdio.h>
#include <sys/auxv.h>
#include <unistd.h>
#include <stdlib.h>

#include "debugmenot.h"
#include "test_nearheap.h"

static int detect(void)
{
    /* GDB relocates the heap to the end of the bss section */
    static unsigned char bss;
    unsigned char *probe = malloc(0x10);

    if (probe - &bss > 0x20000) {
        return RESULT_NO;
    } else {
        return RESULT_YES;
    }

}

static int cleanup(void)
{
    /* Nothing to be done */ 
    return 0;
}

int register_test_nearheap(struct test_chain *all_tests, unsigned int test_bmp)
{
    struct test_chain *test;

    if (!(test_bmp & (1 << TEST_ID_NEARHEAP)))
        return 0;

    test = test_chain_alloc_new(all_tests);

    if (!test)
        return 1 << TEST_ID_NEARHEAP;

    test->detect = detect;
    test->description = TEST_DESC_NEARHEAP;
    test->name = TEST_NAME_NEARHEAP;
    test->cleanup = cleanup;

    return 0;
}
