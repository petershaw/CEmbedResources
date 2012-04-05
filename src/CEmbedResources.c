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
    table->callback = NULL; 
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
            cur->callback(target);
            
            char *dest[target->resourceSize];
            decode_base64(dest, target->content);
            
            target->content = strdup(dest);

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
            CER_resource_t *target = malloc(sizeof(CER_resource_t));
            cur->callback(target);
            
            char *dest = malloc(target->resourceSize * sizeof(char));
            decode_base64(dest, target->content);
            
            free(target);
            return dest;
        }
    }
    return NULL;


}

int CEmbedResources_Insert(CER_resourcetable_t *table, char *identifyer, void *callback) {
    int errno = 0;
    
    // DEBUG
    printf("Insert: %s\n", identifyer);
    
    if (table->resourceIdentifyer == NULL) {
        table->resourceIdentifyer = identifyer;
        table->callback = callback;
    } else {
        errno += _cer_insert_recr(table, identifyer, callback);
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
int _cer_insert_recr(CER_resourcetable_t *cur, char *identifyer, void *callback) {
    int errno = 0;
    int cmpResult = strncmp(identifyer, cur->resourceIdentifyer, 32);
    if (cmpResult > 0) {
        // is right-side
        if (cur->right == NULL) {
            if ((cur->right = malloc(sizeof (CER_resourcetable_t))) == NULL) {
                return ++errno;
            }
            cur->right->resourceIdentifyer = identifyer;
            cur->right->callback = callback;
            
            cur->right->left = NULL;
            cur->right->right = NULL;
        } else {
            errno += _cer_insert_recr(cur->right, identifyer, callback);
        }
    } else if (cmpResult < 0) {
        // is left-side 
        if (cur->left == NULL) {
            if ((cur->left = malloc(sizeof (CER_resourcetable_t))) == NULL) {
                return ++errno;
            }
            cur->left->resourceIdentifyer = identifyer;
            cur->left->callback = callback;
            
            cur->left->left = NULL;
            cur->left->right = NULL;
        } else {
            errno += _cer_insert_recr(cur->left, identifyer, callback);
        }
    } else {
        // == (overwrite)
        cur->left->callback = callback;
    }
    return errno;
}