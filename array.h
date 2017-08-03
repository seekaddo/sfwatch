MIT License

Copyright (c) 2017 Dennis Addo

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.

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

extern int search_indx(size_t *ind, const sds_array *sda, const char *path);
 
extern void delete_str(sds_array *array, const size_t index);
 
extern void freeall(sds_array *sds,size_t len);
 
 
#endif //ARRAY_H
