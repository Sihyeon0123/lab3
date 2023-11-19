#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>

void createDirectory(const char *dirname) {
    if (mkdir(dirname, 0777) == -1) {
        perror("디렉토리 생성 오류");
        exit(EXIT_FAILURE);
    }
}

void createFile(const char *dirname, const char *filename, const char *content) {
    char path[256];
    sprintf(path, "%s/%s", dirname, filename); // 디렉토리와 파일 이름을 합쳐서 경로 생성

    FILE *file = fopen(path, "w");
    if (file != NULL) {
        fprintf(file, "%s\n", content);
        fclose(file);
    } else {
        perror("파일 생성 오류");
        exit(EXIT_FAILURE);
    }
}

void readFile(const char *dirname, const char *filename) {
    char path[256];
    sprintf(path, "%s/%s", dirname, filename); // 디렉토리와 파일 이름을 합쳐서 경로 생성

    FILE *file = fopen(path, "r");
    if (file != NULL) {
        char buffer[256];
        while (fgets(buffer, sizeof(buffer), file) != NULL) {
            printf("%s", buffer);
        }
        fclose(file);
    } else {
        perror("파일 읽기 오류");
        exit(EXIT_FAILURE);
    }
}

void deleteFile(const char *dirname, const char *filename) {
    char path[256];
    sprintf(path, "%s/%s", dirname, filename); // 디렉토리와 파일 이름을 합쳐서 경로 생성

    if (remove(path) == 0) {
        printf("파일 '%s' 삭제 성공.\n", filename);
    } else {
        perror("파일 삭제 오류");
        exit(EXIT_FAILURE);
    }
}

void deleteDirectory(const char *dirname) {
    if (rmdir(dirname) == 0) {
        printf("디렉토리 '%s' 삭제 성공.\n", dirname);
    } else {
        perror("디렉토리 삭제 오류");
        exit(EXIT_FAILURE);
    }
}

int main() {
    char dirname[100];
    char filename[100];
    char content[256];
    char answer;

    printf("디렉토리 이름을 입력하세요: ");
    scanf("%s", dirname);

    createDirectory(dirname);

    printf("파일 이름을 입력하세요: ");
    scanf("%s", filename);

    printf("파일 내용을 입력하세요: ");
    scanf(" %[^\n]", content);

    createFile(dirname, filename, content);

    printf("\n파일 내용:\n");
    readFile(dirname, filename);

    printf("\n파일을 삭제하시겠습니까? (y/n): ");
    scanf(" %c", &answer);

    if (answer == 'y' || answer == 'Y') {
        deleteFile(dirname, filename);
        // 디렉토리 삭제 여부 물어보기
        printf("\n디렉토리를 삭제하시겠습니까? (y/n): ");
        scanf(" %c", &answer);

        if (answer == 'y' || answer == 'Y') {
            deleteDirectory(dirname);
        }
    }
    return 0;
}
