/**
 * @file fescape.h
 * @author Robert Primmer (https://github.com/rprimmer)
 * @brief Filter unprintable characters from input stream.
 * @details Files that contain non-printable characters mess up the display when printed (e.g., via cat(1)).
 * This program allows the display of such files, substituting hex (or optionally octal) codes for the
 * non-printable characters. Optionally it can show the count for repeated non-printable characters
 * rather than display each repeated hex/octal code.
 * 
 * @version 1.2
 * @date 2024-04-08
 */

#pragma once 

#include <ctype.h>
#include <stdio.h> 
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

/**
 * @brief Display help to user
 * 
 * @param program Calling program name
 */
void usage(const char *program); 

/**
 * @brief convert non-ASCII characters to hex or octal representation
 * 
 * @param input_stream Input stream to read.
 * @param output_stream Output stream to write.
 * @param repeats If true, display repeated character count. 
 * @param octal If true, display control sequences in octal instead of hex. 
 * @param filter_newlines If true, filter newline characters.
 */
void fescape(FILE *input_stream, FILE *output_stream, bool repeats, bool octal, bool filter_newlines);
