#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

// 부모 프로세스 종료시 호출될 함수
void parentExitFunction() {
    printf("부모 프로세스를 종료합니다.\n");
}

// 자식 프로세스 종료시 호출될 함수
void childExitFunction() {
    printf("자식 프로세스를 종료종료합니다.\n");
}

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

        // atexit 함수를 사용하여 자식 프로세스 종료시 호출될 함수 등록
        atexit(childExitFunction);

        // 자식 프로세스에서의 작업 수행
        // (이 예제에서는 단순히 sleep 함수를 사용하여 대기하는 것으로 대체)
        for(int i=1; i<6; i++){
            printf("자식 프로세스: %d\n",i);
            sleep(1);
        }
        

        printf("자식 프로세스 작업 완료\n");

        exit(0);
    } else {
        // atexit 함수를 사용하여 부모 프로세스 종료시 호출될 함수 등록
        atexit(parentExitFunction);

        // 부모 프로세스에서 실행하는 부분
        printf("부모 프로세스 - ID: %d, 자식 프로세스 ID: %d\n", getpid(), child_pid);

        // 부모 프로세스는 자식 프로세스의 종료를 기다림
        int status;
        waitpid(child_pid, &status, 0);
        printf("부모 프로세스 - 자식 프로세스의 작업이 완료되었습니다.\n");

        if (WIFEXITED(status)) {
            printf("보모 프로세스 - 자식 프로세스 종료 코드: %d\n", WEXITSTATUS(status));
        }
    }

    return 0;
}
