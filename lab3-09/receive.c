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
    struct msgq_data recv_data;

    // 메시지 큐 연결
    if ((qid = msgget(QKEY, 0666)) == -1) {
        perror("msgget failed");
        exit(1);
    }

    while (1) {
        // 메시지 수신
        if (msgrcv(qid, &recv_data, sizeof(struct msgq_data), 1, 0) == -1) {
            perror("msgrcv failed");
            exit(1);
        }

        printf("상대방: %s\n", recv_data.text);

        // 메시지 송신
        printf("나: ");
        fgets(recv_data.text, BUFSIZE, stdin);
        recv_data.text[strcspn(recv_data.text, "\n")] = '\0'; // 개행 문자 제거
        recv_data.type = 1;

        if (msgsnd(qid, &recv_data, sizeof(struct msgq_data), 0) == -1) {
            perror("msgsnd failed");
            exit(1);
        }

    }

    return 0;
}
