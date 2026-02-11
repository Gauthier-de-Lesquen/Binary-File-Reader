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
    int help;
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
    Editor a = {0, 0, 0, 0, 0};

    for(int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-x") == 0 || strcmp(argv[i], "--hexadecimal") == 0) {
            a.hex = 1;
        }
        else if (strcmp(argv[i], "-b") == 0 || (strcmp(argv[i], "--binary") == 0)) {
            a.bin = 1;
        }
        else if (strcmp(argv[i], "-d") == 0 || (strcmp(argv[i], "--decimal") == 0)) {
            a.dec = 1;
        }
        else if (strcmp(argv[i], "-ha") == 0 || (strcmp(argv[i], "--hide-ascii") == 0)) {
            a.hid = 1;
        }
        else if (strcmp(argv[i], "-h") == 0 || (strcmp(argv[i], "--help") == 0)) {
            a.help = 1;
            printf("use the Binary File Reader like: [name you chose by compiling] <options> <file>\n");
            printf("The options can be :\n");
            printf("    -h or --help                      display the possible commands\n");
            printf("    -x or --hexadecimal               display the bytes in hexadecimal format\n");
            printf("    -b or --binary                    display the bytes in binary format\n");
            printf("    -d or --decimal                   display the bytes in decimal format\n");
            printf("    -u <number> or --until <number>   display all the bytes before the byte at the specified index\n");
            printf("    -ul <num> or --until-line <num>   display all the lines before the line at the specified index\n");
            printf("    -f(--from) <num> -t(--to) <num>   display all the bytes fom the fist index to the second\n");
            printf("    -ha or --hide-ascii               display the bytes without their ascii corespondants\n");
            exit(0);
        }
        else if (strcmp(argv[i], "-u") == 0 || (strcmp(argv[i], "--until") == 0)) {

            if (i + 1 >= argc || !is_number(argv[i+1])) {
                printf("Error: expecting valid integer after -u\n");
                exit(1);
            }

            a.lim = (int)strtol(argv[i+1], NULL, 10);
            i++;
        }
        else if (strcmp(argv[i], "-ul") == 0 || (strcmp(argv[i], "--until-line") == 0)) {

            if (i + 1 >= argc || !is_number(argv[i+1])) {
                printf("Error: expecting valid integer after -ul\n");
                exit(1);
            }

            a.linelim = (int)strtol(argv[i+1], NULL, 10);
            i++;
        }
        else if (strcmp(argv[i], "-f") == 0 || (strcmp(argv[i], "--from") == 0)) {

            if (i + 1 >= argc || !is_number(argv[i+1])) {
                printf("Error: expecting valid integer after -f\n");
                exit(1);
            }

            a.from = (int)strtol(argv[i+1], NULL, 10);
            i++;
        }
        else if (strcmp(argv[i], "-t") == 0 || (strcmp(argv[i], "--to") == 0)) {

            if (i + 1 >= argc || !is_number(argv[i+1])) {
                printf("Error: expecting valid integer after -t\n");
                exit(1);
            }

            a.to = (int)strtol(argv[i+1], NULL, 10);
            i++;
        }         
        else if (argv[i][0] != '-' && strchr(argv[i], '.') != NULL) {
            strcpy(a.file, argv[i]);
        }
        else {
            printf("Syntax Error : invalid argument\n");
            printf("Type  rb -h  to see the valid arguments");
            exit(1);
        }

    }

    if (!a.from && a.to || a.from && !a.to) {
        printf("Syntax Error : --from (-f) needs to be with --to (-t)\n");
        exit(1); 
    }

    if (a.file[0] == '\0') {
        printf("Syntax Error : you must specify a valid file name\n");
        exit(1);
    }

    int verif = a.hex + a.bin + a.dec;
    if (verif > 1) {
        printf("Syntax Error : you must select only one byte display type");
        exit(1);
    }
    if (verif == 0) {
        a.hex = 1;
    }

    FILE *file = fopen(a.file, "rb");
    if (file == NULL) {
        printf("Error: unable to read the specified file");
        exit(1);
    }
    unsigned char buffer[16];
    int count = 0;
    int read;
    int bytesPerLine = (a.hex || a.dec) ? 16 : 7;

    while ((read = fread(buffer, 1, bytesPerLine, file)) > 0) {
        for (int i = 0; i < read; i++) {
            int globalIndex = count + i;

            if (a.from && globalIndex < a.from) continue;
            if (a.to && globalIndex > a.to) break;

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


         // Si l'option -h n'est pas activée, afficher l'ASCII à droite
         if (!a.hid) {
            int padding = bytesPerLine - read;
            for (int p = 0; p < padding; p++) {
                if (a.hex) printf("   ");
                else if (a.dec) printf("    ");
                else if (a.bin) printf("         ");
            }

            printf("  ");
            for (int i = 0; i < read; i++) {
                int globalIndex = count + i;

                if (a.from && globalIndex < a.from) continue;
                if (a.to && globalIndex > a.to) break;

                unsigned char byte = buffer[i];
                if (isprint(byte)) {
                    printf("%c", byte);
                } else {
                    printf(".");
                }
            }
            printf(" ");
        }

        printf("\n");

        count += read;
        if (a.lim != 0 && count >= a.lim || a.to != 0 && count >= a.to) {
            break;
        }
        if (a.linelim != 0 && (count/bytesPerLine) >= a.linelim) {
            break;
        }
    }

    fclose(file);
    return 0;
}
