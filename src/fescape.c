#include "fescape.h"
#include "system-actions.h"

void usage(const char *program) {
    printf("Usage: %s [OPTIONS] <ARGUMENTS>\n\n", program);
    printf("Options:\n");
    printf("  -h, --help        Display this help message and exit\n");
    printf("  -r, --repeats     Show repeated non-ASCII chars in brackets\n");
    printf("  -o, --octal       Display non-ASCII characters in octal instead of hex\n\n");
    printf("Arguments:\n");
    printf("  filename(s)       filename(s) to display\n");
    printf("  -                 streams from stdin\n");
    printf("  no argument       equivalent to -, streams from stdin\n\n");
    printf("Examples:\n");
    printf("  %s\n", program);
    printf("  %s -\n", program);
    printf("  %s MyBinaryFile\n", program);
    printf("  %s MyBinaryFile1 MyBinaryFile2 MyBinaryFile3\n\n", program);
    // printf("Restrictions:\n");
    // printf("  None.\n\n");
    // printf("Notes:\n");
    // printf("  None.\n");
    exit(EXIT_SUCCESS);
} // usage()

void fescape(FILE *input_stream, FILE *output_stream, bool repeats, bool octal) {
    int current_char;
    int saved_char = EOF;
    int repeat_count = 1;

    while ((current_char = getc(input_stream)) != EOF) {
        if (ferror(input_stream)) {
            fclose(input_stream);
            HANDLE_ERROR("unable to read input stream");
        }

        if (iscntrl(current_char) || !isprint(current_char)) { 
            if (current_char == saved_char && repeats) {
                repeat_count++;
            } else {
                if (repeat_count > 1 && repeats) { // Handle repeat printing.
                    fprintf(output_stream, "[%i]", repeat_count);
                    repeat_count = 1;
                }
                saved_char = current_char;
                fprintf(output_stream, octal ? "<%.3o>" : "<0x%02x>", current_char);
            }
        } else {
            if (repeat_count > 1 && repeats) { // Final repeat count print for control sequences.
                fprintf(output_stream, "[%i]", repeat_count);
                repeat_count = 1;
            }
            putc(current_char, output_stream); // Print printable characters directly.
            saved_char = EOF; // Reset saved_char since we're out of a repeat or control sequence.
        }
    }
    if (repeat_count > 1 && repeats) { // Handle the case for the last character being repeated.
        fprintf(output_stream, "[%i]", repeat_count);
    }
}
