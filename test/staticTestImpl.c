#include <stdlib.h>
#include "../../src/CEmbedResources.h"
#include "staticTestImpl.h"

static void _add_resouce_1(CER_resource_t *target){
    const char *content = "YQ==\0"; // "a"
    target->resourceName = "a.file";
    target->resourceSize = 2;
    target->resourceIdentifyer = "ABCD0000000000000000000000000000";
    target->content = (char *) content; 
}

// add right
static void _add_resouce_2(CER_resource_t *target){
    const char *content = "YWJjIGRlZg==\0"; // "abc def"
    target->resourceName = "b.file";
    target->resourceSize = 8;
    target->resourceIdentifyer = "ADED0000000000000000000000000000";
    target->content = (char *) content; 
}

// add right, right
static void _add_resouce_3(CER_resource_t *target){
    const char *content = "YWJjIGRlZiAxMjM=\0"; // "abc def 123"
    target->resourceName = "c.file";
    target->resourceSize = 12;
    target->resourceIdentifyer = "AEED0000000000000000000000000000";
    target->content = (char *) content; 
}

// add right, left
static void _add_resouce_4(CER_resource_t *target){
    const char *content = "YWJjIGRlZiAxMjM0\0"; // "abc def 1234"
    target->resourceName = "d.file";
    target->resourceSize = 13;
    target->resourceIdentifyer = "ADAD0000000000000000000000000000";
    target->content = (char *) content; 
}

int *initialize_resources(CER_resourcetable_t *table, int (*insertFunc)(CER_resourcetable_t *, CER_resource_t *)){
    int errno = 0;
    CER_resource_t *all_resources = calloc(NUM_OF_RESOURCES, sizeof(CER_resource_t));
    int i = 1;
    _add_resouce_1( &all_resources[i * sizeof(CER_resource_t)] );
    
    ++i;
    _add_resouce_2( &all_resources[i * sizeof(CER_resource_t)] );
    
    ++i;
    _add_resouce_3( &all_resources[i * sizeof(CER_resource_t)] );
    
    ++i;
    _add_resouce_4( &all_resources[i * sizeof(CER_resource_t)] );
    
    for(i=1; i<=NUM_OF_RESOURCES; ++i){
        errno += (*insertFunc)(table, &all_resources[i * sizeof(CER_resource_t)]);
    }
    return errno;
}

