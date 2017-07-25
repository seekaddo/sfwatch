/**
 *@file string.h
 *	A simple string datatype to store array of strings
 *	@author Dennis Addo
 *	@date 24/07/2017
 **/

#ifndef STRING_H
#define STRING_H

#include <stdio.h>


typedef char **string;


extern string init_string(size_t size);

extern size_t get_size(void);

extern void add_str(string *str, const char *s);

extern void freeall(string *ss, size_t len);


#endif //STRING_H
