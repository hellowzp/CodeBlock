#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <stdbool.h>
#include <string.h>

//enum bool{false,true};

bool readLine(FILE* f, char* buff)

{

 int index = 0;

 while(EOF != (buff[index] = fgetc(f)))

 {

  if(buff[index]=='\n')

  {

   buff[index]=0;

   return true;

  }

  index++;

 }

 return false;

}

typedef struct student

{

 int math;

 int chinese;

 struct student *next;

}SNode,*LPSNode;

int main()

{

 // 生成

 printf("正在随机生成一百个人的成绩...\n");

 LPSNode header = new SNode;

 memset(header, '\0', sizeof(SNode));

 header->chinese = rand()%100;

 srand( (unsigned)time( NULL ) );

 Sleep(100);

 header->math = rand()%100;

 srand( (unsigned)time( NULL ) );

 Sleep(100);

 LPSNode node = header;

 for(int i = 0; i<99; i++)

 {

  node->next = new SNode;

  node = node->next;

  memset(node, '\0', sizeof(SNode));

  node->chinese = rand()%100;

  srand( (unsigned)time( NULL ) );

  Sleep(100);

  node->math = rand()%100;

  srand( (unsigned)time( NULL ) );

  Sleep(100);

 }

 // 保存

 printf("正在将一百个人的成绩保存到abc.txt文件...\n");

 FILE* fp = fopen("abc.txt", "w");

 if(fp==NULL) return -1;

 int index = 1;

 char buff[256];

 node = header;

 do

 {

  memset(buff, '\0', sizeof(buff));

  sprintf(buff, "Index %d:\n", index );

  fputs(buff, fp);

  memset(buff, '\0', sizeof(buff));

  sprintf(buff, "Chinese: %d\n", node->chinese );

  fputs(buff, fp);

  memset(buff, '\0', sizeof(buff));

  sprintf(buff, "Math: %d\n", node->math );

  fputs(buff, fp);

  fputs("\n", fp);

  node = node->next;

  index++;

 }

 while(node != NULL);

 fclose(fp);

 

 // 读取并显示

 printf("正在将一百个人的成绩从abc.txt文件读取出来...\n");

 fp = fopen("abc.txt", "r");

 if(fp==NULL) return -1;

 memset(buff, '\0', sizeof(buff));

 while(readLine(fp, buff))

 {

  printf("%s\n", buff);

 }

 while(node != NULL);

 fclose(fp);

 

 printf("打完收工！\n");

 getchar();

}
