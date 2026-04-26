#include "../include/mime.h"
#include <string.h>
#include <strings.h>

static const struct {
    const char* ext;
    const char* mime;
} TABLE[] = {
    { ".html", "text/html; charset=utf-8" },
    { ".htm",  "text/html; charset=utf-8" },
    { ".css",  "text/css; charset=utf-8"  },
    { ".js",   "application/javascript"   },
    { ".json", "application/json"         },
    { ".txt",  "text/plain; charset=utf-8"},
    { ".png",  "image/png"                },
    { ".jpg",  "image/jpeg"               },
    { ".jpeg", "image/jpeg"               },
    { ".gif",  "image/gif"                },
    { ".svg",  "image/svg+xml"            },
    { ".ico",  "image/x-icon"             },
    { ".webp", "image/webp"               },
    { ".pdf",  "application/pdf"          },
    { NULL,    NULL                       }
};

const char* mime_for_path(const char* path)
{
    if (!path) return "application/octet-stream";
    const char* dot = strrchr(path, '.');
    if (!dot) return "application/octet-stream";

    for (size_t i = 0; TABLE[i].ext; i++) {
        if (strcasecmp(dot, TABLE[i].ext) == 0) return TABLE[i].mime;
    }
    return "application/octet-stream";
}
