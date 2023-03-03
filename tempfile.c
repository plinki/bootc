#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include "tempfile.h"

void generate_temp_path(char* path) {
    strcpy(path, "/tmp/tempfileXXXXXX");
}

TemporaryFile* create_tempfile() {
    TemporaryFile* tf = malloc(sizeof(TemporaryFile));
    if (!tf) {
        return NULL;
    }

    generate_temp_path(tf-> path);
    tf->fd = mkstemp(tf->path);
    if (tf->fd == -1) {
        free(tf);
        return NULL;
    }

    return tf;
}

void destroy_tempfile(TemporaryFile* tf) {
    if (tf) {
        close(tf->fd);
        unlink(tf->path);
        free(tf);
    }
}

const char* tempfile_path(TemporaryFile* tf) {
    return tf ? tf->path : NULL;
}

