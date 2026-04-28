#ifndef CTM_CORE_IO_H
#define CTM_CORE_IO_H

#define IO_BUFFER_SIZE 1024

typedef enum {
    IO_SUCCESS,
    IO_ERR_INVALID_ARGUMENT,
    IO_ERR_FILE_NOT_FOUND,
    IO_ERR_OPEN,
    IO_ERR_READ,
    IO_ERR_INTERNAL
} io_result_t;

const char* io_strerror(io_result_t result);

io_result_t io_read_file(const char* file_path,
                            char*       out);

#endif
