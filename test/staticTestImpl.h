#include <stdlib.h>
#include "../../src/CEmbedResources.h"

#define NUM_OF_RESOURCES (4)


int *initialize_resources(CER_resourcetable_t *table, int (*insertFunc)(CER_resourcetable_t *, CER_resource_t *));

