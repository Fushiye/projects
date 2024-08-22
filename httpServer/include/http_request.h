#include "file_handler.h"
#include "cgi_handler.h"
#include "need.h"

#ifndef HTTP_REQUEST_H
#define HTTP_REQUEST_H

void accept_request(int);
void headers(int, const char *);
void bad_request(int);
void not_found(int);
void cannot_execute(int);
void unimplemented(int);

#endif // HTTP_REQUEST_H

