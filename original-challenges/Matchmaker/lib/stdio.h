/*
 * Copyright (C) Narf Industries <info@narfindustries.com>
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included
 * in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
 * OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
 * CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
 * TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
 * SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

#ifndef STDIO_H_
#define STDIO_H_

#include <libcgc.h>

#include "stdlib.h"

typedef struct cgc_FILE cgc_FILE;
extern cgc_FILE *stdin, *stdout, *stderr;

/** Is this file in read or write mode, mutually exclusive */
enum mode {
    /** Read mode */
    READ,
    /** Write mode */
    WRITE
};

/**
 * Read exactly size bytes from fd into ptr.
 *
 * @param fd The file descriptor to read from
 * @param ptr The output buffer
 * @param size The size to read
 * @return size on success, else EXIT_FAILURE
 */
cgc_ssize_t cgc_read_all(int fd, void *ptr, size_t size);

/**
 * Read at most size bytes from fd into ptr, stopping on delim.
 *
 * @param fd The file descriptor to read from
 * @param ptr The output buffer
 * @param delim The byte to stop on
 * @param size The size to read
 * @return size on success, else EXIT_FAILURE
 */
cgc_ssize_t cgc_read_until(int fd, void *ptr, unsigned char delim, size_t size);

/**
 * Write exactly size bytes from ptr to fd.
 *
 * @param fd The file descriptor to write to
 * @param ptr The input buffer
 * @param size The size to write
 * @return size on success, else EXIT_FAILURE
 */
cgc_ssize_t cgc_write_all(int fd, const void *ptr, size_t size);

/**
 * Open a cgc_FILE given a file descriptor and a mode.
 *
 * @param fd The file descriptor to open
 * @param mode The mode of the cgc_FILE
 * @return A pointer to a cgc_FILE or NULL on failure
 */
cgc_FILE *cgc_fdopen(int fd, enum mode mode);

/**
 * Close a cgc_FILE opened with cgc_fdopen.
 *
 * @param stream The cgc_FILE to close
 */
void cgc_fclose(cgc_FILE *stream);

/**
 * Flush buffer from cgc_FILE pointer to its fd.
 *
 * @param stream The cgc_FILE to flush
 * @return EXIT_SUCCESS on success, else EXIT_FAILURE
 */
int cgc_fflush(cgc_FILE *stream);

/**
 * Read exactly size bytes from cgc_FILE pointer into ptr, buffered.
 *
 * @param ptr The output buffer
 * @param size The size to read
 * @param stream The cgc_FILE pointer to read from
 * @return size on success, else EXIT_FAILURE
 */
cgc_ssize_t cgc_fread(void *ptr, size_t size, cgc_FILE *stream);

/**
 * Read at most size bytes from cgc_FILE pointer into ptr, stopping on delim, buffered.
 *
 * @param ptr The output buffer
 * @param delim The byte to stop on
 * @param size The size to read
 * @param stream The cgc_FILE pointer to read from
 * @return size on success, else EXIT_FAILURE
 */
cgc_ssize_t cgc_fread_until(void *ptr, unsigned char delim, size_t size, cgc_FILE *stream);

/**
 * Write size bytes from ptr into cgc_FILE pointer, buffered.
 *
 * @param ptr The input buffer
 * @param size The size to write
 * @param stream The cgc_FILE pointer to write to
 * @return size on success, else EXIT_FAILURE
 */
cgc_ssize_t cgc_fwrite(const void *ptr, size_t size, cgc_FILE *stream);

/**
 * Get a character from cgc_FILE pointer, buffered.
 *
 * @param stream The cgc_FILE pointer to read from
 * @return The value of the character read, or EXIT_FAILURE
 */
int cgc_fgetc(cgc_FILE *stream);

/**
 * Write a character to cgc_FILE pointer, buffered.
 *
 * @param stream The cgc_FILE pointer to write to
 * @return EXIT_SUCCESS on success, else EXIT_FAILURE
 */
int cgc_fputc(int character, cgc_FILE *stream);

/**
 * Get a character from stdin, buffered.
 *
 * @return The value of the character read, or EXIT_FAILURE
 */
#define getc() cgc_fgetc(stdin)

/**
 * Write a character to stdout, buffered.
 *
 * @return EXIT_SUCCESS on success, else EXIT_FAILURE
 */
#define putc(character) cgc_fputc(character, stdout)

/**
 * Print a formatted string to stdout, taking parameters from a cgc_va_list,
 *      buffered.
 *
 * @param format The format string to use
 * @param arg The cgc_va_list to retrieve values from
 * @return The number of bytes written on success, else EXIT_FAILURE
 */
int cgc_vprintf(const char *format, cgc_va_list arg);

/**
 * Print a formatted string to a cgc_FILE pointer, taking parameters from a cgc_va_list,
 *      buffered.
 *
 * @param stream The cgc_FILE pointer to write to
 * @param format The format string to use
 * @param arg The cgc_va_list to retrieve values from
 * @return The number of bytes written on success, else EXIT_FAILURE
 */
int cgc_vfprintf(cgc_FILE *stream, const char *format, cgc_va_list arg);

/**
 * Write a formatted string to a buffer, taking parameters from a cgc_va_list.
 *
 * @param s The buffer to write to
 * @param format The format string to use
 * @param arg The cgc_va_list to retrieve values from
 * @return The number of bytes written on success, else EXIT_FAILURE
 */
int cgc_vsprintf(char *s, const char *format, cgc_va_list arg);

/**
 * Write a formatted string to a buffer, writing at most num bytes, taking
 *      parameters from a cgc_va_list.
 *
 * @param s The buffer to write to
 * @param num The maximum number of bytes to write
 * @param format The format string to use
 * @param arg The cgc_va_list to retrieve values from
 * @return The number of bytes written on success, else EXIT_FAILURE
 */
int cgc_vsnprintf(char *s, size_t num, const char *format, cgc_va_list arg);

/**
 * Read formatted input from stdin, taking parameters from a cgc_va_list,
 *      buffered.
 *
 * UNIMPLEMENTED
 *
 * @param format The format string to use
 * @param arg The cgc_va_list to write values to
 * @return The number of bytes read on success, else EXIT_FAILURE
 */
int cgc_vscanf(const char *format, cgc_va_list arg);

/**
 * Read formatted input from a cgc_FILE pointer, taking parameters from a cgc_va_list,
 *      buffered.
 *
 * UNIMPLEMENTED
 *
 * @param stream The file pointer to read from
 * @param format The format string to use
 * @param arg The cgc_va_list to write values to
 * @return The number of bytes read on success, else EXIT_FAILURE
 */
int cgc_vfscanf(cgc_FILE *stream, const char *format, cgc_va_list arg);

/**
 * Read formatted input from a string, taking parameters from a cgc_va_list.
 *
 * UNIMPLEMENTED
 *
 * @param s The string to read from
 * @param format The format string to use
 * @param arg The cgc_va_list to write values to
 * @return The number of bytes read on success, else EXIT_FAILURE
 */
int cgc_vsscanf(char *s, const char *format, cgc_va_list arg);

/**
 * Print a formatted string to stdout, buffered.
 *
 * @param format The format string to use
 * @return The number of bytes written on success, else EXIT_FAILURE
 */
int cgc_printf(const char *format, ...);

/**
 * Print a formatted string to a cgc_FILE pointer, buffered.
 *
 * @param stream The cgc_FILE pointer to write to
 * @param format The format string to use
 * @return The number of bytes written on success, else EXIT_FAILURE
 */
int cgc_fprintf(cgc_FILE *stream, const char *format, ...);

/**
 * Write a formatted string to a buffer.
 *
 * @param s The buffer to write to
 * @param format The format string to use
 * @return The number of bytes written on success, else EXIT_FAILURE
 */
int cgc_sprintf(char *s, const char *format, ...);

/**
 * Write a formatted string to a buffer, writing at most num bytes.
 *
 * @param s The buffer to write to
 * @param num The maximum number of bytes to write
 * @param format The format string to use
 * @return The number of bytes written on success, else EXIT_FAILURE
 */
int cgc_snprintf(char *s, size_t num, const char *format, ...);

/**
 * Read formatted input from stdin, buffered.
 *
 * UNIMPLEMENTED
 *
 * @param format The format string to use
 * @return The number of bytes read on success, else EXIT_FAILURE
 */
int cgc_scanf(const char *format, ...);

/**
 * Read formatted input from a cgc_FILE pointer, buffered.
 *
 * UNIMPLEMENTED
 *
 * @param stream The cgc_FILE pointer to read from
 * @param format The format string to use
 * @return The number of bytes read on success, else EXIT_FAILURE
 */
int cgc_fscanf(const char *format, ...);

/**
 * Read formatted input from a string.
 *
 * UNIMPLEMENTED
 *
 * @param s The string to read from
 * @param format The format string to use
 * @return The number of bytes read on success, else EXIT_FAILURE
 */
int cgc_sscanf(char *s, const char *format, ...);

#endif /* STDIO_H_ */

