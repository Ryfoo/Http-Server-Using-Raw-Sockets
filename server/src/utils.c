#include "../include/utils.h"
#include <strings.h>
#include <time.h>
#include <stdio.h>


void load_env(const char *filename)
{
    FILE *file = fopen(filename, "r");
    if (!file) {
        perror("Could not open .env file");
        return;
    }

    char line[256];
    while (fgets(line, sizeof(line), file)) {
        if (line[0] == '#' || line[0] == '\n') continue;
        line[strcspn(line, "\n")] = 0;

        char *eq = strchr(line, '=');
        if (!eq) continue;

        *eq = '\0';
        char *key   = line;
        char *value = eq + 1;
        setenv(key, value, 1);
    }

    fclose(file);
}


char* load_file(const char* filename, long* out_size)
{
    FILE* f = fopen(filename, "rb");
    if (!f) return NULL;

    if (fseek(f, 0, SEEK_END) != 0) { fclose(f); return NULL; }
    long size = ftell(f);
    if (size < 0) { fclose(f); return NULL; }
    rewind(f);

    char* buffer = malloc((size_t)size + 1);
    if (!buffer) { fclose(f); return NULL; }

    size_t bytes_read = fread(buffer, 1, (size_t)size, f);
    buffer[bytes_read] = '\0';
    fclose(f);

    if (out_size) *out_size = (long)bytes_read;
    return buffer;
}


const char* header_get(const headers_list_t* hl, const char* name)
{
    if (!hl || !name) return NULL;
    for (size_t i = 0; i < hl->headers_counter; i++) {
        if (strcasecmp(hl->headers[i].name, name) == 0) {
            return hl->headers[i].value;
        }
    }
    return NULL;
}


void header_set(headers_list_t* hl, const char* name, const char* value)
{
    if (!hl || !name || !value) return;
    if (hl->headers_counter >= HEADERS_LEN) return;
    header_t* h = &hl->headers[hl->headers_counter++];
    strncpy(h->name,  name,  HEADER_NAME_LENGTH  - 1);
    h->name[HEADER_NAME_LENGTH - 1] = '\0';
    strncpy(h->value, value, HEADER_VALUE_LENGTH - 1);
    h->value[HEADER_VALUE_LENGTH - 1] = '\0';
}


void http_date_now(char* buffer, size_t buffer_size)
{
    if (!buffer || buffer_size < 30) return;
    time_t now = time(NULL);
    struct tm gmt;
    gmtime_r(&now, &gmt);
    /* IMF-fixdate, e.g. "Sun, 06 Nov 1994 08:49:37 GMT" */
    strftime(buffer, buffer_size, "%a, %d %b %Y %H:%M:%S GMT", &gmt);
}
