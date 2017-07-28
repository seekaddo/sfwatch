/**
 *@file string.h
 *  A simple dynamic string  array for storing strings
 	using struct for a compact datatype and remove the tripple star programming
 	This sd_array is tracks the size and capactiy for easy access.
 *  @author Dennis Addo
 *  @date 24/07/2017
 **/
 
#ifndef ARRAY_H
#define ARRAY_H
 
#include <stdio.h>
 
 
typedef struct sd_array {
    size_t capacity;        //holds the total size of the array
    size_t size;            //holds the current number of elements in the array
    char **mem;             //where elements are stored
}sds_array;
 
 
extern sds_array init_sds_array(size_t size);
 
extern void add_str(sds_array *sdsa,const char *s);
 
extern void delete_str(sds_array *array, const size_t index);
 
extern void freeall(sds_array *sds,size_t len);
 
 
#endif //ARRAY_H
