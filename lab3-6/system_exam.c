#include <stdlib.h>
#include <stdio.h>
#include <string.h>

int main(int argc, char* argv[]) {
    if (argc < 2) {
        fprintf(stderr, "Usage: %s command [args...]\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    // system 함수를 사용하여 명령을 실행
    char command[4096];  // 충분한 크기로 배열을 할당
    snprintf(command, sizeof(command), "%s", argv[1]);
    
    for (int i = 2; i < argc; ++i) {
        snprintf(command + strlen(command), sizeof(command) - strlen(command), " %s", argv[i]);
    }

    int result = system(command);

    // 명령의 반환값 출력
    if (result == -1) {
        perror("system");
    } else {
        printf("Command returned: %d\n", result);
    }

    return 0;
}
