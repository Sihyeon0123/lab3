#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define BUFSIZE 128
#define QKEY (key_t)0111

struct msgq_data {
    long type;
    char text[BUFSIZE];
};

int main() {
    int qid;
    struct msgq_data send_data;

    // 메시지 큐 생성
    if ((qid = msgget(QKEY, IPC_CREAT | 0666)) == -1) {
        perror("msgget failed");
        exit(1);
    }

    while (1) {
        
        // 메시지 전송
        printf("나: ");
        fgets(send_data.text, BUFSIZE, stdin);
        send_data.text[strcspn(send_data.text, "\n")] = '\0'; // 개행 문자 제거
        send_data.type = 1;

        if (msgsnd(qid, &send_data, sizeof(struct msgq_data), 0) == -1) {
            perror("msgsnd failed");
            exit(1);
        }

        // 메시지 수신
        if (msgrcv(qid, &send_data, sizeof(struct msgq_data), 1, 0) == -1) {
            perror("msgrcv failed");
            exit(1);
        }

        printf("상대방: %s\n", send_data.text);
    }

    return 0;
}
