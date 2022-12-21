#ifndef _TESTS
#define _TESTS

#include "data_structures.h"

typedef int(test_func_t)(char *);

typedef struct _test_case
{
    const char *name;
    test_func_t *func;
} test_case;

typedef void(register_func_t)(slice *);

typedef struct _test_suite
{
    const char *name;
    test_case *tests;
    int n;
} test_suite;

void register_data_structure_tests(slice *tests_suites);
void register_obj3d_tests(slice *tests_suites);
void register_entities_tests(slice *tests_suites);

#endif
