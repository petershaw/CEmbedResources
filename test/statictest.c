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
    
    free(result);
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
    printf("1! %d %s %s [%p]\n", len, resultResource->resourceName, resultResource->content, resultResource->content);
    
    CU_ASSERT_EQUAL(strcmp(resultResource->content, result), 0);
    
    free(resultResource);
    free(table4);
}

/**
 * This Test demonstrate how to check memeory allocation and its free issue.
 * We allocate some space and assert that the memory usage grows. after a free 
 * of allocated space, the memory allocation must be resettet to a lower with.
 */
void testMemoryFreeCheck(){
    puts("- test Memory Free Check  -");
    puts("- ----------------------- -");
    puts("- This test takes a while -");
    puts("");
    struct rusage rus;
    getrusage(RUSAGE_SELF, &rus);
    
    // Things befor we want to mature. 
    char *holder[10000];
    char *bigHolder;
    int loops =  10000;
    int i;
    
    // Variables used for tests. 
    long int afterFirstTestKiloByte = 0;
    long int afterSecondTestKiloByte = 0;
    long int afterThirdTestKiloByte = 0;
    long int afterForuthTestKiloByte = 0;
    long int afterFivesTestKiloByte = 0;
    
    getrusage(RUSAGE_SELF, &rus);
    long int initialKiloByte = rus.ru_maxrss;
    printf("Initial KB Used: %ld\n", initialKiloByte);

    // a adder to proof that we get it write
    char add = "A";
    getrusage(RUSAGE_SELF, &rus);
    long int afterAddKiloByte = rus.ru_maxrss;
    printf("After a char add KB Used: %ld\n", initialKiloByte);
    getrusage(RUSAGE_SELF, &rus);
    long int growingKiloByte = rus.ru_maxrss;
    
    // This loop should do nothing.
    for(i=0; i<loops; ++i){
        printf(".");
    }
    puts("");
    getrusage(RUSAGE_SELF, &rus);
    afterFirstTestKiloByte = rus.ru_maxrss;
    printf("First Test should be 0 : %ld\n", (afterFirstTestKiloByte - afterAddKiloByte));
    CU_ASSERT_TRUE( (afterFirstTestKiloByte - afterAddKiloByte) == 0);
  
    // This loop should free all allocated space.
    for(i=0; i<(loops * 128); ++i){
        char *tmp = malloc(1024 * 1024);
        free(tmp);
    }
    
    getrusage(RUSAGE_SELF, &rus);
    afterSecondTestKiloByte = rus.ru_maxrss;
    printf("Second Test should be 4096: %ld\n", (afterSecondTestKiloByte - afterAddKiloByte));
    CU_ASSERT_TRUE( (afterSecondTestKiloByte - afterAddKiloByte) <= 4096);
    
    // This loop should increase the allocated space.
    for(i=0; i<loops; ++i){
         holder[i] = malloc(1024 * 1024);
    }
    getrusage(RUSAGE_SELF, &rus);
    afterThirdTestKiloByte = rus.ru_maxrss;
    printf("Third Test should be grater than 4096 : %ld\n", (afterThirdTestKiloByte - afterAddKiloByte));
    CU_ASSERT_TRUE( (afterThirdTestKiloByte - afterAddKiloByte) > 4096);
    
    // now free the memmory and get back to the initial value
    for(i=0; i<loops; ++i){
        free(holder[i]);
    }  
    
    getrusage(RUSAGE_SELF, &rus);
    afterForuthTestKiloByte = rus.ru_maxrss;
    printf("Forth Test should be reset to 0 (OS do not return free'd mem) (same as %ld) : %ld\n", (afterThirdTestKiloByte - afterAddKiloByte), (afterForuthTestKiloByte - afterAddKiloByte));
    CU_ASSERT_TRUE( (afterThirdTestKiloByte - afterAddKiloByte) == (afterThirdTestKiloByte - afterAddKiloByte));

    // This loop should use the allocated space.
    for(i=0; i<loops; ++i){
         holder[i] = malloc(1024 * 1024);
    }
    getrusage(RUSAGE_SELF, &rus);
    afterFivesTestKiloByte = rus.ru_maxrss;
    printf("Fives Test shouln'd be increase %ld == %ld\n", (afterThirdTestKiloByte - afterAddKiloByte), (afterFivesTestKiloByte - afterAddKiloByte));
    CU_ASSERT_TRUE( (afterFivesTestKiloByte - afterAddKiloByte) == (afterThirdTestKiloByte - afterAddKiloByte));
    puts("- ----------------------- -");
}

/**
 * Try to get the same resource a few times - result must be the same
 */
void testMulitigetSameElement(){
    // initialize
    
    CER_resourcetable_t *table = malloc(sizeof (CER_resourcetable_t));
    CEmbedResources_Init(table, &initialize_resources);
    
    // first get (reference get)
    CER_resource_t *resultResource = malloc(sizeof (CER_resource_t));
    int depth = CEmbedResources_GetResourceByIdentifyer(table, (char *) "D6670C850CD9357B2EE617E4C21CC5EB", resultResource);
    
    // CHECK POINTER - to be sure that other poiters are checked
    char *resourcePointer = malloc(16 * sizeof(char));
    sprintf(resourcePointer, "%p", resultResource);
    printf("ResourePointer: %s\n", resourcePointer);
    
    printf("Set remName with size: %d\n", (strlen(resultResource->resourceName) * sizeof(char)) );
    char* remName    = malloc( (strlen(resultResource->resourceName) * sizeof(char)) );

    strcpy(remName, resultResource->resourceName);
    char bufPointerName[16];
    char bufRemName[16];
    sprintf(&bufPointerName, "%p", resultResource->resourceName);
    sprintf(&bufRemName, "%p", remName);
    printf("PointerName   : %s\n", bufPointerName);
    printf("RemName       : %s\n", bufRemName);   
    CU_ASSERT_NOT_EQUAL(strcmp(bufPointerName, bufRemName), 0);
    CU_ASSERT_EQUAL(strcmp(resultResource->resourceName, remName), 0);
    puts("");
    
    printf("Set remContent with size: %d\n", (strlen(resultResource->content) * sizeof(char)) );
    char* remContent    = malloc( (strlen(resultResource->content) * sizeof(char)) );
    printf("ContentPointer      : %p\n", resultResource->content);
    printf("RemContent      new : %p\n", remContent);
    strcpy(remContent, resultResource->content);
    char bufPointerContent[16];
    char bufRemContent[16];
    sprintf(&bufPointerContent, "%p", resultResource->content);
    sprintf(&bufRemContent, "%p", remContent);
    printf("PointerContent   : %s\n", bufPointerContent);
    printf("RemContent       : %s\n", bufRemContent);   
    CU_ASSERT_NOT_EQUAL(strcmp(bufPointerContent, bufRemContent), 0);
    CU_ASSERT_EQUAL(strcmp(resultResource->content, remContent), 0);

    // get loop
    struct rusage rus;
    int loops = 5000;
    int i;
    long int memMemory = 0;
    long int memUsed = 0;
    long int cur = 0;
    getrusage(RUSAGE_SELF, &rus);
    memMemory = rus.ru_maxrss;
    
    CER_resource_t *loopResource = malloc(sizeof (CER_resource_t));
    
    for(i=0; i<loops; ++i){
        int depth = CEmbedResources_GetResourceByIdentifyer(table, (char *) "D6670C850CD9357B2EE617E4C21CC5EB", loopResource);    
    
        CU_ASSERT_EQUAL(strcmp(loopResource->content,      remContent), 0);
        CU_ASSERT_EQUAL(strcmp(loopResource->resourceName, remName), 0);


                
        getrusage(RUSAGE_SELF, &rus);
        printf("SIZE: mem: %ld cur: %ld kb\n", memMemory, rus.ru_maxrss);
        
        
        if(i > 1){
            cur = rus.ru_maxrss - memUsed;
            printf("ADD MEM: %d kb\n", cur);
            CU_ASSERT_TRUE(cur == 0);
        }
        memUsed = rus.ru_maxrss;
        
        // IMPORTENT TO CLEAN THE CONTENT!!!
        free(loopResource->content);
    }
    free(loopResource);
    
    printf("Loop done.\n");
    
    // cleanup
    free(remName);
    free(remContent);
    free(resultResource);
    free(table);
}

void testOK(){
    ;
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
            (NULL == CU_add_test(pSuite, "testLargeContent", testLargeContent)) ||
            (NULL == CU_add_test(pSuite, "testMemoryFreeCheck", testMemoryFreeCheck)) ||
            (NULL == CU_add_test(pSuite, "testMulitigetSameElement", testMulitigetSameElement)) ||
              (NULL == CU_add_test(pSuite, "testOK", testOK)) // just better handling comments
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
