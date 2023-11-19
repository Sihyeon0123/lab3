#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <stdlib.h>
#include <setjmp.h>

void restart(int signo);
void alarm_handler(int signo);
void end(int signo);
sigjmp_buf jbuf;
int i = 0;

int main(){
    struct sigaction act;

    
    /* 현재 위치를 저장 */
    if (sigsetjmp(jbuf, 1) == 0) {
        act.sa_handler = restart;
        sigaction(SIGINT, &act, NULL);
        signal(SIGALRM, alarm_handler);
        signal(SIGUSR1, end);
    }
    alarm(1);
    i = 0;
    while (1) {
        printf("i: %d\n", i); 
        if(i == 10)
            raise(SIGUSR1);
        sleep(1);   
    }
}

void restart(int signo){
    fprintf(stderr, "\ni가 0으로 초기화 됩니다.\n");
    siglongjmp(jbuf, 1); /* 저장된 곳으로 복귀 */
}

void end(int signo) {
    printf("i가 10에 도달하였습니다.\n");
    exit(0);
}

void alarm_handler(int signo) {
    i++;
    alarm(1); // 1초 후에 다시 시그널을 발생시키도록 설정
}