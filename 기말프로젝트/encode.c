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

int howManyLine(FileData* fd) {
                int cnt = 0;
                char txt;
                while (fscanf(fd->fp, "%c", &txt) != EOF)
                                if (txt == '\n') cnt++;
                rewind(fd->fp);
                return cnt;
}

void fillZeroData(FileData* fd) {
                for (int i = 0; i < 16; i++) fd->data[i] = 0;
}

int howManyFriends(FILE* fp) {
                int cnt = -1;
                char txt;
                char line[256];

                do {
                                fscanf(fp, " %[^\n]s", line);
                                cnt++;
                } while (strcmp(line, "*DESCRIPTION*") != 0);
                rewind(fp);

                do {
                                fscanf(fp, " %[^\n]s", line);
                } while (strcmp(line, "*FRIENDS LIST*") != 0);
                //fscanf(fp, " %[^\n]s", line);
                return cnt / 4;
}

void mk_haming(char ch, char haming[]) {
                short bin[8];
                int j = 0, i;

                for (i = 7; i >= 0; i--)
                                bin[7 - i] = (ch >> i) & 1;

                haming[11] = (bin[7] + bin[6] + bin[4] + bin[3] + bin[1]) % 2;
                haming[10] = (bin[7] + bin[5] + bin[4] + bin[2] + bin[1]) % 2;
                haming[8] = (bin[6] + bin[5] + bin[4] + bin[0]) % 2;
                haming[4] = (bin[3] + bin[2] + bin[1] + bin[0]) % 2;

                for (i = 0; i < 12; i++) {
                                if (i == 4 || i == 8 || i == 10 || i == 11)
                                                continue;
                                else haming[i] = bin[j++];
                }
}
//해밍코드를 가져와서 16*12배열에 저장
void mk_arr_1612(FileData* fd) {
                char haming[12];
                for (int i = 0; i < 16; i++) {
                                mk_haming(fd->data[i], haming);
                                for (int k = 0; k < 12; k++)
                                                fd->block[i][k] = haming[k];
                }
}
// 16*12 배열에 저장한 값 확인용
void print_haming(FileData* fd) {
                for (int i = 0; i < 16; i++) {
                                printf("%d 번째\n", i);
                                for (int k = 0; k < 12; k++)
                                                printf("%d ", fd->block[i][k]);
                                printf("\n");
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

//#. short 배열값 확인용 출력 함수
void print_short(unsigned short arr[12]) {
                for (int i = 0; i < 12; i++) {
                                printf("%d\n", arr[i]);
                }
}

//#. storage를 초기화
void fillZero(FileData* fd) {
                for (int i = 0; i < 16; i++)
                                for (int j = 0; j < 12; j++)
                                                fd->block[i][j] = '0';
}

//#. 값이 잘 들어가고 있는 확인하는 함수
void print_short_one(int result, char storage[16][12], int j, int i) {
                printf("%d(result) = %d(result) + (%d(storage[%d][%d]) * %d(square(15-%d)))\n",
                                                result, result, storage[j][i], j, i, square(15 - j), j);
}

//#. storage를 받아와 short형 자료구조로 만들어서 파일에 저장
void binary_to_short(FileData* fd) {
                int i, j;
                unsigned short arr[12];
                for (i = 0; i < 12; i++) {
                                unsigned short result = 0;
                                for (j = 0; j < 16; j++) {
                                                result += fd->block[j][i] * square(15 - j);
                                                //print_short_one(result, stroage, j, i);
                                }
                                //printf("\n");
                                arr[i] = result;
                }
                //print_short(arr);

                FILE* fp = fopen(fd->encodedFilename, "ab+");
                fwrite(arr, sizeof(unsigned short), 12, fp);

                fillZero(fd);
}


void status(FileData* fd) {
                char line[256];

                int cnt2 = 7;
                while (cnt2--) {
                                char what[10];
                                fscanf(fd->fp, " %s", what);
                                fscanf(fd->fp, " %[^\n]s", line);
                                char ch= -1;
                                strcat(line, &ch);
                                //printf("%s\n",line);
                                int size = strlen(line);
                                for (int i = 0; i <= size; i++) {
                                                if (fd->charcnt == 16) {
                                                                mk_arr_1612(fd);
                                                                //print_haming(fd);
                                                                binary_to_short(fd);
                                                                fillZero(fd);
                                                                fillZeroData(fd);
                                                                fd->charcnt = 0;
                                                }
                                                if (i == size) continue;
                                                fd->data[fd->charcnt++] = line[i];
                                }
                }
}

int item(FileData* fd) {
                char line[256];
                for (int i = 0; i < 256; i++) line[i] = 0;
                char str[256];
                fscanf(fd->fp, " %[^\n]s", str); fd->cnt--;
                while (strstr(str, ":") != NULL) {
                                char* item = strtok(str, " ");
                                char* get = strtok(NULL, "\n");

                                strcat(line, get);
                                if (strcmp(item, "BOMB:") == 0) strcat(line, "A");
                                if (strcmp(item, "POTION:") == 0) strcat(line, "B");
                                if (strcmp(item, "CURE:") == 0) strcat(line, "C");
                                if (strcmp(item, "BOOK:") == 0) strcat(line, "D");
                                if (strcmp(item, "SHIELD:") == 0) strcat(line, "E");
                                if (strcmp(item, "CANNON:") == 0) strcat(line, "F");

                                fscanf(fd->fp, " %[^\n]s", str); fd->cnt--;
                }
                char ch=-1;
                strcat(line, &ch);
                int size = strlen(line);
                for (int i = 0; i <= size; i++) {
                                if (fd->charcnt == 16) {
                                                //printf("%ld\n", strlen(line));
                                                mk_arr_1612(fd);
                                                binary_to_short(fd);
                                                fillZero(fd);
                                                fd->charcnt = 0;
                                }
                                if (i == size) continue;
                                fd->data[fd->charcnt++] = line[i];
                }
                //printf("itemline:%s\n", str);
                size = strlen(str) + 1;
                fseek(fd->fp, -size, SEEK_CUR);
}

int friends(FileData* fd) {
                char line[256];

                int cnt2 = 4;
                while (cnt2--) {
                                fscanf(fd->fp, " %[^\n]s", line);

                                if (strcmp(line, "*DESCRIPTION*") != 0) {
                                                strtok(line, ":");

                                                strcpy(line, strtok(NULL, ""));
                                }
                                char ch=-1;
                                strcat(line, &ch);

                                int size = strlen(line);
                                for (int i = 1; i <= size; i++) {
                                                if (fd->charcnt == 16) {
                                                                mk_arr_1612(fd);
                                                                binary_to_short(fd);
                                                                fillZero(fd);
                                                                fd->charcnt = 0;
                                                }
                                                if (i == size) continue;
                                                fd->data[fd->charcnt++] = line[i];
                                }
                }
                fscanf(fd->fp, "%s", line);
}

int desc(FileData* fd)
{
                char ch=-1;
                char backtic[3] = "`";
                strcat(backtic, &ch);
                for (int i = 0; i <= 2; i++) {
                                if (fd->charcnt == 16) {
                                                mk_arr_1612(fd);
                                                binary_to_short(fd);
                                                fillZero(fd);
                                                fd->charcnt = 0;
                                }
                                if (i == 2) continue;
                                fd->data[fd->charcnt++] = backtic[i];
                }

                char* lines[1000];
                char* comp[1000];
                int compCnt = 0;
                char buffer[300];
                int index[10] = { 0 };
                int count = 0;
                int flag = 0;

                while (fscanf(fd->fp, " %[^\n]s", buffer)!=EOF)
                {
                                flag = 0;
                                for (int i = 0; i < count; i++)
                                {
                                                if (strcmp(lines[i], buffer) == 0)
                                                {
                                                                flag = 1;
                                                                comp[compCnt] = (char*)malloc(sizeof(char) * 4);
                                                                sprintf(comp[compCnt], "?%d%s",  i+1, &ch);
                                                                compCnt++;
                                                                break;
                                                }
                                }
                                if (flag == 0)
                                {

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

                for (int i = 0; i <= compCnt; ++i)
                {
                                for (int j = 0; j <= strlen(comp[i]); ++j)
                                {
                                                if (fd->charcnt == 16)
                                                {
                                                                mk_arr_1612(fd);
                                                                binary_to_short(fd);
                                                                fillZero(fd);
                                                                fillZeroData(fd);
                                                                fd->charcnt = 0;
                                                }
                                                if (j == strlen(comp[i])) continue;
                                                fd->data[fd->charcnt++] = comp[i][j];
                                }
                }

                if (fd->charcnt != 0) {
                                mk_arr_1612(fd);
                                binary_to_short(fd);
                                fillZero(fd);
                                fd->charcnt = 0;
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

                //int charcnt=0;//data에 몇번째까지 들어갔는지 표시
                while (fd->cnt--) {
                                char line[256];
                                fscanf(fd->fp, " %[^\n]s", line);
                                //printf("line:%s, cnt:%d\n", line, cnt);

                                if (strcmp(line, "*USER STATUS*") == 0) {
                                                status(fd);// data몇번째까지 썼는지 반환
                                                fd->cnt -= 7;// status함수에서 7줄 처리를 해서 그 만큼 감소 시킴
                                }
                                else if (strcmp(line, "*ITEMS*") == 0) item(fd);
                                else if (strcmp(line, "*FRIENDS LIST*") == 0) {
                                                int friend_cnt = howManyFriends(fd->fp);
                                                for (int i = 0; i < friend_cnt; i++) {
                                                                friends(fd);
                                                                fd->cnt -= 5;
                                                }
                                                int size = strlen("*DESCRIPTION*");
                                                fseek(fd->fp, -size, SEEK_CUR);
                                }
                                if (strcmp(line, "*DESCRIPTION*") == 0) {
                                                desc(fd);
                                                break;
                                }
                }
                fclose(fd->fp);

                return 0;
}
