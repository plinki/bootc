#ifndef TEMPFILE_H_
#define TEMPFILE_H_

#include <stdio.h>
#include <stdlib.h>

typedef struct TemporaryFile {
    char path[1024];
    int fd;
} TemporaryFile;

void generate_temp_path(char* path);
TemporaryFile* create_tempfile();
void destroy_tempfile(TemporaryFile* tf);
const char* tempfile_path(TemporaryFile* tf);

#endif

