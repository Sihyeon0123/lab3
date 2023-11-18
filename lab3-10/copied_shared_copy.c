#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>

#define SEM_KEY 1234
#define SHM_KEY 5678
#define BUF_SIZE 16384

// 세마포어 구조체
struct sembuf acquire = {0, -1, SEM_UNDO};
struct sembuf release = {0, 1, SEM_UNDO};

void semWait(int semId) {
    semop(semId, &acquire, 1);
}

void semSignal(int semId) {
    semop(semId, &release, 1);
}

int main(int argc, char* argv[]) {
    char input_file;
    if (argc < 2){
        printf("./shared <file>\n");
        exit(1);
    }

    // 세마포어 생성 또는 열기
    int semId = semget(SEM_KEY, 1, IPC_CREAT | 0666);
    if (semId == -1) {
        perror("Failed to create/open semaphore");
        exit(EXIT_FAILURE);
    }

    // 세마포어 초기화
    semctl(semId, 0, SETVAL, 1);

    // 공유 메모리 생성 또는 열기
    int shmId = shmget(SHM_KEY, BUF_SIZE, IPC_CREAT | 0666);
    if (shmId == -1) {
        perror("Failed to create/open shared memory");
        exit(EXIT_FAILURE);
    }

    // 공유 메모리에 접근
    char *sharedMemory = (char *)shmat(shmId, NULL, 0);

    // 부모 프로세스는 파일을 읽어 공유 메모리에 저장
    pid_t childPid = fork();
    if (childPid == 0) {
        size_t len = strlen(argv[1]);
        char *newFilename = (char *)malloc(len + 8);
        strcpy(newFilename, "copied_");
        strcat(newFilename, argv[1]);

        // 자식 프로세스는 세마포어를 기다림
        semWait(semId);

        // 자식 프로세스는 공유 메모리에서 읽어 파일에 쓰기
        FILE *outputFile = fopen(newFilename, "w");
        if (outputFile == NULL) {
            perror("Failed to open output file");
            exit(EXIT_FAILURE);
        }
        free(newFilename);

        fprintf(outputFile, "%s", sharedMemory);

        fclose(outputFile);

        // 세마포어를 해제하고 종료
        semSignal(semId);
        shmdt(sharedMemory);
        exit(EXIT_SUCCESS);
    } else if (childPid > 0) {
        // 부모 프로세스는 세마포어를 기다림
        semWait(semId);

        // 부모 프로세스는 파일에서 읽어 공유 메모리에 저장
        FILE *inputFile = fopen(argv[1], "r");
        if (inputFile == NULL) {
            perror("Failed to open input file");
            exit(EXIT_FAILURE);
        }

        // 파일에서 읽어 공유 메모리에 저장
        fread(sharedMemory, sizeof(char), BUF_SIZE, inputFile);

        fclose(inputFile);

        // 세마포어를 해제하고 자식 프로세스를 기다림
        semSignal(semId);
        wait(NULL);

        // 공유 메모리를 삭제하고 종료
        shmctl(shmId, IPC_RMID, NULL);
        shmdt(sharedMemory);
        exit(EXIT_SUCCESS);
    } else {
        perror("Fork failed");
        exit(EXIT_FAILURE);
    }

    return 0;
}
