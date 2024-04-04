/*
 * Buffer Module Header
 * File: buffer.h
 * Author: Liam M. Murphy
 */

#ifndef BUFFER_H
#define BUFFER_H

#include <stdbool.h>
#include <stdio.h>
#include <unistd.h>

typedef struct line_s {
    char *text;             // Contents of a line
    size_t length;          // Length of the line (number of characters)
    unsigned int index;     // Line index (number)
    bool dirty;             // True if the line is modified. False otherwise
    struct line_s *prev;    // Pointer to previous line_t. NULL if this is the first line in the file.
    struct line_s *next;    // Pointer to next line_t. NULL if this is the last line in the file.
} line_t;

typedef struct buffer_s {
    line_t *first_line;     // Head of line_t linked list
    line_t *last_line;      // Most recently appended line_t
    size_t size;            // Total size of the buffer, in bytes
    bool dirty;             // Was the buffer modified? True if yes, else false
} buffer_t;

// Allocate and initialize empty buffer
buffer_t *create_buffer(void);

// Allocate and initialize a line_t
line_t *create_line(const char *const str);

// Release memory within buffer
void destroy_buffer(buffer_t *buffer);

// Copy the data from one buffer to another
void duplicate_buffer(const buffer_t *const in, buffer_t *out);

// Read the file, line by line, and populate buffer
void read_file_into_buffer(buffer_t *buffer, FILE *file);

// Insert a line of text into file_buffer at a particular line number
void insert_line_into_buffer(buffer_t *buffer, unsigned int line_num, char *text);

// Output the contents of the buffer to the screen
void render_buffer(const buffer_t *const buffer);

void write_buffer_to_file(const buffer_t *const buffer, const char *const filename);

#endif // BUFFER_H
