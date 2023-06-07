#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char* encodedFilename;

typedef struct{
                FILE* fp;
                char** all;
                int maxCol;
}FileData;

// 2의 제곱수 구하는 함수
int pow2(int value) {
                int result = 1;
                for (int i = 0; i < value; i++)
                                result *= 2;
                return result;
}
char output_origin(unsigned short result) {
                //11=p1,10=p2,8=p3,4=p4
                char haming[12];
                char bin[8] = { 0, };
                int j = 11, i, k = 0, p[4] = { 0, }, syn = 0, check = 0;
                char output = 0;
                //비트값을 배열에 넣는 함수
                for (i = 0; i < 12; i++)
                                haming[i] = result >> (11 - i) & 1;
                //배열에 잘 들어 갔는지 확인
                /*for (i = 0; i < 12; i++)
                        printf("%d ", haming[i]);
                        printf("\n");*/
                p[0] = haming[1] + haming[3] + haming[5] + haming[7] + haming[9] + haming[11];
                p[1] = haming[1] + haming[2] + haming[5] + haming[6] + haming[9] + haming[10];
                p[2] = haming[0] + haming[5] + haming[6] + haming[7] + haming[8];
                p[3] = haming[0] + haming[1] + haming[2] + haming[3] + haming[4];
                //printf("p1: %d p2: %d p3: %d p4: %d\n",p[0], p[1], p[2], p[3]);
                //오류가 있는 지 검사
                for (i = 0; i < 4; i++) {
                                if (p[i] % 2 == 1) check += 1 * pow2(i);
                                else if (p[i] % 2 == 0) check += 0 * pow2(i);
                }
                // printf("check : %d\n", check);
                if (check != 0) {
                                if (haming[12 - check] == 0) haming[12 - check] = 1;
                                else if (haming[12 - check] == 1) haming[12 - check] = 0;
                }
                /*       printf("check befor\n");
                                                 for(i = 0; i < 12; i++)
                                                 printf("%d ", haming[i]);
                                                 printf("\n");  */
                for (i = 0; i < 12; i++) {
                                if (i == 11 || i == 10 || i == 8 || i == 4)
                                                continue;
                                else {
                                                bin[k] = haming[i];
                                                k++;
                                }
                }

                /* for(i = 0; i < 8; i++)
                         printf("%d ", bin[i]);
                         printf("\n");*/

                for (i = 0; i < 8; i++)
                                output += bin[i] * pow2(7 - i);
                return output;
}

void view(FileData* fd, int col) {
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
                view(fd, col + 1);

                if (fd->all == NULL) {
                                fd->all = (char**)malloc(sizeof(char*) * fd->maxCol);
                }

                fd->all[fd->maxCol - col] = (char*)malloc(sizeof(char) * 16);

                for (i = 0; i < 16; i++) {
                                unsigned short result = 0;
                                for (j = 0; j < 12; j++)
                                                result += (storage[i][j] & 1) * pow2(11 - j);
                                fd->all[fd->maxCol - col][i] = output_origin(result);
                }

                for (i = 0; i < 16; i++)
                                printf("%c", fd->all[fd->maxCol - col][i]);
                printf("\n");
}

void clearStr(char str[]){
                int size=strlen(str);
                for(int i=0;i<size;i++) str[i]=0;
}

void readAll(FileData* fd){
                int USS=1;//max=7
                int ITS=0;//max=1
                int FRS=0;//max=4 백틱 없으면 초기화
                // int DSS;
                int SCnt=0;
                char str[256];
                int index=0;
                int RealFirst=1;
                int fCnt=0;
                clearStr(str);

                printf("*USER STATUS*\n");
                for(int i=0;i < fd->maxCol;i++){
                                for(int j=0;j<16;j++){
                                                if(fd->all[i][j]!='/') str[index++]=fd->all[i][j];
                                                if(fd->all[i][j]=='/'){
                                                                if(USS<=7){
                                                                                switch(USS){
                                                                                               case 1 :
                                                                                               printf("ID: %s\n", str);
                                                                                               break;
                                                                                               case 2:
                                                                                               printf("NAME: %s\n", str);
                                                                                               break;
                                                                                               case 3:
                                                                                               printf("GENDER: %s\n", str);
                                                                                               break;
                                                                                               case 4:
                                                                                               printf("AGE: %s\n", str);
                                                                                               break;
                                                                                               case 5:
                                                                                               printf("HP: %s\n", str);
                                                                                               break;
                                                                                               case 6:
                                                                                               printf("MP: %s\n", str);
                                                                                               break;
                                                                                               case 7:
                                                                                               printf("COIN: %s\n", str);
                                                                                               break;
                                                                                }
                                                                                USS++;
                                                                                clearStr(str);
                                                                                index=0;
                                                                }
                                                                else if(USS==8&&ITS==0){
                                                                                printf("*ITEMS*\n");
                                                                                ITS++;
                                                                                int size=strlen(str);
                                                                                int numCnt=0;
                                                                                char num[4];
                                                                                for(int i=0;i<size;i++){
                                                                                               if(str[i]>='0'&&str[i]<='9'){
                                                                                               num[numCnt++]=str[i];
                                                                                               continue;
                                                                                               }
                                                                                               else{
                                                                                               switch(str[i]){
                                                                                               case 'A':
                                                                                               printf("BOMB: %s\n", num);
                                                                                               numCnt=0;
                                                                                               break;
                                                                                               case 'B':
                                                                                               printf("POTION: %s\n", num);
                                                                                               numCnt=0;
                                                                                               break;
                                                                                               case 'C':
                                                                                               printf("CURE: %s\n", num);
                                                                                               numCnt=0;
                                                                                               break;
                                                                                               case 'D':
                                                                                               printf("BOOK: %s\n", num);
                                                                                               numCnt=0;
                                                                                               break;
                                                                                               case 'E':
                                                                                               printf("SHIELD: %s\n", num);
                                                                                               numCnt=0;
                                                                                               break;
                                                                                               case 'F':
                                                                                               printf("CANNON: %s\n", num);
                                                                                               numCnt=0;
                                                                                               break;
                                                                                               }
                                                                                               clearStr(num);
                                                                                               }
                                                                                }
                                                                                clearStr(str);
                                                                                index=0;
                                                                }
                                                                else if(strcmp(str,"`")==0){
                                                                        FRS=5;
                                                                        clearStr(str);
                                                                        index=0;
                                                                }
                                                                else if(ITS==1&&FRS<=4){
                                                                                FRS++;
                                                                                if(RealFirst){
                                                                                               printf("*FRIENDS LIST*\n");
                                                                                               RealFirst=0;
                                                                                }
                                                                                switch(FRS){
                                                                                               case 1:
                                                                                               printf("FRIENDS%d ID: %s\n", ++fCnt, str);
                                                                                               break;
                                                                                               case 2:
                                                                                               printf("FRIENDS%d NAME: %s\n", fCnt, str);
                                                                                               break;
                                                                                               case 3:
                                                                                               printf("FRIENDS%d GENDER: %s\n", fCnt, str);
                                                                                               break;
                                                                                               case 4:
                                                                                               printf("FRIENDS%d AGE: %s\n", fCnt, str);
                                                                                               FRS=0;
                                                                                               break;
                                                                                }
                                                                                clearStr(str);
                                                                                index=0;
                                                                }
                                                                else if(FRS==5&&strcmp(str,"~")!=0){
                                                                                printf("%s\n", str);
                                                                                clearStr(str);
                                                                                index=0;
                                                                }
                                                }

                                }

                }

}
int main(int argc, char* argv[]) {
                char* filename = argv[1];
                encodedFilename = argv[2];

                FileData* fd = (FileData*)malloc(sizeof(FileData));
                fd->fp = fopen(filename, "rb");
                int col = 1;

                fd->all = NULL;
                view(fd, col);
                readAll(fd);
                fclose(fd->fp);

                return 0;
}
