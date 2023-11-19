#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

int main(int argc, char* argv[]) {
    // 파이프를 통해서 프로세스의 출력을 다른 프로세스의 입력으로 변경
    int pipes[2][2];
    pid_t pids[3];
    char buffer[100];
    
    // 파이프 생성
    for (int i = 0; i < 2; ++i) {
        if (pipe(pipes[i]) == -1) {
            perror("pipe");
            exit(EXIT_FAILURE);
        }
    }

    // 자식 프로세스 생성
    if ((pids[0] = fork()) == -1) {
        perror("fork");
        exit(EXIT_FAILURE);
    }

    if (pids[0] == 0) { // 첫 번째 자식 프로세스
        // 현재 자식 프로세스에서 이전 프로세스의 출력을 입력으로 받음
        close(pipes[0][0]); // 현재 프로세스의 읽기 디스크립터를 닫음
        dup2(pipes[0][1], STDOUT_FILENO); // 현재 프로세스의 출력을 파이프로 연결
        close(pipes[0][1]); // 현재 프로세스의 쓰기 디스크립터를 닫음
    
        // pipe_input 프로그램 실행
        if (execvp("./pipe_input", argv) == -1) {
            perror("execvp");
            exit(EXIT_FAILURE);
        }
    } else if (pids[0] > 0){
        waitpid(pids[0], NULL, 0);
    }

    if ((pids[1] = fork()) == -1) {
        perror("fork");
        exit(EXIT_FAILURE);
    }

    if (pids[1] == 0) { // 두 번째 자식 프로세스
        // 현재 자식 프로세스에서 이전 프로세스의 출력을 입력으로 받음
        close(pipes[0][1]); // 현재 프로세스의 쓰기 디스크립터를 닫음
        dup2(pipes[0][0], STDIN_FILENO); // 이전 프로세스의 출력을 현재 프로세스의 입력으로 복제
        close(pipes[0][0]); // 이전 프로세스의 읽기 디스크립터를 닫음

        close(pipes[1][0]); // 현재 프로세스의 읽기 디스크립터를 닫음
        dup2(pipes[1][1], STDOUT_FILENO); // 현재 프로세스의 출력을 파이프로 연결
        close(pipes[1][1]); // 현재 프로세스의 쓰기 디스크립터를 닫음

        if (execvp("./pipe_output", argv) == -1) {
            perror("execvp");
            exit(EXIT_FAILURE);
        }
    } else if (pids[1] > 0) {
        close(pipes[1][1]); // 부모 프로세스에서는 쓰기 디스크립터를 닫음
        waitpid(pids[1], NULL, 0);
    
        ssize_t read_bytes;

        // pipes[1][0]을 통해 출력을 읽어옴
        read_bytes = read(pipes[1][0], buffer, sizeof(buffer));
        buffer[read_bytes] = '\0'; // 문자열 끝에 NULL 추가

        printf("파이프에서 받은 메시지: %s\n", buffer);

        close(pipes[1][0]); // 읽기 디스크립터를 닫음
    }

    
    // 공유메모리에 쓰기 및 읽기
    int shmid;
    char *shared_memory;

    // 공유 메모리 생성
    shmid = shmget((key_t)1234, sizeof(buffer), 0666 | IPC_CREAT);
    if (shmid == -1) {
        perror("shmget failed : ");
        exit(EXIT_FAILURE);
    }

    // 공유 메모리를 사용하기 위해 프로세스 메모리에 붙인다.
    shared_memory = shmat(shmid, (void *)0, 0);
    if (shared_memory == (void *)-1) {
        perror("shmat failed : ");
        exit(EXIT_FAILURE);
    }

    // 자식 프로세스 생성
    if ((pids[2] = fork()) == -1) {
        perror("fork");
        exit(EXIT_FAILURE);
    }

    if (pids[2] == 0) { // 자식 프로세스
        strcpy(shared_memory, buffer); // 공유 메모리에 데이터 쓰기
        exit(EXIT_SUCCESS);
    } else if (pids[2] > 0) { // 부모 프로세스
        waitpid(pids[2], NULL, 0);
        printf("공유메모리에서 읽은 메시지: %s\n", shared_memory); // 공유 메모리에서 데이터 읽기
    }

    // 공유 메모리를 사용한 후에는 반드시 분리해야 한다.
    if (shmdt(shared_memory) == -1) {
        perror("shmdt failed : ");
        exit(EXIT_FAILURE);
    }

    // 프로세스가 공유 메모리를 모두 사용했으면 ipcrm 명령어로 공유 메모리를 제거한다.
    if (shmctl(shmid, IPC_RMID, NULL) == -1) {
        perror("shmctl failed : ");
        exit(EXIT_FAILURE);
    }
    return 0;
}
