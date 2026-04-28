#include "io.h"

#include <errno.h>
#include <stddef.h>
#include <stdio.h>

const char* io_strerror(const io_result_t result)
{
    switch (result) {
        case IO_SUCCESS:
            return "Success";
        case IO_ERR_FILE_NOT_FOUND:
            return "File not found";
        case IO_ERR_INVALID_ARGUMENT:
            return "Invalid argument";
        case IO_ERR_OPEN:
            return "Failed to open file";
        case IO_ERR_READ:
            return "Failed to read file";
        case IO_ERR_INTERNAL:
            return "Internal error";
    }
    return "Unknown error";
}

io_result_t io_read_file(const char* file_path,
                         char*       out)
{
    FILE*  fp;
    size_t bytes_read;

    if (file_path == NULL || out == NULL) {
        return IO_ERR_INVALID_ARGUMENT;
    }

    if ((fp = fopen(file_path, "r")) == NULL) {
        return IO_ERR_OPEN;
    }

    errno      = 0;
    bytes_read = fread(out, 1, IO_BUFFER_SIZE - 1, fp);

    if (ferror(fp)) {
        fclose(fp);
        return IO_ERR_INTERNAL;
    }

    out[bytes_read] = '\0';
    fclose(fp);

    return IO_SUCCESS;
}
