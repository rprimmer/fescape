#include "fescape.h"

#include <errno.h>
#include <getopt.h>
#include <stdlib.h>
#include <string.h>

static void usage(FILE *output, const char *program) {
    fprintf(output,
            "Usage: %s [OPTIONS] [FILE ...]\n\n"
            "Escape non-printable bytes; read stdin when FILE is omitted or '-'.\n\n"
            "  -h, --help       Display this help and exit\n"
            "  -r, --repeats    Collapse repeated escaped bytes into <code>[count]\n"
            "  -n, --newline    Escape newlines too (otherwise preserve them)\n"
            "  -o, --octal      Use octal instead of hexadecimal escapes\n\n"
            "Use -- before filenames that begin with '-'.\n"
            "Examples:\n"
            "  %s -rn binary.dat\n"
            "  %s file1 - file2\n", program, program, program);
}

static void report_error(const char *program, const char *source) {
    int error = errno;
    fprintf(stderr, "%s: %s: %s\n", program, source,
            strerror(error ? error : EIO));
}

static int filter_input(const char *program, const char *name,
                        const struct fescape_options *options) {
    bool is_stdin = strcmp(name, "-") == 0;
    const char *label = is_stdin ? "standard input" : name;
    FILE *input = is_stdin ? stdin : fopen(name, "rb");
    if (input == NULL) {
        report_error(program, label);
        return EXIT_FAILURE;
    }

    int status = EXIT_SUCCESS;
    if (fescape(input, stdout, options) < 0) {
        report_error(program, ferror(stdout) ? "standard output" : label);
        status = EXIT_FAILURE;
    }
    if (!is_stdin && fclose(input) == EOF) {
        report_error(program, label);
        status = EXIT_FAILURE;
    }
    return status;
}

int main(int argc, char **argv) {
    const char *program = strrchr(argv[0], '/');
    program = program ? program + 1 : argv[0];
    struct fescape_options options = {false, false, false};
    static const struct option long_options[] = {
        {"help", no_argument, NULL, 'h'},
        {"repeats", no_argument, NULL, 'r'},
        {"newline", no_argument, NULL, 'n'},
        {"octal", no_argument, NULL, 'o'},
        {NULL, 0, NULL, 0}
    };

    int option;
    opterr = 0;
    while ((option = getopt_long(argc, argv, "+hrno", long_options, NULL)) != -1) {
        switch (option) {
        case 'h':
            usage(stdout, program);
            return fflush(stdout) == EOF ? EXIT_FAILURE : EXIT_SUCCESS;
        case 'r': options.repeats = true; break;
        case 'n': options.filter_newlines = true; break;
        case 'o': options.octal = true; break;
        default:
            fprintf(stderr, "%s: invalid option; use --help for usage\n", program);
            return EXIT_FAILURE;
        }
    }

    int status = EXIT_SUCCESS;
    if (optind == argc)
        status = filter_input(program, "-", &options);
    else {
        for (; optind < argc; ++optind) {
            if (filter_input(program, argv[optind], &options) != EXIT_SUCCESS)
                status = EXIT_FAILURE;
            if (ferror(stdout))
                break;
        }
    }
    if (fflush(stdout) == EOF) {
        report_error(program, "standard output");
        status = EXIT_FAILURE;
    }
    return status;
}
