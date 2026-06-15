#include "kernel/types.h"
#include "kernel/fcntl.h"
#include "user/user.h"
#include "kernel/riscv.h"

int main(int argc, char *argv[])
{
    int size = 8 * 4096;
    char* buf = (char*) sbrk(size);

    char* cmp = "This may help.";

    for (char* cursor = buf; cursor < buf + size - 16; cursor++) {
        if (strcmp(cursor, cmp) == 0) {
            printf("%s\n", cursor + 16);
            exit(0);
        }
    }

    exit(1);
}

