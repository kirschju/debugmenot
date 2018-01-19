#define _DEFAULT_SOURCE
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <libgen.h>

#include "debugmenot.h"
#include "test_parent.h"

static int detect(void)
{
    char link_name[0x100] = { 0 };
    size_t target_max_len = 0x100;
    ssize_t target_real_len = 0;
    int res = 0;
    char *link_target = calloc(target_max_len, sizeof(char));
    if (!link_target) {
        fprintf(stderr, "Out of memory in file __FILE__!");
        return RESULT_UNK;
    }
    pid_t parent = getppid();

    snprintf(link_name, sizeof(link_name) - 1, "/proc/%u/exe", parent);
    while ((target_real_len = readlink(link_name, link_target, target_max_len))
                                                             == target_max_len) {
        target_max_len *= 2;
        /* BUG: This leaks a chunk of memory of size target_max_len if realloc
         * fails, but I'm willing to take this risk ... */
        link_target = realloc(link_target, target_max_len);
        if (!link_target) {
            fprintf(stderr, "Out of memory in file __FILE__ while growing buf!");
            return 0;
        }
        memset(link_target, '\x00', target_max_len);
    }

    if (!strcmp(basename(link_target), "gdb"))
        res = RESULT_YES;
    if (strstr(link_target, "lldb"))
        res = RESULT_YES;
    if (!strcmp(basename(link_target), "strace"))
        res = RESULT_YES;
    if (!strcmp(basename(link_target), "ltrace"))
        res = RESULT_YES;

    free(link_target);
    return res;
}

static int cleanup(void)
{
    /* Nothing to be done */ 
    return 0;
}

int register_test_parent(struct test_chain *all_tests, unsigned int test_bmp)
{
    struct test_chain *test;

    if (!(test_bmp & (1 << TEST_ID_PARENT)))
        return 0;

    test = test_chain_alloc_new(all_tests);

    if (!test)
        return 1 << TEST_ID_PARENT;

    test->detect = detect;
    test->description = TEST_DESC_PARENT;
    test->name = TEST_NAME_PARENT;
    test->cleanup = cleanup;

    return 0;
}
