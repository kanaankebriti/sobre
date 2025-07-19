/**
 * @file json_utils.h
 * @brief Utility functions for reading text-based files like JSON.
 */

#ifndef JSON_UTILS_H
#define JSON_UTILS_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Loads an entire file into a null-terminated string.
 *
 * This function reads the contents of a text file into dynamically allocated memory.
 * It guarantees null-termination and is suitable for parsing text formats like JSON.
 *
 * @param filename The path to the file to load.
 * @return A pointer to a heap-allocated null-terminated string containing the file contents,
 *         or NULL if an error occurred (file not found, read error, or allocation failure).
 *
 * @note The caller is responsible for freeing the returned memory using free().
 */
char *load_file_as_string(const char *filename);

#ifdef __cplusplus
}
#endif

#endif // JSON_UTILS_H
