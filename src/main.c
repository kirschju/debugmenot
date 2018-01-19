#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <sys/auxv.h>

#include "debugmenot.h"

#include "test_env.h"
#include "test_ptrace.h"
#include "test_vdso.h"
#include "test_noaslr.h"
#include "test_parent.h"
#include "test_ldhook.h"

unsigned int this_arch = UINT_MAX;

const char *arch_strings[] = {
    [ARCH_AMD64] = "x86_64",
    [ARCH_I386]  = "i686",
    [ARCH_ARM64] = "aarch64",
    [ARCH_ARMV7] = "v7l",
};

int aslr_active(void)
{
    char aslr_state[2] = { 0 };
    int res = 0;
    FILE *fp = fopen("/proc/sys/kernel/randomize_va_space", "r");

    if (!fp)
        return RESULT_UNK;

    if (fread((void *)aslr_state, 1, sizeof(aslr_state) - 1, fp) != sizeof(aslr_state) - 1) {
        fclose(fp);
        return RESULT_UNK;
    }

    if (aslr_state[0] != '0')
        res = RESULT_YES;
    else
        res = RESULT_NO;

    fclose(fp);
    return res;
}


struct test_chain *test_chain_alloc_new(struct test_chain *all_tests)
{
    struct test_chain *test = NULL, *tail = NULL;

    if (!all_tests) {
#ifdef DEBUG
        fprintf(stderr, "BUG: all_tests is NULL!\n");
#endif
        return NULL;

    }

    test = calloc(1, sizeof(struct test_chain));
    if (!test) {
        fprintf(stderr, "Out of memory when registering test file __FILE__\n");
        return NULL;
    }

    tail = all_tests;
    while (tail) {
        if (tail->next_test == NULL) {
            tail->next_test = test;
            break;
        }
        tail = tail->next_test;
    }


    return test;
}

void test_chain_free_all(struct test_chain *all_tests)
{
    struct test_chain *nxt = NULL;

    if (!all_tests) {
#ifdef DEBUG
        fprintf(stderr, "BUG: all_tests is NULL!\n");
#endif
        return;

    }

    while (all_tests) {
        nxt = all_tests->next_test; 
        all_tests->cleanup();
        free(all_tests);
        all_tests = nxt;
    }
    return;
}

static void print_available_tests(FILE *fp) {

    size_t i = 0, j = 0, line_end = 0;
    char *cur_line = NULL;

    fprintf(fp, "Available tests:\n");
    fprintf(fp, "----------------\n\n");

    const char *tests[TEST_ID_MAX][2] = {
        [TEST_ID_ENV] = { TEST_NAME_ENV, TEST_DESC_ENV },
        [TEST_ID_PTRACE] = { TEST_NAME_PTRACE, TEST_DESC_PTRACE },
        [TEST_ID_VDSO]   = { TEST_NAME_VDSO, TEST_DESC_VDSO },
        [TEST_ID_NOASLR] = { TEST_NAME_NOASLR, TEST_DESC_NOASLR },
        [TEST_ID_PARENT] = { TEST_NAME_PARENT, TEST_DESC_PARENT },
        [TEST_ID_LDHOOK] = { TEST_NAME_LDHOOK, TEST_DESC_LDHOOK },
    };

    for (i = 0; i < TEST_ID_MAX; j = 0, i++) {
        if (tests[i][0] == NULL) continue;

        fprintf(fp, "%*s%#010x: %s\n", TEST_NAME_INDENT, "", 1 << i, tests[i][0]);

        while (strlen(tests[i][1] + j) > TEST_DESC_LINE_LEN) {
            cur_line = strdup(tests[i][1] + j);
            line_end = TEST_DESC_LINE_LEN;
            while (*(cur_line + line_end) != ' ' && line_end > 0)
                line_end--;
            cur_line[line_end] = '\x00';
            fprintf(fp, "\n%*s%s", TEST_DESC_INDENT, "", cur_line);
            j += line_end + 1;
            free(cur_line);
        }
        fprintf(fp, "\n%*s%s\n\n", TEST_DESC_INDENT, "", tests[i][1] + j);

    }
    return;
}

int main(int argc, char **argv)
{
    int res = 0;
    unsigned int test_bmp = UINT_MAX;
    char *endptr = NULL;
    struct test_chain head = { NULL, NULL, NULL, NULL, NULL };
    struct test_chain *cur = NULL;

    for (unsigned int i = 0; i < sizeof(arch_strings) / sizeof(arch_strings[0]); i++) {
        if (!strcmp((const char *)getauxval(AT_PLATFORM), arch_strings[i]))
            this_arch = i;
    }

    if (this_arch == UINT_MAX) {
        fprintf(stderr, "Running on unsupported architecture.\n");
        return -1;
    }

    if (argc >= 2) {
        test_bmp = (unsigned int)strtoul(argv[1], &endptr, 16);
        if (*endptr) {
            fprintf(stderr, "Failed to convert test selection bitmap. Did you enter a hexadecimal number?");
            return -1;
        }
    }

    head.name = "head";
    head.description = "List head. Should not be visible at all.";

    print_available_tests(stdout);

    res |= register_test_ptrace(&head, test_bmp);
    res |= register_test_vdso(&head, test_bmp);
    res |= register_test_noaslr(&head, test_bmp);
    res |= register_test_env(&head, test_bmp);
    res |= register_test_parent(&head, test_bmp);
    res |= register_test_ldhook(&head, test_bmp);

    if (res) {
        fprintf(stderr, "Failed to register one or more tests (bmp = %#018x)."
                                                          "Exiting ...", res);
        print_available_tests(stderr);
        return -1;
    }

    printf("Use a hexadecimal value representing a bitmap to select tests "
                                "in argv[1] (default %#x).\n\n", UINT_MAX);

    puts("Test results:");
    puts("-------------");

    for (cur = head.next_test; cur; cur = cur->next_test) {
        printf("%12s: ", cur->name);
        switch (cur->detect()) {
            case RESULT_NO:
            puts("\x1b[32mPASS\x1b[39m");
            break;
            case RESULT_YES:
            puts("\x1b[31mFAIL\x1b[39m");
            break;
            case RESULT_UNK:
            puts("\x1b[33mUNKNOWN\x1b[39m");
            break;
            default:
            fprintf(stderr, "BUG: Test %s returned invalid result!\n", cur->name);
            return -1;
        }
    }

    puts("");
    test_chain_free_all(head.next_test);
    

}
