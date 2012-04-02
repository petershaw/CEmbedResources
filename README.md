CEmbedResources - embed resources in a static c-library
=======================================================

In some projects, we need access to a bunch of files that we don't want to distribute in a resources folder, file by file. This toolchain helps to generate a compiled static c library, that can be included into another application by a single .a file that provide access to the resource content.

The c_build tool scans a directory and includes all files into a library with a minimalistic set of functions to get the content out.

I build this tool for a embedded webserver in one of my project. I got a lot of files from the frontend team that i can't provide in a nearby directory, because a need a single executable and don't want to build char*'s of each file by my own. Furthermore the Files changes a lot while I'm building the backend application. This Script is the result of my decision to generate some c code extern and just include the new generated static c library into the projects build process. 
Maybe it is helpful for you, too. 

State of CEmbedResources
------------------------
This tool provides a minimal access to the scanned and included resources and it is a very fast implementation by now. It is well tested in my environment but not in a generalized matter outside my office. So please help by getting it better and moor usable for the rest of this world.

###For example - and what you can do###
* It runs on Apple's Mac OS X (10.6.8) - no test on other systems.
* It uses the 'Identifyer' of a file (this is the md5 of the filename) - no functions to get the content by filename directly, yet. 
* it calls gcc and ar directly from the c_builder.pl script. No make, no configuration process is implemented, so it may brake on other systems or environments. 
* It stores all content inside a btree - so far so good, but the binary tree is not optimized by now. 
* This Readme is in a not so good english, please commit a better one. 

Precondition
------------
* A installed gnu c compiler (gcc)
* Installed perl interpreter

Toolchain
---------
Create a directory with all your resource files in there. It can have subdirectories if you want. Than goto the ./Tools directory inside the CEmbedResources project folder and run the *c_builder.pl* script. It's written in Perl and generates the necessary .c and .h files.

Run:
<code>
    # perl ./c_builder _directory_ [_library name_]
</code>

The script scans the folder _directory_ and reads all files in, generates a hashtable of all informations and writes the information and the content into a .c file. 
After the generation process it will be compiled using *gcc* and packed using *ar*. After all the result will be stored into a tarball and moved to the ./Tools/dist folder. 
By default the library name is **libcembedresources_tttt.a ** where tttt is the current timestamp. 

Using CEmbedResources in your project
-------------------------------------
After you successfully generates a static resource library with CEmbedResources tool c_builder.pl you can link your application against the generated .a file. By default this is the libcembedresources_???.a file inside the generated tar file in the ./Tools/dist folder. 

Inside the tarball are two header files that you have to #include in your code (examples below) and access the resource content by the functions that are described in the CEmbedResources.h file.

	* CEmbedResources_Init: Initialize the resource table.
	* CEmbedResources_Size: Returns the number of resources stored in the table.
	* CEmbedResources_GetResourceByIdentifyer: Copy a resource from the table with a specific Identifyer into a variable and return true (0) if the operation pass, a number of visited elements, instead.
	* CEmbedResources_GetContentByIdentifyer: Returns the content of a resource from the table with a specific Identifyer.
	* CEmbedResources_Insert: Insert a resource into the table while runtime.

Instead of *CEmbedResources_Size* that returns the count of elements in the btree even the elements added with *CEmbedResources_Insert* while runtime, the define of *NUM_OF_RESOURCES* holds the element count that is initialized inserted and build in the .a library. 

Examples
--------
<pre>
#include <stdio.h>
#include <stdlib.h>
#include "lib/CEmbedResources.h"
#include "lib/cer_resourcebundle.h"

/*
 * 
 */
int main(int argc, char** argv) {
    puts("Checking CEmbedResources.");
    
    // initialize Resourcebundle
    CER_resourcetable_t *resourcetable = malloc(sizeof (CER_resourcetable_t));
    CEmbedResources_Init(resourcetable, &initialize_resources);
    
    // create a resource space
    CER_resource_t *result = malloc(sizeof (CER_resource_t));
    
    // get a full resource
    int depth = CEmbedResources_GetResourceByIdentifyer(
            resourcetable, 
            (char *)"92C893BA06F2461748D9E51C3AD7D10B", 
            result);
    printf("-- %s (%d) %d byte\n", result->resourceName, depth, result->resourceSize);
    
    // get the encodes content from the result
    printf("%s\n", result->content);
    
    // get the decoded content 
    char *content = CEmbedResources_GetContentByIdentifyer(resourcetable, 
            (char *)"92C893BA06F2461748D9E51C3AD7D10B");
    printf("%s", content);
    
    // free all
    free(result);
    free(resourcetable);
    
    return (EXIT_SUCCESS);
}
</pre>

Customize the build process
---------------------------
You can customize the build process by editing the c_builder.pl script. Everything is straight forward, quick and dirty perl. The Files that are generated are build form templates. You can find the "header" and "implementation" template in the ./Tools/templates directory. 

Permission and Legal
--------------------
If you found this tool helpful please use it. It's free and you can do whatever you want. You can - if you want - remove the comments and names. You can reverse it upside down or print it out and let your dog chew on it. Do whatever you like - i do not care. But please, Éone last thing: please commit your fixes, that i can get your optimization back to my buildchain. 

Thanks a lot, 
Kris, aka @Peter_Shaw

Contact
-------
Please contact my via twitter <a href="http://twitter.com/peter_shaw">@peter_shaw</a>. 




 