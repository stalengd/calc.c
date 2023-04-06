#include "test_vector.h"


void test_create_vector_is_valid(void** state) {
    Vector vec = vectorCreate(16, sizeof(int));
    assert_true(vec.data != NULL);
    vectorFree(&vec);
}

void test_create_vector_is_empty(void** state) {
    Vector vec = vectorCreate(16, sizeof(int));
    assert_true(vec.size == 0);
    vectorFree(&vec);
}

void test_vector_get_value(void** state) {
    Vector vec = vectorCreate(16, sizeof(int));

    int val0 = 32;
    int val1 = 33;
    vectorPush(&vec, &val0);
    vectorPush(&vec, &val1);

    assert_int_equal(val1, *(int*)vectorGet(&vec, 1));
    vectorFree(&vec);
}

void test_vector_pop_decrease_size(void** state) {
    Vector vec = vectorCreate(16, sizeof(int));

    int val = 32;
    vectorPush(&vec, &val);
    vectorPush(&vec, &val);
    vectorPush(&vec, &val);

    vectorPop(&vec, NULL);

    assert_true(vec.size == 2);
    vectorFree(&vec);
}


int test_vector_run() {
    const struct CMUnitTest tests[] = {
        cmocka_unit_test(test_create_vector_is_valid),
        cmocka_unit_test(test_create_vector_is_empty),
        cmocka_unit_test(test_vector_get_value),
        cmocka_unit_test(test_vector_pop_decrease_size),
    };
 
    return cmocka_run_group_tests(tests, NULL, NULL);
}