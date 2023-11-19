#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <sys/stat.h>

void listFilesRecursively(char *basePath) {
    DIR *dir;
    struct dirent *entry;
    struct stat statbuf;

    if ((dir = opendir(basePath)) == NULL) {
        perror("opendir");
        exit(EXIT_FAILURE);
    }

    while ((entry = readdir(dir)) != NULL) {
        char path[1024];
        snprintf(path, sizeof(path), "%s/%s", basePath, entry->d_name);

        if (stat(path, &statbuf) == -1) {
            perror("stat");
            exit(EXIT_FAILURE);
        }

        if (S_ISDIR(statbuf.st_mode)) {
            // 디렉토리인 경우 재귀적으로 방문
            if (strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..") != 0) {
                printf("Directory: %s\n", path);
                listFilesRecursively(path);
            }
        } else {
            // 파일인 경우
            printf("File: %s\n", path);
        }
    }

    closedir(dir);
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <directory_path>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    printf("Contents of directory '%s':\n", argv[1]);
    listFilesRecursively(argv[1]);

    return 0;
}
