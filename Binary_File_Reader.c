#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

typedef struct {
    int hex;
    int bin;
    int dec;
    int hid;
    int lim;
    int linelim;
    int from;
    int to;
    char file[256];
} Editor;

int is_number(char *s) {
    char *end;
    strtol(s, &end, 10);  // base 10
    return *s != '\0' && *end == '\0';
}

int main(int argc, char *argv[]) {
    Editor a = {0, 0, 0, 0, 0, 0, -1, -1};

    for(int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-x") == 0 || strcmp(argv[i], "--hexadecimal") == 0)
            a.hex = 1;
        else if (strcmp(argv[i], "-b") == 0 || (strcmp(argv[i], "--binary") == 0))
            a.bin = 1;
        else if (strcmp(argv[i], "-d") == 0 || (strcmp(argv[i], "--decimal") == 0))
            a.dec = 1;
        else if (strcmp(argv[i], "-ha") == 0 || (strcmp(argv[i], "--hide-ascii") == 0))
            a.hid = 1;
        else if (strcmp(argv[i], "-h") == 0 || (strcmp(argv[i], "--help") == 0)) {
            printf("use the Binary File Reader like: rb <options> <file>\n");
            printf("The options can be :\n");
            printf("    -h or --help                      display the possible commands\n");
            printf("    -x or --hexadecimal               display the bytes in hexadecimal format\n");
            printf("    -b or --binary                    display the bytes in binary format\n");
            printf("    -d or --decimal                   display the bytes in decimal format\n");
            printf("    -u <number> or --until <number>   display all the bytes before the byte at the specified index\n");
            printf("    -ul <num> or --until-line <num>   display all the lines before the line at the specified index\n");
            printf("    -f(--from) <num> -t(--to) <num>   display all the bytes fom the fist index to the second\n");
            printf("    -ha or --hide-ascii               display the bytes without their ascii corespondants\n");
	        printf("    -v or --version                   display the version of rb\n");
            exit(0);
        }
	    else if (strcmp(argv[i], "-v") == 0 || (strcmp(argv[i], "--version") == 0)) {
	        printf("rb version 1.1.0\n");
	        exit(0);
	    }
        else if (strcmp(argv[i], "-u") == 0 || (strcmp(argv[i], "--until") == 0)) {

            if (i + 1 >= argc || !is_number(argv[i+1])) { //check if -u is followed by a number
                printf("rb: Type Error: expecting valid integer after -u\n");
                exit(2);
            }

            a.lim = (int)strtol(argv[i+1], NULL, 10);
            if (a.lim < 0) { //check if -u is followed by a negative number
                printf("rb: Syntax Error : -u limit must be an unsigned integer\n");
                exit(2);
            }
            i++;
        }
        else if (strcmp(argv[i], "-ul") == 0 || (strcmp(argv[i], "--until-line") == 0)) {

            if (i + 1 >= argc || !is_number(argv[i+1])) {
                printf("rb: Type Error: expecting valid integer after -ul\n");
                exit(2);
            }

            a.linelim = (int)strtol(argv[i+1], NULL, 10);
            if (a.linelim < 0) {
                printf("rb: Syntax Error : -ul limit must be an unsigned integer\n");
                exit(2);
            }
            i++;
        }
        else if (strcmp(argv[i], "-f") == 0 || (strcmp(argv[i], "--from") == 0)) {

            if (i + 1 >= argc || !is_number(argv[i+1])) {
                printf("rb: Type Error: expecting valid integer after -f\n");
                exit(2);
            }

            a.from = (int)strtol(argv[i+1], NULL, 10);
            if (a.from < 0) {
                printf("rb: Syntax Error : -f limit must be an unsigned integer\n");
                exit(2);
            }
            i++;
        }
        else if (strcmp(argv[i], "-t") == 0 || (strcmp(argv[i], "--to") == 0)) {

            if (i + 1 >= argc || !is_number(argv[i+1])) {
                printf("rb: Type Error: expecting valid integer after -t\n");
                exit(2);
            }

            a.to = (int)strtol(argv[i+1], NULL, 10);
            if (a.to < 0) {
                printf("rb: Syntax Error : -t limit must be an unsigned integer\n");
                exit(2);
            }
            i++;
        }
        else if (argv[i][0] != '-') {
            snprintf(a.file, sizeof(a.file), "%s", argv[i]);
        }
        else {
            printf("rb: Syntax Error : invalid argument\n");
            printf("Type  rb -h  to see the valid arguments\n");
            exit(2);
        }

    }

    //errors
    if ((a.from == -1) != (a.to == -1)) {
        printf("rb: Syntax Error : --from (-f) needs to be with --to (-t)\n");
        exit(2);
    }

    if (a.file[0] == '\0') {
        printf("rb: Syntax Error : you must specify a valid file name\n");
        exit(2);
    }

    int verif = a.hex + a.bin + a.dec;
    if (verif > 1) {
        printf("rb: Syntax Error : you must select only one byte display type\n");
        exit(2);
    }
    if (verif == 0) {
        a.hex = 1;
    }

    // open file
    FILE *file = fopen(a.file, "rb" /* open file as binary file*/);
    if (file == NULL) {
        printf("rb: Fatal Error: unable to read the specified file\n");
        exit(1);
    }

    unsigned char buffer[16];
    int count = 0;
    int read;
    int bytesPerLine = (a.bin) ? 7 : 16; // binary bytes are longer than hexadecimal or decimal bytes

    int max_index = (a.lim != 0) ? a.lim : (a.to != -1) ? a.to + 1 : INT_MAX;
    int max_line  = (a.linelim != 0) ? a.linelim : INT_MAX;

    while ((read = fread(buffer, 1, bytesPerLine, file)) > 0) {
        int current_line = count / bytesPerLine;
        if (current_line >= max_line) break;

        int bytes_to_print = read;
        if (count + read > max_index) {
            bytes_to_print = max_index - count;
        }

        // display bytes
        for (int i = 0; i < bytes_to_print; i++) {
            int globalIndex = count + i;

            if (a.from != -1 && globalIndex < a.from) continue;

            unsigned char byte = buffer[i];
            if (a.hex) {
                printf("%02X ", byte);
            }
            else if (a.bin) {
                for (int j = 7; j >= 0; j--) {
                    printf("%d", (byte >> j) & 1);
                }
                printf(" ");
            }
            else if (a.dec) {
                printf("%3d ", byte);
            }
        }

        // ascii display
        if (!a.hid) {
            int padding = bytesPerLine - bytes_to_print;
            for (int p = 0; p < padding; p++) {
                if (a.hex) printf("   ");
                else if (a.dec) printf("    ");
                else if (a.bin) printf("         ");
            }

            printf("  ");
            for (int i = 0; i < bytes_to_print; i++) {
                int globalIndex = count + i;
                if (a.from != -1 && globalIndex < a.from) continue;

                unsigned char byte = buffer[i];
                printf("%c", isprint(byte) ? byte : '.');
            }
        }

        printf("\n");

        count += read;
        if (count >= max_index) break;
    }


    fclose(file);
    return 0;
}
