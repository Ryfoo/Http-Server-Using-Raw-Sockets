#ifndef UTILS_H
#define UTILS_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "common.h"

void  load_env(const char* filename);

/*
   Reads an entire file into a freshly malloc()'d buffer.
   *out_size is set to the number of bytes read (binary-safe).
   Returns NULL on failure.
*/
char* load_file(const char* filename, long* out_size);

/*
   Locates a header value by name (case-insensitive). Returns NULL if absent.
*/
const char* header_get(const headers_list_t* hl, const char* name);

/*
   Appends a header. No-op when the table is full.
*/
void header_set(headers_list_t* hl, const char* name, const char* value);

/*
   Returns an RFC 7231 IMF-fixdate string for "Date:" headers.
   The buffer must be at least 32 bytes.
*/
void http_date_now(char* buffer, size_t buffer_size);

#endif
