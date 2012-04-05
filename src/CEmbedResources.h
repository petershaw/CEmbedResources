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


#ifndef CEMBEDRESOURCES_H
#define	CEMBEDRESOURCES_H

#ifdef	__cplusplus
extern "C" {
#endif

    typedef void fooPtr(void *);
    
    
    /**
     * Structure of a single resource
     */
    typedef struct CER_resource {
        long int resourceSize;
        char *resourceIdentifyer;
        char *resourceName;
        char *content;
    } CER_resource_t;

    /**
     * Binary tree of all resources
     */ 
    struct CER_resourcetable {
        char *resourceIdentifyer;
        fooPtr *callback;
        struct CER_resourcetable *left;
        struct CER_resourcetable *right;
    };
    typedef struct CER_resourcetable CER_resourcetable_t;


    /**
     * Initialize the resource table.
     * 
     * @param CER_resourcetable_t resourcetable to initialize.
     */
    void CEmbedResources_Init(CER_resourcetable_t *, int (*initFn)(CER_resourcetable_t *, void *));

    /**
     * Returns the number of resources stored in the table.
     * 
     * @param CER_resourcetable_t resourcetable to count elements in.
     * @return int number of elements
     */
    int CEmbedResources_Size(CER_resourcetable_t *);

    /**
     * Copy a resource from the table with a specific Identifyer into a variable
     * and return true (0) if the operation pass, a number of visited elements,
     * instead.
     * 
     * @param CER_resourcetable_t resourcetable to get the elements from.
     * @param char * - name of the identifyer
     * @param CER_resource_t * - pointer to the target variable 
     * @return int - 0 on success, otherwise number of visited elements
     */
    int CEmbedResources_GetResourceByIdentifyer(CER_resourcetable_t *,
            char *,
            CER_resource_t *);

    /**
     * Returns the content of a resource from the table with a specific 
     * Identifyer.
     * 
     * @param CER_resourcetable_t resourcetable to get the elements from.
     * @param char * - name of the identifyer
     * @return char * of the content 
     */
    char *CEmbedResources_GetContentByIdentifyer(CER_resourcetable_t *,
            char *);

    /**
     * Insert a resource into the table.
     * 
     * @param CER_resourcetable_t resourcetable to insert element into
     * @param CER_resource_t element to insert.
     * @return int - 0 on success!
     */
    int CEmbedResources_Insert(CER_resourcetable_t *, char *, void *);

#ifdef	__cplusplus
}
#endif

#endif	/* CEMBEDRESOURCES_H */

