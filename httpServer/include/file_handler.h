#include "need.h"
#ifndef FILE_HANDLER_H
#define FILE_HANDLER_H



void serve_file(int, const char *);
void cat(int, FILE *);
int get_line(int, char *, int);
const char *get_mime_type(const char *);

#endif // FILE_HANDLER_H
