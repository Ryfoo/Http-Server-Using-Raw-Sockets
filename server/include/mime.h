#ifndef MIME_H
#define MIME_H

/*
   Returns a Content-Type string for the given path's extension.
   Falls back to "application/octet-stream" for unknown extensions.
   The returned pointer is to a static literal — do not free().
*/
const char* mime_for_path(const char* path);

#endif
