#include "fescape.h"

#include <errno.h>
#include <stdint.h>

static int write_run(FILE *output, int byte, size_t count,
                     const struct fescape_options *options) {
    if ((byte >= 0x20 && byte <= 0x7e) ||
        (byte == '\n' && !options->filter_newlines))
        return fputc(byte, output) == EOF ? -1 : 0;

    if (fprintf(output, options->octal ? "<%03o>" : "<0x%02x>",
                (unsigned int)byte) < 0)
        return -1;
    if (count > 1 && fprintf(output, "[%zu]", count) < 0)
        return -1;
    return 0;
}

int fescape(FILE *input, FILE *output, const struct fescape_options *options) {
    int saved = EOF;
    size_t count = 0;
    int byte;

    errno = 0;
    while ((byte = fgetc(input)) != EOF) {
        bool escaped = (byte < 0x20 || byte > 0x7e) &&
                       (byte != '\n' || options->filter_newlines);
        if (options->repeats && escaped && byte == saved && count < SIZE_MAX) {
            ++count;
            continue;
        }
        if (saved != EOF && write_run(output, saved, count, options) < 0)
            goto failure;
        saved = byte;
        count = 1;
    }

    if (ferror(input))
        goto failure;
    if (saved != EOF && write_run(output, saved, count, options) < 0)
        goto failure;
    return 0;

failure:
    if (errno == 0)
        errno = EIO;
    return -1;
}
