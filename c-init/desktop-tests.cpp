// SPDX-License-Identifier: zlib-acknowledgement
#include <stdarg.h>
#include <stdint.h>
#include <stddef.h>
#include <setjmp.h>
#include <limits.h>

#include "desktop.cpp"

EXPORT_BEGIN
#include <cmocka.h>
EXPORT_END

void
test_example(void **state)
{

}

int 
main(void)
{
	const struct CMUnitTest tests[] = {
    cmocka_unit_test(test_example),
  };

  int cmocka_res = cmocka_run_group_tests(tests, NULL, NULL);

  return cmocka_res;
}
