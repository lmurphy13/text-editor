/*
 * Editor Main Entry
 * File: main.c
 * Author: Liam M. Murphy
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "buffer.h"

// fp is checked by caller
size_t get_filesize(FILE *fp) {
    size_t size = 0;

    fseek(fp, 0L, SEEK_END);
    size = ftell(fp);
    fseek(fp, 0L, SEEK_SET);

    return size;
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        return 1;
    }

    const char *const filename = argv[1];
    
    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        printf("ERROR: Unable to open file '%s'\n", filename);
        return 1;
    }

    printf("Opened file %s\n", filename);
    printf("File size is %d bytes\n", (unsigned int)get_filesize(file));

    buffer_t *file_buffer = create_buffer();
    if (file_buffer != NULL) {
        read_file_into_buffer(file_buffer, file);
        render_buffer(file_buffer);

        insert_line_into_buffer(file_buffer, 100, "LIAM WAS HERE\n");

        render_buffer(file_buffer);

        write_buffer_to_file(file_buffer, "newfile.out");
        destroy_buffer(file_buffer);
    }

    fclose(file);

    return 0;
}
