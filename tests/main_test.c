
#include <stdio.h>

#include "data_structures.h"

#include "tests.h"

register_func_t *registrations[] = {
    register_entities_tests,
    register_obj3d_tests,
    register_data_structure_tests,
};

int main()
{
    int err;
    char error_buffer[4096];

    slice tests_suites = {0, 0, sizeof(test_suite), NULL};
    int num_suites = sizeof(registrations) / sizeof(register_func_t *);
    for (size_t i = 0; i < num_suites; i++)
    {
        registrations[i](&tests_suites);
    }

    int n = tests_suites.len;
    test_suite *suites = (test_suite *)tests_suites.data;
    for (size_t i = 0; i < n; i++)
    {
        printf("\n### Running Suite: %s\n", suites[i].name);
        for (size_t x = 0; x < suites[i].n; x++)
        {
            printf("==========\n\n");

            test_case test = suites[i].tests[x];

            printf("Running: %s\n", test.name);
            memset(error_buffer, 0, sizeof(error_buffer));
            err = test.func((char *)error_buffer);
            if (err != 0)
            {
                printf("FAIL: %s\n\t%s\n", test.name, error_buffer);
                return 1;
            }
            printf("Success: %s\n", test.name);
        }
        printf("==========\n\n");
    }
}
