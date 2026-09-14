/** @file fescape.h
 * Convert non-printable bytes to hexadecimal or octal escapes.
 */
#ifndef FESCAPE_H
#define FESCAPE_H

#include <stdbool.h>
#include <stdio.h>

/** Output options for the byte filter. */
struct fescape_options {
    bool repeats; /**< Collapse repeated escaped bytes with a total count in brackets. */
    bool octal; /**< Use three-digit octal instead of hexadecimal. */
    bool filter_newlines; /**< Escape LF bytes instead of passing them through. */
};

/**
 * Filter a stream using printable ASCII (0x20 through 0x7e).
 * Return 0 on success, or -1 with errno set on a read or write failure.
 * Streams remain owned by the caller; output is not flushed or closed here.
 * Repeat counts are local to this call and include the first byte of each run.
 */
int fescape(FILE *input, FILE *output, const struct fescape_options *options);

#endif
