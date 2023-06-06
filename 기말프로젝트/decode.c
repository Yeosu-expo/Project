#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char* encodedFilename;

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
                                if (haming[12-check] == 0) haming[12-check] = 1;
                                else if (haming[12-check] == 1) haming[12-check] = 0;
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

void view(FILE* fp, char** all) {
                char arr2[16];
                unsigned short arr[12];
                char storage[16][12];
                int i, j;
                int num = fread(arr, sizeof(unsigned short), 12, fp);

                if (num == 0)
                                return;

                for (i = 0; i < 12; i++) {
                                for (j = 0; j < 16; j++)
                                                storage[j][i] = arr[i] >> (15 - j) & 1;
                }
                view(fp);

                for (i = 0; i < 16; i++) {
                                unsigned short result = 0;
                                for (j = 0; j < 12; j++)
                                                result += (storage[i][j] & 1) * pow2(11 - j);
                                arr2[i] = output_origin(result);
                }
                for (i = 0; i < 16; i++)
                                printf("%c", arr2[i]);
                printf("\n");
}

int main(int argc, char* argv[]) {
                char* filename=argv[1];
                encodedFilename=argv[2];
                FILE* fp = fopen(filename,"rt");

                char** all=(char**)malloc(sizeof(char*)*16);
                view(fp, all);
                fclose(fp);

                return 0;
}
