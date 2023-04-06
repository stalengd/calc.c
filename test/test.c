#include <stdarg.h>
#include <stddef.h>
#include <stdint.h>
#include <setjmp.h>
#include <cmocka.h>

#include "test_calc.h"

int main(int argc, char const *argv[])
{
    test_vector_run();
    return test_calc_run();
}
