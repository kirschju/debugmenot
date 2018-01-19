#ifndef _GDB_DETECT_H
#define _GDB_DETECT_H

#define VERSION_MAJOR "1"
#define VERSION_MINOR "0"

#define DEBUG

#define RESULT_NO  0
#define RESULT_UNK 1
#define RESULT_YES 2

struct test_chain {
    /* Performs the actual test. Nonzero return means "debugger found" */
    int (*detect)(void);
    /* Cleanup any leftovers for asynchronous tests */
    int (*cleanup)(void);
    /* Human readable description of the test. */
    const char *description;
    /* Short, but UNIQUELY identifying name of the test. */
    const char *name;
    struct test_chain *next_test;
};

struct test_chain *test_chain_alloc_new(struct test_chain *);
void test_chain_free_all(struct test_chain *all_tests);

int aslr_active(void);

#if ! defined __linux__
#error "Only supported operating system is Linux."
#endif

#define TEST_ID_MAX 32
#if TEST_ID_MAX > 32
#error "TEST_ID_MAX greater than 32 needs fixing up the print_available_tests function."
#endif

#if ! defined __amd64__ && \
    ! defined __arm__ && \
    ! defined __aarch64__ && \
    ! defined __i386__
#error "Compiling for unknown architecture. Only x86(-64) and ARMV[78] are supported."
#endif

#define ARCH_AMD64 0
#define ARCH_I386  1
#define ARCH_ARM64 2
#define ARCH_ARMV7 3

extern const char *arch_strings[];
extern unsigned int this_arch;

#define TEST_DESC_LINE_LEN 52
#define TEST_NAME_INDENT   4
#define TEST_DESC_INDENT   8

#endif
