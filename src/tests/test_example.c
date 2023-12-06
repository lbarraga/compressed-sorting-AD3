//
// Created by lukasbt on 10/22/23.
//

#include "acutest.h"

void test_function(void)
{
    TEST_CHECK(1 + 1 == 2);
    TEST_MSG("Expected: %d", 2);
}

TEST_LIST = {
        {"test_function", test_function},
        {NULL, NULL} // mark end
};
