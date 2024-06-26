#include <getopt.h>
#include <libgen.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "fescape.h"
#include "system-actions.h"

int main(int argc, char **argv) { 
    char program[PATH_MAX];
    basename_r(argv[0], program);
    FILE *fp;
    bool repeat_count = false;
    bool show_octal = false;
    bool filter_newlines = false;

#ifdef DEBUG
    setbuf(stdout, NULL); 
    setbuf(stderr, NULL);
    fprintf(stderr, "%s, %d: argc: %d, optind: %d\n", basename(__FILE__), __LINE__, argc, optind);
#endif // DEBUG

    // Handle switches
    int option = 0;
    int option_index = 0;
    static struct option long_options[] = {{"help", no_argument, 0, 'h'},
                                           {"repeats", no_argument, 0, 'r'},
                                           {"newline", no_argument, 0, 'n'},
                                           {"octal", no_argument, 0, 'o'},
                                           {0, 0, 0, 0}};
    while ((option = getopt_long(argc, argv, "hrno", long_options, &option_index)) != -1) {
        switch (option) {
        case 'h':
            usage(program);
            break;
        case 'r':
            repeat_count = true;
            break;
        case 'n':
            filter_newlines = true;
            break;
        case 'o':
            show_octal = true;
            break;
        default:
            HANDLE_ERROR("invalid switch provided");
        }
    }

#ifdef DEBUG
    fprintf(stderr, "%s, %d: argc: %d, optind: %d\n", basename(__FILE__), __LINE__, argc, optind);
#endif // DEBUG

    // Handle arguments and actions
    int retval = 0;

    if (optind >= argc)
        fescape(stdin, stdout, repeat_count, show_octal, filter_newlines);
    else
        for (; optind < argc; optind++) {
            if (strcmp(argv[optind], "-") == 0)
                fescape(stdin, stdout, repeat_count, show_octal, filter_newlines);
            else {
                if ((fp = fopen(argv[optind], "r")) == NULL) 
                    HANDLE_ERROR("fopen: %s, file: %s", strerror(errno), argv[optind]);

                fescape(fp, stdout, repeat_count, show_octal, filter_newlines);
                fprintf(stdout, "\n");
                fclose(fp);
            } 
        } 

    return ferror(stdout) ? EOF : retval;
} 
