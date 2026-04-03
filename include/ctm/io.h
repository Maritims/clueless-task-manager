/**
 * @file cio.h
 * @author Martin Severin Steffensen
 * @date 02 April 2026
 * @brief IO API.
 */

#ifndef CIO_H
#define CIO_H

/**
 * @brief Reads a file into a buffer.
 * @param path The file path.
 * @param out_size A pointer to where the buffer's final size will be stored.
 * @return A buffer containing the contents of the file.
 */
char* cio_read_file(const char* path, size_t* out_size);

#endif //CIO_H
