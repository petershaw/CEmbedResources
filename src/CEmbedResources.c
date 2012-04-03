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

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "base64.h"
#include "CEmbedResources.h"

void CEmbedResources_Init(CER_resourcetable_t *table, int (*initFn)(CER_resourcetable_t *, void *)) {
    table->resourceIdentifyer = NULL;
    table->left = NULL;
    table->right = NULL;
    table->cer_resource = NULL;
    (*initFn)(table, &CEmbedResources_Insert);
}

int CEmbedResources_Size(CER_resourcetable_t *cur) {
    int nodeLeft, nodeRight;
    if (cur == NULL) {
        return 0;
    }
    nodeLeft = CEmbedResources_Size(cur->left);
    nodeRight = CEmbedResources_Size(cur->right);
    return (nodeLeft + nodeRight + 1);
}

int CEmbedResources_GetResourceByIdentifyer(CER_resourcetable_t *table,
        char *identifyer,
        CER_resource_t *target) {
    int visited = 0;
    target->content = NULL;
    target->resourceIdentifyer = NULL;
    target->resourceName = NULL;
    target->resourceSize = 0;
    CER_resourcetable_t *cur = table;
    while (cur != NULL) {
        ++visited;
        if (strcmp(cur->resourceIdentifyer, identifyer) < 0) {
            // look right;
            cur = cur->right;
            continue;
        } else if (strcmp(cur->resourceIdentifyer, identifyer) > 0) {
            // look left;
            cur = cur->left;
            continue;
        } else {
            // == 0
            // found:
            *target = *cur->cer_resource;
            break;
        }
    }
    return visited;
}

char *CEmbedResources_GetContentByIdentifyer(CER_resourcetable_t *table,
        char *identifyer) {
    CER_resourcetable_t *cur = table;
    while (cur != NULL) {
        if (strcmp(cur->resourceIdentifyer, identifyer) < 0) {
            // look right;
            cur = cur->right;
            continue;
        } else if (strcmp(cur->resourceIdentifyer, identifyer) > 0) {
            // look left;
            cur = cur->left;
            continue;
        } else {
            // == 0
            // found:
            unsigned char *dest = malloc(cur->cer_resource->resourceSize);
            decode_base64(dest, cur->cer_resource->content);
            return dest;
        }
    }
    return NULL;


}

int CEmbedResources_Insert(CER_resourcetable_t *table, CER_resource_t *element) {
    int errno = 0;
    if (table->resourceIdentifyer == NULL) {
        table->resourceIdentifyer = element->resourceIdentifyer;
        table->cer_resource = malloc(sizeof (CER_resource_t));
        memcpy(table->cer_resource, element, sizeof (CER_resource_t));
    } else {
        errno += _cer_insert_recr(table, element);
    }
    return errno;
}

/**
 * Walk down the btree and find a place to insert the element.
 * 
 * @param cur - current CER_resourcetable_t table-seqment
 * @param element - CER_resource_t element to insert 
 * @return int - error number.
 */
int _cer_insert_recr(CER_resourcetable_t *cur, CER_resource_t *element) {
    int errno = 0;
    int cmpResult = strncmp(element->resourceIdentifyer, cur->resourceIdentifyer, 32);
    if (cmpResult > 0) {
        // is right-side
        if (cur->right == NULL) {
            if ((cur->right = malloc(sizeof (CER_resourcetable_t))) == NULL) {
                return ++errno;
            }
            cur->right->resourceIdentifyer = element->resourceIdentifyer;
            cur->right->cer_resource = malloc(sizeof (CER_resource_t));
            memcpy(cur->right->cer_resource, element, sizeof (CER_resource_t));
            cur->right->left = NULL;
            cur->right->right = NULL;
        } else {
            errno += _cer_insert_recr(cur->right, element);
        }
    } else if (cmpResult < 0) {
        // is left-side 
        if (cur->left == NULL) {
            if ((cur->left = malloc(sizeof (CER_resourcetable_t))) == NULL) {
                return ++errno;
            }
            cur->left->resourceIdentifyer = element->resourceIdentifyer;
            cur->left->cer_resource = malloc(sizeof (CER_resource_t));
            memcpy(cur->left->cer_resource, element, sizeof (CER_resource_t));
            cur->left->left = NULL;
            cur->left->right = NULL;
        } else {
            errno += _cer_insert_recr(cur->left, element);
        }
    } else {
        // == (overwrite)
        cur->cer_resource = element;
        cur->cer_resource = malloc(sizeof (CER_resource_t));
        memcpy(cur->cer_resource, element, sizeof (CER_resource_t));
    }
    return errno;
}