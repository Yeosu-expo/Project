#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
        FILE* fp;
        char** all;
        FILE* tmp;
        int maxCol;
}FileData;

//#. 2의 제곱수 구하는 함수
int pow2(int value) {
        int result = 1;
        for (int i = 0; i < value; i++)
                result *= 2;
        return result;
}

//#. 해밍코드화된 short를 해석하고 문자로 저장하여 반환
char output_origin(unsigned short result) {
        char haming[12];
        char bin[8] = { 0, };
        int j = 11, i, k = 0, p[4] = { 0, }, syn = 0, check = 0;
        char output = 0;

        //비트값을 배열에 넣는 반복문
        for (i = 0; i < 12; i++)
                haming[i] = result >> (11 - i) & 1;

        //검사비트 값을 p배열에 각각 저장
        p[0] = haming[1] + haming[3] + haming[5] + haming[7] + haming[9] + haming[11];
        p[1] = haming[1] + haming[2] + haming[5] + haming[6] + haming[9] + haming[10];
        p[2] = haming[0] + haming[5] + haming[6] + haming[7] + haming[8];
        p[3] = haming[0] + haming[1] + haming[2] + haming[3] + haming[4];

        //오류가 있는 지 검사
        for (i = 0; i < 4; i++) {
                if (p[i] % 2 == 1) check += 1 * pow2(i);
                else if (p[i] % 2 == 0) check += 0 * pow2(i);
        }
        if (check != 0) {                                               //check가 0이 아니면 오류, 해당 위치 값 수정
                haming[12 - check] = (haming[12 - check] + 1) % 2;
        }

        //해밍코드를 다시 아스키코드로 전환하는 반복문
        for (i = 0; i < 12; i++) {
                if (i == 11 || i == 10 || i == 8 || i == 4)
                        continue;
                else {
                        bin[k] = haming[i];
                        k++;
                }
        }

        //output 변수에 한자리씩 밀어넣는 반복문
        for (i = 0; i < 8; i++)
                output += bin[i] * pow2(7 - i);
        return output;
}

//#. 바이너리 파일을 읽어서 거꾸로 기록된 파일을 정방향으로 재배열
//#. 읽은 파일을 동적 배열에 저장
void get(FileData* fd, int col) {
        char arr2[16];
        unsigned short arr[12];
        char storage[16][12];
        int i, j;
        int num = fread(arr, sizeof(unsigned short), 12, fd->fp);

        if (num == 0) {
                return;
        }

        for (i = 0; i < 12; i++) {
                for (j = 0; j < 16; j++)
                        storage[j][i] = arr[i] >> (15 - j) & 1;
        }
        fd->maxCol = col;
        get(fd, col + 1);

        if (fd->all == NULL) {
                fd->all = (char**)malloc(sizeof(char*) * fd->maxCol);
        }

        fd->all[fd->maxCol - col] = (char*)malloc(sizeof(char) * 16);

        for (i = 0; i < 16; i++) {
                unsigned short result = 0;
                for (j = 0; j < 12; j++){
                        result += (storage[i][j] & 1) * pow2(11 - j);
                }
                fd->all[fd->maxCol - col][i] = output_origin(result);
        }
}

//#. str 배열을 초기화하는 함수
void clearStr(char str[], int size) {
        for (int i = 0; i < size; i++) str[i] = 0;
}

//#. 압축된 상태 정보를 복원하는 함수
void status(int USS, char str[], FileData* fd) {
        switch (USS) {  //USS로 들어오는 정보의 순서에 따라 출력하는 값의 속성에 맞게 적어 줌
                case 1:
                        fprintf(fd->tmp, "ID%s\n", str);
                        break;
                case 2:
                        fprintf(fd->tmp, "NAME%s\n", str);
                        break;
                case 3:
                        fprintf(fd->tmp, "GENDER%s\n", str);
                        break;
                case 4:
                        fprintf(fd->tmp, "AGE%s\n", str);
                        break;
                case 5:
                        fprintf(fd->tmp, "HP%s\n", str);
                        break;
                case 6:
                        fprintf(fd->tmp, "MP%s\n", str);
                        break;
                case 7:
                        fprintf(fd->tmp, "COIN%s\n", str);
                        break;
        }
}

//#. 압축된 아이템 정보를 복원하는 함수
void item(char str[], FileData* fd) {
        int size = strlen(str);
        int numCnt = 0;                         //num 배열 인덱스 카운트하는 변수
        char num[4] = {0, };    //아이템 값을 저장하는 변수
        int realNum;
        for (int i = 0; i < size; i++) {
                //숫자거나 공백일 떄 num 배열에 저장
                if ((str[i] >= '0' && str[i] <= '9') || str[i] == 32) {
                        num[numCnt++] = str[i];
                        continue;
                }
                else {
                        switch (str[i]) {       //영어 대문자에 따라 아이템이 정해져서 num값이랑 함께 출력
                                case 'A':
                                        fprintf(fd->tmp, "BOMB:%s\n", num);
                                        break;
                                case 'B':
                                        fprintf(fd->tmp, "POTION:%s\n", num);
                                        break;
                                case 'C':
                                        fprintf(fd->tmp, "CURE:%s\n", num);
                                        break;
                                case 'D':
                                        fprintf(fd->tmp, "BOOK:%s\n", num);
                                        break;
                                case 'E':
                                        fprintf(fd->tmp, "SHIELD:%s\n", num);
                                        break;
                                case 'F':
                                        fprintf(fd->tmp, "CANNON:%s\n", num);
                                        break;
                        }
                        numCnt = 0;

                        for(int j = 1; j < 4; j++) num[j] = 0;
                }
        }
}

//#. 친구 리스트 출력 함수
void friends(int FRS, int fCnt, char str[], FileData* fd) {
        switch (FRS) {  //FRS 값을 기준으로 스위치문 작동
                case 1:
                        fprintf(fd->tmp, "FRIEND%d ID:%s\n", fCnt, str);
                        break;
                case 2:
                        fprintf(fd->tmp, "FRIEND%d NAME:%s\n", fCnt, str);
                        break;
                case 3:
                        fprintf(fd->tmp, "FRIEND%d GENDER:%s\n", fCnt, str);
                        break;
                case 4:
                        fprintf(fd->tmp, "FRIEND%d AGE:%s\n\n", fCnt, str);
                        break;
        }
}

//#. 읽은 바이너리 파일을 원본 텍스트 파일과 같이 복원하는 함수
void readAll(FileData* fd) {
        int USS = 0;//max=7
        int ITS = 0;//max=1
        int FRS = 0;//max=4 백틱 없으면 초기화
        char str[256];
        int index = 0;
        int fCnt = 0;
        int strSize = sizeof(str) / sizeof(str[0]);
        int descIndex = 0;
        char descTxt[100][1000];

        clearStr(str, strSize);

        fprintf(fd->tmp, "*USER STATUS*\n");
        for (int i = 0; i < fd->maxCol; i++) {
                for (int j = 0; j < 16; j++) {
                        if (fd->all[i][j] != -1) {
                                str[index++] = fd->all[i][j];
                        }
                        else if (fd->all[i][j] == -1) {
                                if (USS < 7) {
                                        status(++USS, str, fd);
                                        clearStr(str, strSize);
                                        index = 0;
                                }
                                else if (USS == 7 && ITS == 0) {
                                        fprintf(fd->tmp, "\n*ITEMS*\n");
                                        ITS++;
                                        item(str, fd);
                                        clearStr(str, strSize);
                                        index = 0;
                                }
                                else if (strcmp(str, "`") == 0) {                       //백틱(`) 나오면 친구 정보가 끝난 것이니
                                        fprintf(fd->tmp, "*DESCRIPTION*\n");//본인소개글 출력
                                        FRS = 5;
                                        clearStr(str, strSize);
                                        index = 0;
                                }
                                else if (ITS == 1 && FRS <= 4) {
                                        if(FRS == 0) fCnt++;    //친구 정보 묶음을 적기 시작한 적이 없으면 친구 수 증가
                                        if (fCnt == 1 && FRS == 0) {    //*FRIENDS LIST* 한 번만 출력
                                                fprintf(fd->tmp, "\n*FRIENDS LIST*\n");
                                        }
                                        FRS++;
                                        friends(FRS, fCnt, str, fd);
                                        if (FRS == 4) FRS = 0;                          //한 명 정보 다 받으면 초기화 후 다시 친구 정보 불러옴
                                        clearStr(str, strSize);                         //그 전 친구 정보 초기화
                                        index = 0;
                                }
                                else if (FRS == 5 && strcmp(str, "~") != 0) {   //친구 정보 다 받으면 본인소개글로
                                        if (str[1] == '?') {                                                                                            //넘어감
                                                int num2 = 0;
                                                for (int i = 2; i < strlen(str); i++) {
                                                        num2 = (num2 * 10) + str[i] - '0';
                                                }
                                                fprintf(fd->tmp, "%s\n", descTxt[num2 - 1]);
                                        }
                                        else {
                                                fprintf(fd->tmp, "%s\n", str);
                                                strcpy(descTxt[descIndex++], str);
                                        }
                                        clearStr(str, strSize);
                                        index = 0;
                                }
                        }
                }
        }
}

//#. 파일 속에 섞이는 개행문자를 제외하고 최종 파일을 작성하는 함수
void finish(char* fileName, FileData* fd) {
        FILE* fp = fopen(fileName, "wt");
        if (fp == NULL) {
                printf("파일 열기 실패\n");
                exit(1);
        }
        fd->tmp = fopen("tmp.txt", "rt");
        char ch = '\0';
        while (1) {
                if (fscanf(fd->tmp, "%c", &ch) == EOF) {
                        //#. 파일 끝까지 한 글자씩 받아와서 변수에 저장
                        break;
                }
                else if ((ch > 126 || ch < 9) || (ch > 13 && ch < 32)) {
                        //#. 변수 값을 아스키 10진과 비교해서 특정 구간이면 기록안함
                        continue;
                }
                else {
                        fprintf(fp, "%c", ch);
                }
        }
        fclose(fd->tmp);
        fclose(fp);
}

//#. 동적할당 해제
void freeAll(FileData* fd){
        for(int i = 0; i < fd->maxCol; i++){
                free(fd->all[i]);
        }
        free(fd->all);
}

int main(int argc, char* argv[]) {
        FileData* fd = (FileData*)malloc(sizeof(FileData));
        char* filename = argv[1];
        fd->tmp = fopen("tmp.txt", "wt");
        fd->fp = fopen(filename, "rb");
        fd->all = NULL;
        int col = 1;

        get(fd, col);
        readAll(fd);

        fclose(fd->fp);
        fclose(fd->tmp);

        finish(argv[2], fd);

        freeAll(fd);
        free(fd);
        return 0;
}