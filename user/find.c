#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/fs.h"
#include "kernel/fcntl.h"

void find(char* path, char* key) {
    int fd;
    struct stat st;
    struct dirent de;

    if((fd = open(path, O_RDONLY)) < 0){
        fprintf(2, "find: cannot open %s\n", path);
        return;
    }
    if(fstat(fd, &st) < 0){
        fprintf(2, "find: cannot stat %s\n", path);
        close(fd);
        return;
    }

    char *filename = path + strlen(path);
    while (filename > path && *(filename - 1) != '/') {
        filename--;
    }
    if (strcmp(filename, key) == 0) {
        printf("%s\n", path);
    }

    switch (st.type) {
        case T_DEVICE:
        case T_FILE: {
            char *p;
            for(p = path + strlen(path); p >= path && *p != '/'; p--)
                ;
            p++;
            break;
        }
        case T_DIR:
            while (read(fd, &de, sizeof(de)) == sizeof(de)) {
                if (de.inum == 0) continue;
                if (strcmp(de.name, ".") == 0 || strcmp(de.name, "..") == 0) continue;
                char newpath[512];
                strcpy(newpath, path);

                char *p = newpath + strlen(newpath);
                *p++ = '/';

                memmove(p, de.name, DIRSIZ);
                p[DIRSIZ] = 0;

                find(newpath, key);
            }
    }
    
    close(fd);
    return;
}

int main (int argc, char* argv[]) {

    if (argc < 3) {
        fprintf(2, "Usage: find <dir> <key>\n");
        exit (1);
    }
    find(argv[1], argv[2]);
    exit(0);
}
