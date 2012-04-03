/**
* 
* File:   c_builder.pl
* Author: Kris Wolff
* Twitter: @peter_shaw
*
* Created on March, 2012
* 
* 
* ----------------------------------------------------------------------------
* / embed resources in a static-c-library                                    /
*
*/

#include <stdio.h>
#include <stdlib.h>
#include "CUnit/Basic.h"
#include "../src/CEmbedResources.h"
#include "staticTestImpl.h"

/*
 * CUnit Test Suite
 */

int init_suite(void) {
    return 0;
}

int clean_suite(void) {
    return 0;
}

void test_initialize() {
    CER_resourcetable_t *table = malloc(sizeof (CER_resourcetable_t));
    CEmbedResources_Init(table, initialize_resources);
    int size = CEmbedResources_Size(table);
    CU_ASSERT_EQUAL(size, NUM_OF_RESOURCES);
    free(table);

}

void testGetResourceByIdentifyer() {
    CER_resourcetable_t *table2 = malloc(sizeof (CER_resourcetable_t));
    CEmbedResources_Init(table2, &initialize_resources);

    CER_resource_t *result = malloc(sizeof (CER_resource_t));
    int depth = CEmbedResources_GetResourceByIdentifyer(table2, (char *) "ADED0000000000000000000000000000", result);
    printf("-- %s (%d)\n", result->resourceName, depth);
    CU_ASSERT_EQUAL(result->resourceName, "b.file");
    CU_ASSERT_EQUAL(depth, 2);
    free(result);

    CER_resource_t *result2 = malloc(sizeof (CER_resource_t));
    depth = CEmbedResources_GetResourceByIdentifyer(table2, (char *) "ABCD0000000000000000000000000000", result2);
    printf("-- %s (%d)\n", result2->resourceName, depth);
    CU_ASSERT_EQUAL(result2->resourceName, "a.file");
    CU_ASSERT_EQUAL(depth, 1);
    free(result2);

    CER_resource_t *result3 = malloc(sizeof (CER_resource_t));
    depth = CEmbedResources_GetResourceByIdentifyer(table2, (char *) "ADAD0000000000000000000000000000", result3);
    printf("-- %s (%d)\n", result3->resourceName, depth);
    CU_ASSERT_EQUAL(result3->resourceName, "d.file");
    CU_ASSERT_EQUAL(depth, 3);
    free(result3);

    // resource not there.
    CER_resource_t *result4 = malloc(sizeof (CER_resource_t));
    depth = CEmbedResources_GetResourceByIdentifyer(table2, (char *) "ZZZZ0000000000000000000000000000", result4);
    printf("#- %s (%d)\n", result4->resourceName, depth);
    CU_ASSERT_EQUAL(result4->resourceName, NULL);
    CU_ASSERT_EQUAL(depth, 4);
    free(result4);

    free(table2);
}

void testGetResourceContentByIdentifyer() {
    CER_resourcetable_t *table2 = malloc(sizeof (CER_resourcetable_t));
    CEmbedResources_Init(table2, &initialize_resources);
    char *expected = "abc def";
    CER_resource_t *result = malloc(sizeof (CER_resource_t)); 
    int depth = CEmbedResources_GetResourceByIdentifyer(table2, (char *) "ADED0000000000000000000000000000", result);
    printf("-- %s (%d) = %s\n", result->resourceName, depth, result->content);
    printf("CHECK: %d\n", strcmp(result->content, expected));
    CU_ASSERT_EQUAL(strcmp(result->content, expected), 0);
    free(result);
    free(table2);
}

void testGetContentByIdentifyer() {
    CER_resourcetable_t *table3 = malloc(sizeof (CER_resourcetable_t));
    CEmbedResources_Init(table3, &initialize_resources);

    const char *expected = "abc def\0";
    char *result = CEmbedResources_GetContentByIdentifyer(table3, (char *) "ADED0000000000000000000000000000");
    printf("Content: %s Cmp(%d)\n", result, (strcmp(result, expected)));
    CU_ASSERT_STRING_EQUAL(result, expected);
    free(table3);
}

void testLargeContent(){
    CER_resourcetable_t *table4 = malloc(sizeof (CER_resourcetable_t));
    CEmbedResources_Init(table4, &initialize_resources);

    CER_resource_t *resultResource = malloc(sizeof (CER_resource_t));
    int depth = CEmbedResources_GetResourceByIdentifyer(table4, (char *) "D6670C850CD9357B2EE617E4C21CC5EB", resultResource);
    char *result = CEmbedResources_GetContentByIdentifyer(table4, (char *) "D6670C850CD9357B2EE617E4C21CC5EB");
    int len = strlen(result);
    
    CU_ASSERT_EQUAL(resultResource->resourceSize, len);
    free(table4);
}

int main() {
    CU_pSuite pSuite = NULL;

    /* Initialize the CUnit test registry */
    if (CUE_SUCCESS != CU_initialize_registry())
        return CU_get_error();

    /* Add a suite to the registry */
    pSuite = CU_add_suite("statictest", init_suite, clean_suite);
    if (NULL == pSuite) {
        CU_cleanup_registry();
        return CU_get_error();
    }

    /* Add the tests to the suite */
    if ((NULL == CU_add_test(pSuite, "test initialize", test_initialize)) ||
            (NULL == CU_add_test(pSuite, "testGetResourceByIdentifyer", testGetResourceByIdentifyer)) ||
            (NULL == CU_add_test(pSuite, "testGetResourceContentByIdentifyer", testGetResourceContentByIdentifyer)) ||
            (NULL == CU_add_test(pSuite, "testGetContentByIdentifyer", testGetContentByIdentifyer)) ||
            (NULL == CU_add_test(pSuite, "testLargeContent", testLargeContent))
            ) {
        CU_cleanup_registry();
        return CU_get_error();
    }

    /* Run all tests using the CUnit Basic interface */
    CU_basic_set_mode(CU_BRM_VERBOSE);
    CU_basic_run_tests();
    CU_cleanup_registry();
    return CU_get_error();
}
