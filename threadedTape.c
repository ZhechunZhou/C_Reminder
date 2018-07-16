#include "tape.h"
#include <stdio.h>
#include <pthread.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>

FILE * tape;
size_t tape_length;
int buffer;
int * content;
char filename[100];
FILE ** files;

size_t nhead=0;
char command[5];
char ch;
char* commands[3]={"HEAD","READ","QUIT"};
int offset;
int * head_offset;
int quit=1;

pthread_t* threads;

void* tape_reader(void* args) {
 int headIndex = ((int*)args)[0];
 int read_offset = ((int*)args)[1];
 int cursor;

 if(head_offset[headIndex]<0)
  cursor=tape_length-(-head_offset[headIndex])%tape_length;
 else
  cursor=head_offset[headIndex]%tape_length;

 if(read_offset>=0)
 {
  for(int i = 0;i<read_offset;i++)
  {
   buffer=content[cursor];
   fwrite(&buffer,1,1,files[headIndex]);
   cursor=(cursor+1)%tape_length;
  }
 }
 else
 {
  for(int i = 0;i>read_offset;i--)
  {
   cursor=cursor-1;
   if(cursor<0)
    cursor+=tape_length;
   buffer=content[cursor];
   fwrite(&buffer,1,1,files[headIndex]);
  }
  cursor--;
 }
 head_offset[headIndex]=cursor;
 return NULL;
}

int main(int argc, char**argv) {

 if(argc<=1)
 {
  printf("Tape Not Inserted\n");
  return 0;
 }
 strcpy(filename,argv[1]);
 tape=fopen(filename,"rb");
 if(tape==NULL)
 {
  printf("Cannot Read Tape\n");
  return 0;
 }
 fseek(tape, 0, SEEK_END);
 tape_length = ftell(tape);
 rewind(tape);

 content=(int *)malloc(sizeof(int)*tape_length);
 for (int i = 0; i < tape_length; i++) {
  fread(&buffer, 1, 1, tape);
  content[i]=buffer;
 }
 fclose(tape);

 head_offset=(int *)malloc(sizeof(int)*nhead);
 files=(FILE **)malloc(sizeof(FILE *)*nhead);
 threads=(pthread_t *)malloc(sizeof(pthread_t)*nhead);

 while(quit)
 {
  for(int i = 0;i<4;i++)
  {
   ch=getchar();
   if(ch==10)
    i--;
   else
    command[i]=(char)ch;
  }
  command[4]='\0';

  if(!strcmp(command,commands[0]))  //HEAD
  {
   scanf("%d",&offset);
   nhead++;
   files=(FILE **)realloc(files, sizeof(FILE *)*nhead);
   head_offset=(int *)realloc(head_offset,sizeof(int)*nhead);
   threads=(pthread_t *)realloc(threads,sizeof(pthread_t)*nhead);

   char temp[20];
   snprintf(temp, 20, "head%zu",nhead);
   files[nhead-1]=fopen(temp,"wb");
   head_offset[nhead-1]=offset;

   if(offset>=0)
    printf("HEAD %zu at +%d\n\n",nhead,offset);
   else
    printf("HEAD %zu at %d\n\n",nhead,offset);
  }
  else if(!strcmp(command,commands[1]))//READ
  {
   scanf("%d",&offset);
   int tempArgs[nhead][2];
   for(int i =0;i<nhead;i++)
   {
    tempArgs[i][0]=i;
    tempArgs[i][1]=offset;
    pthread_create(&threads[i],NULL,tape_reader,(void *)tempArgs[i]);
   }
   for(int i =0;i<nhead;i++)
   {
    pthread_join(threads[i],NULL);
   }
   printf("Finished Reading\n\n");
  }
  else  //QUIT
  {
   for(int i=0;i<nhead;i++)
   {
    fclose(files[i]);
   }
   free(files);
   free(content);
   free(head_offset);
   quit=0;
  }
 }
}
