/*
 * Editor Module Header
 * File: editor.h
 * Author: Liam M. Murphy
 */

#ifndef EDITOR_H
#define EDITOR_H

#define MAX_BUFFERS 4

typedef struct editor_s {
    buffer_t buffers[MAX_BUFFERS];
} editor_t;

#endif // EDITOR_H
