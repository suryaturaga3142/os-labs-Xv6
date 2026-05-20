#include "kernel/types.h"
#include "kernel/stat.h"
#include "kernel/fcntl.h"
#include "user/user.h"

int main (int argc, char *argv[]) {
    if (argc <= 1) {
        fprintf(2, "Usage: sixfive <file>\n");
        exit (1);
    }
    for (int f = 1; f < argc; f++) {
        int fd;
        if ((fd = open(argv[f], O_RDONLY)) < 0) {
            fprintf(2, "sixfive: cannot open %s\n", argv[f]);
            exit(1);
        }

        char key;
        char buf[512];
        int pos = 0;

        while (read(fd, &key, 1) > 0) {
            bool matched = false;
            switch (key) {
                case '-':
                case '.':
                case ',':
                case '/':
                case '\r':
                case '\t':
                case '\n':
                    if (pos != 0) {
                        buf[pos] = '\0';
                        int value = atoi(buf);
                        if (value % 5 == 0 || value % 6 == 0) {
                            write (1, buf, pos);
                            write (1, "\n", 1);
                        }
                        pos = 0;
                    }
                    matched = true;
                    break;
                default:
                    break;
            }
            if (key >= '0' && key <= '9') {
                if (key != '0' || pos != 0) buf[pos++] = key;
            }
            else if (!matched) pos = 0;
        }
        if (pos != 0) {
            buf[pos] = '\0';
            int value = atoi(buf);
            if (value % 5 == 0 || value % 6 == 0) {
                write (1, buf, pos);
                write (1, "\n", 1);
            }
            pos = 0;
        }
    }

    exit (0);
}

