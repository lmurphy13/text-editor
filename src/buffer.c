/*
 * Buffer Module
 * File: buffer.c
 * Author: Liam M. Murphy
 */

#include "buffer.h"

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Static prototypes
static void append_line_to_buffer(buffer_t *buffer, line_t *new_line, bool first);
static int count_digits(unsigned int num);
static void print_buffer(const buffer_t *const buffer);
static void renumber_lines(line_t *line);

// Allocate and initialize empty buffer
buffer_t *create_buffer(void) {
    buffer_t *retval = NULL;
    retval           = (buffer_t *)malloc(sizeof(buffer_t));

    if (retval != NULL) {
        memset(retval, 0, sizeof(buffer_t));
    }

    return retval;
}

line_t *create_line(const char *const str) {
    line_t *retval = NULL;

    if (str == NULL) {
        printf("ERROR: Unable to allocate line_t for empty string\n");
        return NULL;
    }

    retval = (line_t *)malloc(sizeof(line_t));
    if (retval != NULL) {
        memset(retval, 0, sizeof(line_t));

        // strlen(str) + 1 (newline) + 1 (null terminator)
        retval->text = (char *)malloc(strlen(str) + 2);
        if (retval->text == NULL) {
            printf("ERROR: Unable to allocate memory for line text\n");

            free(retval);
            return NULL;
        }

        snprintf(retval->text, strlen(str) + 2, "%s", str);
        retval->length = strlen(retval->text);
        retval->index  = 0;
        retval->dirty  = false;
        retval->prev   = NULL;
        retval->next   = NULL;
    }

    return retval;
}

void destroy_line(line_t *line) {
    if ((line != NULL) && (line->text != NULL)) {
        free(line->text);
        free(line);
    }
}

// Release memory within buffer
void destroy_buffer(buffer_t *buffer) {
    if (buffer == NULL) {
        printf("ERROR: Unable to free buffer since the pointer to it is NULL\n");
        return;
    }

    // Walk backwards up the buffer and free each line_t
    line_t *curr_line = buffer->last_line;
    while (curr_line != NULL) {
        line_t *prev = curr_line->prev;
        destroy_line(curr_line);
        curr_line = prev;
    }

    free(buffer);
}

// Copy the data from one buffer to another
void duplicate_buffer(const buffer_t *const in, buffer_t *out) {
    // TODO
    assert(0 && "Not yet implemented");
}

static void append_line_to_buffer(buffer_t *buffer, line_t *new_line, bool first) {
    if (first) {
        new_line->prev     = NULL;
        new_line->next     = NULL;
        new_line->index    = 1;
        buffer->first_line = new_line;
        buffer->last_line  = new_line;
    } else {
        // Append case

        // new_line->prev becomes the current last_line
        new_line->prev = buffer->last_line;

        // Increment line number
        new_line->index = buffer->last_line->index + 1;

        // current last_line->next becomes new_line
        buffer->last_line->next = new_line;

        // current last_line becomes new_line
        buffer->last_line = new_line;
    }
}

// Read the file, line by line, and populate buffer
void read_file_into_buffer(buffer_t *buffer, FILE *file) {
    // Read a line from file
    // Borrowed from: https://stackoverflow.com/a/3501681
    char *line         = NULL;
    size_t length      = 0;
    ssize_t read       = 0;
    bool is_first_line = true;

    while ((read = getline(&line, &length, file)) != -1) {
        line_t *new_line = create_line(line);

        // If the line read does not match the line stored within new_line, yell about it
        assert(strncmp(line, new_line->text, strlen(line)) == 0);

        if (is_first_line) {
            append_line_to_buffer(buffer, new_line, true);
            is_first_line = false;
        } else {
            append_line_to_buffer(buffer, new_line, false);
        }

        // Update the total buffer size line by line
        buffer->size += new_line->length;
    }
}

static void renumber_lines(line_t *line) {
    line_t *curr_line = line->next;
    while (curr_line != NULL) {
        // Set the current line's index to the previous line + 1
        curr_line->index = curr_line->prev->index + 1;
        curr_line        = curr_line->next;
    }
}

// Insert a line of text into file_buffer at a particular line number
void insert_line_into_buffer(buffer_t *buffer, unsigned int line_num, char *text) {
    if (line_num == 0) {
        printf("Cannot insert line at index 0\n");
        return;
    }

    if (line_num > (buffer->last_line->index + 1)) {
        printf("Cannot append more than 1 line beyond the end of the file. (file length: %d lines; "
               "requested line: %d)\n",
               buffer->last_line->index, line_num);
        return;
    }

    if (line_num == (buffer->last_line->index + 1)) {
        // Just append to last_line
        line_t *new_line = create_line(text);
        if (new_line != NULL) {
            new_line->prev          = buffer->last_line;
            new_line->next          = NULL;
            new_line->index         = line_num;
            buffer->last_line->next = new_line;

            buffer->size += new_line->length;
            return;
        } else {
            printf("ERROR: Unable to allocate new line\n");
            return;
        }
    }

    line_t *curr_line = buffer->first_line;
    while ((curr_line != NULL) && (curr_line->index < line_num)) {
        curr_line = curr_line->next;
    }

    // Make new line
    line_t *new_line = create_line(text);
    if (new_line != NULL) {
        // Insert it into the list
        new_line->prev        = curr_line->prev;
        new_line->next        = curr_line;
        new_line->index       = line_num;
        curr_line->prev->next = new_line;
        curr_line->prev       = new_line;

        buffer->size += new_line->length;
        renumber_lines(new_line);
    } else {
        printf("ERROR: Unable to allocate new line\n");
        return;
    }
}

void remove_line_from_buffer(buffer_t *buffer, unsigned int line_num) {
    if (line_num > buffer->last_line->index) {
        printf("ERROR: Cannot remove a line at an index beyond the end of the file. (file length: "
               "%d lines; requested line: %d)\n",
               buffer->last_line->index, line_num);
        return;
    }

    line_t *curr_line = buffer->first_line;
    while ((curr_line != NULL) && (curr_line->index < line_num)) {
        curr_line = curr_line->next;
    }

    // We are at line_num. Delete line
    if (curr_line != NULL) {
        curr_line->prev->next = curr_line->next;
        curr_line->next->prev = curr_line->prev;
        buffer->size -= curr_line->length;
        destroy_line(curr_line);
    }
}

static int count_digits(unsigned int num) {
    int count = 0;
    do {
        num /= 10;
        count++;
    } while (num != 0);

    return count;
}

static void print_buffer(const buffer_t *const buffer) {
    line_t *curr_line      = buffer->first_line;
    const int final_lineno = count_digits(buffer->last_line->index);

    while (curr_line != NULL) {
        // %*d lets us pre-define the maximum number of digits within the integer
        printf("%*d %s", final_lineno, curr_line->index, curr_line->text);
        curr_line = curr_line->next;
    }
}

// Output the contents of the buffer to the screen
void render_buffer(const buffer_t *const buffer) {
    // TODO
    print_buffer(buffer);
}

void write_buffer_to_file(const buffer_t *const buffer, const char *const filename) {
    FILE *new_file = fopen(filename, "w+");
    if (new_file != NULL) {
        line_t *curr_line = buffer->first_line;
        while (curr_line != NULL) {
            fwrite(curr_line->text, 1, strlen(curr_line->text), new_file);
            curr_line = curr_line->next;
        }

        fclose(new_file);
    } else {
        printf("ERROR: Unable to open file 'filename' for writing");
    }
}
