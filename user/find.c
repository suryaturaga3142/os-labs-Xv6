#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/fs.h"
#include "kernel/fcntl.h"

bool to_exec = false;

void find(char* path, char* key, int cut_argc, char* cut_argv[]) {
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

    switch (st.type) {
        case T_DEVICE:
        case T_FILE: {
            char *filename = path + strlen(path);
            while (filename > path && *(filename - 1) != '/') filename--;

            if (strcmp(filename, key) == 0) {
                if (to_exec) {
                    int pid = fork();
                    if (pid == 0) {
                        char* exec_argv[32];
                        for (int i = 0; i < cut_argc; i++) exec_argv[i] = cut_argv[i];
                        exec_argv[cut_argc] = path;
                        exec_argv[cut_argc + 1] = 0;
                        exec(exec_argv[0], exec_argv);
                    }
                    else {
                        wait(0);
                    }
                }
                else printf("%s\n", path);
            }
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

                find(newpath, key, cut_argc, cut_argv);
            }
    }
    
    close(fd);
    return;
}

int main (int argc, char* argv[]) {

    if (argc < 3) {
        fprintf(2, "Usage: find <dir> <key> [-exec <cmd>]\n");
        exit (1);
    }
    if (argc == 4) {
        fprintf(2, "Usage: find <dir> <key> [-exec <cmd>]\n");
        exit (1);
    }
    if (argc > 4) {
        if (strcmp("-exec", argv[3]) == 0) to_exec = true;
        else {
            fprintf(2, "Usage: find <dir> <key> [-exec <cmd>]\n");
            exit (1);
        }
    }

    find(argv[1], argv[2], argc - 4, &(argv[4]));
    exit(0);
}
