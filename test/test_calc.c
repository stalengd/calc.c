#include "test_calc.h"

#define CALCULATION_EPSILON (float)0.00001

#define assert_expression(expression, expected) \
	_assert_expression((char* const)expression, \
			(float)expected, \
			__FILE__, __LINE__)

bool areFloatsEquals(float a, float b, float epsilon)
{
    return fabs(a - b) < epsilon;
}

void _assert_expression
    (char* const expression, float expected, const char* const file, const int line) 
{
    float result = 0;

    calculate(expression, &result);

    size_t buffSize = 64;
    char* buff = malloc(buffSize);
    while (StringCbPrintf(buff, buffSize, "%s = %f", expression, expected) 
        == STRSAFE_E_INSUFFICIENT_BUFFER)
    {
        free(buff);
        buffSize *= 2;
        buff = malloc(buffSize);
    }
    
    mock_assert(areFloatsEquals(expected, result, CALCULATION_EPSILON), buff, file, line);

    free(buff);
}

void test_2_plus_2_equals_4(void** state) {
    assert_expression("2 + 2", 4);
}

void test_222_plus_222_plus_1_equals_445(void** state) {
    assert_expression("222 + 222 + 1", 445);
}

void test_1_plus_b_2_plus_3_b_equals_6(void** state) {
    assert_expression("1 + (2 + 3)", 6);
}


int test_calc_run() {
    const struct CMUnitTest tests[] = {
        cmocka_unit_test(test_2_plus_2_equals_4),
        cmocka_unit_test(test_222_plus_222_plus_1_equals_445),
        cmocka_unit_test(test_1_plus_b_2_plus_3_b_equals_6),
    };
 
    return cmocka_run_group_tests(tests, NULL, NULL);
}