#define _DEFAULT_SOURCE
#include <stdio.h>
#include <sys/auxv.h>
#include <sys/utsname.h>
#include <unistd.h>
#include <string.h>

#include "debugmenot.h"
#include "test_noaslr.h"

/* 64 or 32 bit ELF running on a 64 bit kernel */
static const unsigned long long elf_bits[] = {
    [ARCH_AMD64] = 0x0000555555000000ULL,
    [ARCH_I386]  = 0x0000000056550000ULL,
    [ARCH_ARM64] = 0x0000aaaaaa000000ULL,
    [ARCH_ARMV7] = 0x00000000aaaa0000ULL,
};

/* 32 bit ELF running on a 32 bit kernel. */
static const unsigned long long elf_bits_native[] = {
    [ARCH_I386]  = 0x0000000000400000ULL,
    [ARCH_ARMV7] = 0x0000000000400000ULL,
};

static const unsigned long long elf_mask[] = {
    [ARCH_AMD64] = 0x0000ffffff000000ULL,
    [ARCH_I386]  = 0x00000000ffff0000ULL,
    [ARCH_ARM64] = 0x0000ffffff000000ULL,
    [ARCH_ARMV7] = 0x00000000ffff0000ULL,
};

/* 64 or 32 bit shared library running on a 64 bit kernel */
static const unsigned long long lib_bits[] = {
    [ARCH_AMD64] = 0x00007ffff7000000ULL,
    [ARCH_I386]  = 0x00000000f7f00000ULL,
    [ARCH_ARM64] = 0x0000ffffb7000000ULL,
    [ARCH_ARMV7] = 0x00000000f7700000ULL,
};

/* 32 bit shared library running on a 32 bit kernel */
static const unsigned long long lib_bits_native[] = {
    [ARCH_I386]  = 0x00000000b7f00000ULL,
    [ARCH_ARMV7] = 0x0000000076f00000ULL,
};

static const unsigned long long lib_mask[] = {
    [ARCH_AMD64] = 0x0000ffffff000000ULL,
    [ARCH_I386]  = 0x00000000fff00000ULL,
    [ARCH_ARM64] = 0x0000ffffff000000ULL,
    [ARCH_ARMV7] = 0x00000000fff00000ULL,
};

static int detect(void)
{

    /* Program headers are close enough to the beginning of the ELF to be
     * representative for the ELF's base address. We use ld_base because it's
     * the simplest way to obtain an address of some shared library. */
    unsigned long elf = getauxval(AT_PHDR) & ~((unsigned long)getpagesize() - 1);
    unsigned long ld = getauxval(AT_BASE) & ~((unsigned long)getpagesize() - 1);
    struct utsname utsname;

    if (!elf || !ld) {
        return RESULT_UNK;
    }

    if (!(aslr_active() == RESULT_YES)) {
        /* No ASLR on this machine. Unknown result */
        return RESULT_UNK;
    }

    if (uname(&utsname) == -1) {
        return RESULT_UNK;
    }

    switch (this_arch) {
        case ARCH_AMD64:
        case ARCH_ARM64:
            if (((elf & elf_mask[this_arch]) == elf_bits[this_arch]) &&
                ((ld & lib_mask[this_arch]) == lib_bits[this_arch]))
                return RESULT_YES;
            return RESULT_NO;

        case ARCH_I386:
            if (!strcmp(utsname.machine, arch_strings[ARCH_AMD64])) {
                /* 32 bit binary running on 64 bit kernel */
                if (((elf & elf_mask[this_arch]) == elf_bits[this_arch]) &&
                    ((ld & lib_mask[this_arch]) == lib_bits[this_arch]))
                    return RESULT_YES;
                return RESULT_NO;

            } else {
                /* 32 bit binary running on 32 bit kernel */
                if (((elf & elf_mask[this_arch]) == elf_bits_native[this_arch]) &&
                    ((ld & lib_mask[this_arch]) == lib_bits_native[this_arch]))
                    return RESULT_YES;
                return RESULT_NO;

            }

        case ARCH_ARMV7:
            if (!strcmp(utsname.machine, arch_strings[ARCH_ARM64])) {
                /* 32 bit binary running on 64 bit kernel */
                if (((elf & elf_mask[this_arch]) == elf_bits[this_arch]) &&
                    ((ld & lib_mask[this_arch]) == lib_bits[this_arch]))
                    return RESULT_YES;
                return RESULT_NO;

            } else {
                /* 32 bit binary running on 32 bit kernel */
                if (((elf & elf_mask[this_arch]) == elf_bits_native[this_arch]) &&
                    ((ld & lib_mask[this_arch]) == lib_bits_native[this_arch]))
                    return RESULT_YES;
                return RESULT_NO;

            }
        default:
            return RESULT_UNK;
    }
}

static int cleanup(void)
{
    /* Nothing to be done */ 
    return 0;
}

int register_test_noaslr(struct test_chain *all_tests, unsigned int test_bmp)
{
    struct test_chain *test;

    if (!(test_bmp & (1 << TEST_ID_NOASLR)))
        return 0;

    test = test_chain_alloc_new(all_tests);

    if (!test)
        return 1 << TEST_ID_NOASLR;

    test->detect = detect;
    test->description = TEST_DESC_NOASLR;
    test->name = TEST_NAME_NOASLR;
    test->cleanup = cleanup;

    return 0;
}
