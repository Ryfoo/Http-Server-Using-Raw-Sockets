#ifndef UTILS_H
#define UTILS_H
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Loading envrionmental variables
void load_env(const char *filename);
char* load_file(const char* filename, long* out_size);


#endif