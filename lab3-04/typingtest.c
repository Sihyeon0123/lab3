#include <stdio.h>
#include <termios.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>
#include <time.h>

#define PASSWORDSIZE 12

int main(void) {
    int fd;
    int nread, cnt = 0, errcnt = 0;
    char ch;
    const char *texts[] = {
        "The magic thing is that you can change it.",
        "Success is not final, failure is not fatal: It is the courage to continue that counts.",
        "Life is what happens when you're busy making other plans.",
        "The only limit to our realization of tomorrow will be our doubts of today.",
        "In the end, we will remember not the words of our enemies, but the silence of our friends."
    };
    int num_texts = sizeof(texts) / sizeof(texts[0]);
    
    struct termios init_attr, new_attr;
    time_t start_time, end_time;

    fd = open(ttyname(fileno(stdin)), O_RDWR);
    tcgetattr(fd, &init_attr);

    new_attr = init_attr;
    new_attr.c_lflag &= ~ICANON;
    new_attr.c_lflag &= ~ECHO; /* ~(ECHO | ECHOE | ECHOK | ECHONL); */
    new_attr.c_cc[VMIN] = 1;
    new_attr.c_cc[VTIME] = 0;

    if (tcsetattr(fd, TCSANOW, &new_attr) != 0) {
        fprintf(stderr, "터미널 속성을 설정할 수 없음.\n");
    }

    printf("출력되는 문장을 그대로 입력하세요.");
    time(&start_time); // 시작 시간 기록
    
    for (int i = 0; i < num_texts; ++i) {
        int k = 0;
        const char *text = texts[i];
        printf("\n%s\n",text);
        while ((nread = read(fd, &ch, 1)) > 0 && ch != '\n') {
            cnt++;
            if (ch == text[k++])
                write(fd, &ch, 1);
            else {
                write(fd, "*", 1);
                errcnt++;
            }
        }
    }
    time(&end_time); // 종료 시간 기록  

    printf("\n타이핑 오류의 횟수는 %d\n", errcnt);

    double elapsed_time = difftime(end_time, start_time);
    double typing_speed = (cnt / elapsed_time) * 60; // 평균 분당 타자수 계산

    printf("평균 분당 타자수: %.2f\n", typing_speed);

    tcsetattr(fd, TCSANOW, &init_attr);
    close(fd);

    return 0;
}
