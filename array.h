/**
 *@file array.h
 *  A simple Array datatype to store array of strings
 *  @author Dennis Addo
 *  @date 24/07/2017
 **/
 
#ifndef ARRAY_H
#define ARRAY_H
 
#include <stdio.h>
 
 
typedef char **Array;
 
 
extern Array init_string(size_t size);
 
extern size_t get_size(void);
 
extern void add_str(Array *str, const char *s);
 
extern void freeall(Array *ss, size_t len);
 
 
#endif //ARRAY_H
