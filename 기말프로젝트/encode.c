#include <stdio.h>
#include <string.h>
#include <stdlib.h>

typedef struct {
    char block[16][12];
    char data[16];
    int cnt;
    int charcnt;
    char* filename;
    char* encodedFilename;
    FILE* fp;
}FileData;

//#. 파일의 전체 라인 수를 체크해서 반환해주는 함수
int howManyLine(FileData* fd) {
    int cnt = 0;
    char txt;
    while (fscanf(fd->fp, "%c", &txt) != EOF)   //커서 끝까지 반복
        if (txt == '\n') cnt++;
    rewind(fd->fp); //맨 밑으로간 커서를 맨 위로 올려줌
    return cnt;
}



//#. friend가 몇 명인지 세어 주는 함수
int howManyFriends(FILE* fp) {
    int cnt = -1;
    char txt;
    char line[256];

    do {
        fscanf(fp, " %[^\n]s", line);
        cnt++;
    } while (strcmp(line, "*DESCRIPTION*") != 0);       // DESCRIPTION*을 만날때까지 라인수를 카운트 함
                                                                                
    rewind(fp); //다시 파일 맨 위로
    do {
        fscanf(fp, " %[^\n]s", line);
    } while (strcmp(line, "*FRIENDS LIST*") != 0);//함수에 들어오기 전의 위치인 *FRIENDS LIST*로 이동
    return cnt / 4;       
}

//#. 문자를 해밍코드로 변환해서 문자열에 저장하는 함수
void mk_haming(char ch, char haming[]) {        //파라미터로 문자 하나와 해밍코드화된 12자리를 저장해줄 문자열 배열을 받음
    short bin[8];                                                             
    int j = 0, i;

    for (i = 7; i >= 0; i--)
        bin[7 - i] = (ch >> i) & 1;

                //검사 비트를 넣을 곳에 패리티 비트 값을 넣어줌
    haming[11] = (bin[7] + bin[6] + bin[4] + bin[3] + bin[1]) % 2;
    haming[10] = (bin[7] + bin[5] + bin[4] + bin[2] + bin[1]) % 2;
    haming[8] = (bin[6] + bin[5] + bin[4] + bin[0]) % 2;
    haming[4] = (bin[3] + bin[2] + bin[1] + bin[0]) % 2;

    for (i = 0; i < 12; i++) {
        if (i == 4 || i == 8 || i == 10 || i == 11)
          continue;                     //패리티 변수가 들어갈 곳은 건너뛰기
        else
                                        haming[i] = bin[j++];
    }
}
//해밍코드를 가져와서 16*12배열에 저장
void mk_arr_1612(FileData* fd) {
    char haming[12];
    for (int i = 0; i < 16; i++) {
        mk_haming(fd->data[i], haming);
        for (int k = 0; k < 12; k++)
                                                //구조체 안의 block배열에 해밍코드를 넣음
            fd->block[i][k] = haming[k];
    }
}

//#. num 만큼의 2의 제곱을 반환
unsigned short square(int num) {
    unsigned short result = 1;
    for (int i = 0; i < num; i++) {
        result *= 2;
    }
    return result;
}

//#. storage를 초기화
void fillZero(FileData* fd) {
    for (int i = 0; i < 16; i++)
        for (int j = 0; j < 12; j++)
            fd->block[i][j] = '0';
}

//#. data배열 초기화 함수
void fillZeroData(FileData* fd) {
    for (int i = 0; i < 16; i++) fd->data[i] = 0;
}

//#. storage를 받아와 short형 자료구조로 만들어서 파일에 저장
void binary_to_short(FileData* fd) {
    int i, j;
    unsigned short arr[12];
    for (i = 0; i < 12; i++) {
        unsigned short result = 0;
        for (j = 0; j < 16; j++) {
            result += fd->block[j][i] * square(15 - j);
        }
        arr[i] = result;
    }

    FILE* fp = fopen(fd->encodedFilename, "ab+");
    if (fp == NULL) {
        printf("파일 열기 실패\n");
        exit(1);
    }
    fwrite(arr, sizeof(unsigned short), 12, fp);

    fillZero(fd);
}

//#. 해밍코드화 및 바이너리 작성 코드 호출 함수
void store(FileData* fd) {
    mk_arr_1612(fd);
    //print_haming(fd);
    binary_to_short(fd);
    fillZero(fd);
                fillZeroData(fd);
    fd->charcnt = 0;
}

//#. 라인을 fd->data[16]에 저장 및 바이너리 작성
void storeLine(char line[], FileData* fd) {
    int size = strlen(line);
    for (int i = 0; i <= size; i++) {
        if (fd->charcnt == 16) {
            store(fd);
        }
        if (i == size) continue;
        fd->data[fd->charcnt++] = line[i];
    }
}

//#. user status 정보를 읽어오는 함수
void status(FileData* fd) {
    char line[256]; //한 줄을 받아와 저장할 함수
    int cnt2 = 7; //상태 정보는 7줄임
    while (cnt2--) {
        char what[10];//콜론 앞의 정보를 저장할 변수
        fscanf(fd->fp, " %[^:]s", what);
        fscanf(fd->fp, " %[^\n]s", line); //콜론 뒤의 정보를 받아옴
        char ch = -1;
        strcat(line, &ch); //각 속성을 구분해주는 구분자를 붙임
        storeLine(line, fd);
    }
}

//#. 아이템 정보를 입력받는 함수
int item(FileData* fd) {
    char line[256];//받은 정보를 저장할 변수
    for (int i = 0; i < 256; i++) line[i] = 0;
    char str[256];
    fscanf(fd->fp, " %[^\n]s", str); fd->cnt--;  //파일에서 정보 입력받음
    while (strstr(str, ":") != NULL) {
        char* item = strtok(str, ":"); //각각의 아이템 정보와 아이템
        char* get = strtok(NULL, "\n"); //개수를 콜론을 기준으로 분리

        strcat(line, get);
        if (strcmp(item, "BOMB") == 0) strcat(line, "A");   //아이템에 따라
        if (strcmp(item, "POTION") == 0) strcat(line, "B");  //대응되는 알파벳
        if (strcmp(item, "CURE") == 0) strcat(line, "C"); //입력
        if (strcmp(item, "BOOK") == 0) strcat(line, "D");
        if (strcmp(item, "SHIELD") == 0) strcat(line, "E");
        if (strcmp(item, "CANNON") == 0) strcat(line, "F");

        fscanf(fd->fp, " %[^\n]s", str); fd->cnt--;  //다음 줄 입력받기
    }
    char ch = -1;
    strcat(line, &ch);
    storeLine(line, fd);
    int size = strlen(str) + 1;
    fseek(fd->fp, -size, SEEK_CUR);
}

//#. 친구 정보를 입력받는 함수
int friends(FileData* fd) {
    char line[256];

    int cnt2 = 4;    //cnt2변수를 4로 초기화해서 friend 한 명씩 받아오도록 함
    while (cnt2--) {
        fscanf(fd->fp, " %[^\n]s", line);

        if (strcmp(line, "*DESCRIPTION*") != 0) {  //line이 *DESCRIPTION*이아닌 경우 실행
            strtok(line, ":");
            strcpy(line, strtok(NULL, ""));
        }
        char ch = -1;
        strcat(line, &ch);
        storeLine(line, fd);
    }
    fscanf(fd->fp, "%s", line);
}

//#. 본인소개글 정보를 받는 함수
int desc(FileData* fd)
{
    char ch = -1;
    char backtic[3] = "`";//친구 수를 세기 위해 친구와 본인소개글을 구분해주는 문자를 추가
    strcat(backtic, &ch);
    for (int i = 0; i <= 2; i++) {
        if (fd->charcnt == 16) {
            store(fd);
        }
        if (i == 2) continue;
        fd->data[fd->charcnt++] = backtic[i];
    }

    char* lines[1000];  //라인전체가 반복되었는지 확인하기 위해 라인별로 저장하는 문자열 배열
    char* comp[1000];           //압축된 디스크립션 내용을 담는 배열
    char buffer[300];           //파일에서 한 라인을 저장하는 변수
    int compCnt = 0;            //comp배열에 index를 세어주는 변수
    int count = 0;                      //라인전체 반복을 체크하기 위해 읽은 라인 수에 맞게 올라가는 변수
    int flag = 0;                               //라인 전체가 반복되었는지 체크하는 변수

    while (fscanf(fd->fp, " %[^\n]s", buffer) != EOF) { //파일이 끝날 때까지 입력받음
        flag = 0;
        for (int i = 0; i < count; i++) {
            if (strcmp(lines[i], buffer) == 0){ //라인 전체가 반복된 경우
                flag = 1;
                comp[compCnt] = (char*)malloc(sizeof(char) * 4);
                sprintf(comp[compCnt], "?%d%s", i + 1, &ch);
                compCnt++;
                break;
            }
        }
        if (flag == 0){ //라인 전체가 반복되지 않은 경우
            lines[count] = (char*)malloc(sizeof(char) * 300);
            strcpy(lines[count], buffer);
            comp[compCnt] = (char*)malloc(sizeof(char) * (strlen(buffer) + 2));
            sprintf(comp[compCnt], "%s%s", buffer, &ch);
            compCnt++;
            count++;
        }

    }
    comp[compCnt] = (char*)malloc(sizeof(char) * 2);
    strcpy(comp[compCnt], "~");

    for (int i = 0; i <= compCnt; i++){
        storeLine(comp[i], fd);
    }

    if (fd->charcnt != 0) {
        store(fd);
    }
}

int main(int argc, char* argv[]) {
    FileData* fd = (FileData*)malloc(sizeof(FileData));
    fd->filename = argv[1];
    fd->encodedFilename = argv[2];
    fd->fp = fopen(fd->filename, "rt");
    if (fd->fp == NULL) {
        printf("파일 열기 실패\n");
        exit(1);
    }

    fillZero(fd);
    fd->cnt = howManyLine(fd);

    while (fd->cnt--) {
        char line[256];
        fscanf(fd->fp, " %[^\n]s", line);

        if (strcmp(line, "*USER STATUS*") == 0) {
            status(fd);                 //data몇번째까지 썼는지 반환
            fd->cnt -= 7;               //status함수에서 7줄 처리를 해서 그 만큼 감소 시킴
        }
        else if (strcmp(line, "*ITEMS*") == 0) {
            item(fd);
        }
        else if (strcmp(line, "*FRIENDS LIST*") == 0) {
            int friend_cnt = howManyFriends(fd->fp);
            for (int i = 0; i < friend_cnt; i++) {
                friends(fd);
                fd->cnt -= 5;           //친구 정보는 공백까지 5줄 처리해서 그만큼 감소
            }
            int size = strlen("*DESCRIPTION*"); //*DESCRIPTION*이 나오면 커서 위치 이동
            fseek(fd->fp, -size, SEEK_CUR);
        }
        else if (strcmp(line, "*DESCRIPTION*") == 0) {
            desc(fd);
            break;
        }
    }
    fclose(fd->fp);
                free(fd);

    return 0;
}