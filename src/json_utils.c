/**
 * @file json_utils.c
 * @brief Implementation of utility functions for reading files into memory.
 */

#include <stdio.h>
#include <stdlib.h>
#include "json_utils.h"

 /**
  * @brief Loads the entire content of a file as a null-terminated string.
  *
  * Internally opens the file in binary mode ("rb"), seeks to determine the file length,
  * allocates a buffer of size (length + 1) to store the data plus null-terminator,
  * reads the contents, ensures null-termination, and closes the file.
  *
  * @param filename Path to the file to be read.
  * @return Pointer to a null-terminated string on success, or NULL on failure.
  */
char *load_file_as_string(const char *filename)
{
	FILE *fp = fopen(filename, "rb");  // Open file in binary mode to avoid newline translation
	if (!fp)
	{
		// File could not be opened (e.g., doesn't exist, no permissions)
		return NULL;
	}

	// Move file pointer to the end to determine file size
	if (fseek(fp, 0, SEEK_END) != 0)
	{
		fclose(fp);
		return NULL;
	}

	long int length = ftell(fp);  // Get file size in bytes
	if (length < 0)
	{
		fclose(fp);
		return NULL;
	}

	rewind(fp);  // Reset file pointer to beginning

	// Allocate buffer (+1 for the null terminator)
	char *buffer = (char *)malloc((size_t)length + 1);
	if (!buffer)
	{
		fclose(fp);
		return NULL;  // Allocation failed
	}

	// Read the full file into buffer
	size_t bytes_read = fread(buffer, 1, (size_t)length, fp);
	if ((long)bytes_read != length)
	{
		// Could not read entire file; clean up
		free(buffer);
		fclose(fp);
		return NULL;
	}

	buffer[length] = '\0';  // Ensure null termination for string safety

	fclose(fp);  // Close the file

	return buffer;  // Return the dynamically allocated string
}
