#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

int main() {
    pid_t child_pid;

    // 부모 프로세스에서 자식 프로세스를 생성
    child_pid = fork();

    if (child_pid < 0) {
        // fork 실패 시
        fprintf(stderr, "프로세스 생성 실패\n");
        return 1;
    } else if (child_pid == 0) {
        // 자식 프로세스에서 실행하는 부분
        printf("자식 프로세스 - ID: %d\n", getpid());
    } else {
        // 부모 프로세스에서 실행하는 부분
        printf("부모 프로세스 - ID: %d, 자식 프로세스 ID: %d\n", getpid(), child_pid);

        // 부모 프로세스는 자식 프로세스가 종료될 때까지 대기
        wait(NULL);

        printf("자식 프로세스가 종료되었습니다.\n");
    }

    return 0;
}
