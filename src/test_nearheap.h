#ifndef _TEST_NEARHEAP_H
#define _TEST_NEARHEAP_H

#include "debugmenot.h"

int register_test_nearheap(struct test_chain *, unsigned int);

#define TEST_ID_NEARHEAP   5
#define TEST_NAME_NEARHEAP "nearheap"
#define TEST_DESC_NEARHEAP "Application compares beginning of the heap " \
                           "to address of own BSS."

#endif
