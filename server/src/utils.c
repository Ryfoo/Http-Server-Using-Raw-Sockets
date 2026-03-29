#include "../include/utils.h"

void load_env(const char *filename) 
{
    FILE *file = fopen(filename, "r");
    if (!file) {
        perror("Could not open .env file");
        return;
    }

    char line[256];
    while (fgets(line, sizeof(line), file)) {
        // Skip comments and empty lines
        if (line[0] == '#' || line[0] == '\n') continue;

        // Remove trailing newline
        line[strcspn(line, "\n")] = 0;

        // Split on '='
        char *eq = strchr(line, '=');
        if (!eq) continue;

        *eq = '\0';              // split into key and value
        char *key   = line;
        char *value = eq + 1;

        if(!setenv(key, value, 1)) continue;  // load into process environment
    }

    fclose(file);
}




char* load_file(const char* filename, long* out_size) {
    FILE* f = fopen(filename, "rb"); // "rb" is safer for cross-platform binary/text
    if (f == NULL) return NULL;

    // 1. Find file size
    fseek(f, 0, SEEK_END);
    long size = ftell(f);
    rewind(f);

    // 2. Allocate memory (+1 for null terminator if serving as a string)
    char* buffer = malloc(size + 1);
    if (buffer == NULL) {
        fclose(f);
        return NULL;
    }

    // 3. Read the file into the buffer
    size_t bytes_read = fread(buffer, 1, size, f);
    buffer[bytes_read] = '\0'; // Null terminate

    fclose(f);
    
    if (out_size) *out_size = (long)bytes_read;
    return buffer;
}