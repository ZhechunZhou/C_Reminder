#include <stdio.h>
#include<stdlib.h>
typedef struct run_t run_t;
typedef struct nonogram_t nonogram_t;

struct run_t {
  int length;
  int* run;
};

struct nonogram_t {
  int x_runs_length;
  int y_runs_length;
  run_t* x_runs;
  run_t* y_runs;
};
int* count(int* run,int* temp1,int length){

  int flag = 0;
  for(int i = 0; i <= length; i++){
    int temp = *(run+i);
    if(temp==1){
      flag = 1;
      *temp1 += 1;
    }else if(temp == 0 || flag == 1){
      flag = 0;
      temp1++;
    }
  }
  return temp1;
}
int main(void) {
	 nonogram_t field;

  //read the input
  scanf("%d %d",&field.x_runs_length,&field.y_runs_length);
  field.x_runs =(run_t*)malloc(field.y_runs_length*sizeof(int*));
  for(int j = 0; j <= field.y_runs_length; j++){
    int temp[field.x_runs_length];
    for(int i = 0; i <= field.x_runs_length; i++){
       scanf("%1d", &temp[i]);
    }
    (field.x_runs+j)->run = temp;

  }

	rewind(stdin);
	scanf("%d %d",&field.x_runs_length,&field.y_runs_length);
	field.y_runs =(run_t*)malloc(field.x_runs_length*sizeof(int*));
    for(int i = 0; i <= field.x_runs_length; i++){
    int temp[field.y_runs_length];
    for(int j = 0; j <= field.y_runs_length; j++){
       scanf("%d\n", &temp[j]);
    }
    (field.y_runs+i)->run = temp;
  }

  //get the out put
  printf("X:\n");
  for(int j = 0; j <= field.y_runs_length; j++){
    int* temp = (int*)malloc(field.x_runs_length*sizeof(int));
    temp = count((field.x_runs+j)->run,temp,field.x_runs_length);
    for(int i = 0; i <= field.x_runs_length; i++){
      if(temp+i==0){
        break;
      }
      printf("%d ",*(temp+i));
  }
  printf("\n");
}
  printf("Y:\n");
	 for(int i = 0; i <= field.y_runs_length; i++){
    int* temp = (int*)malloc(field.y_runs_length*sizeof(int));
    temp = count((field.y_runs+i)->run,temp,field.y_runs_length);
   for(int j = 0; j <= field.x_runs_length; j++){
      if(temp+i==0){
        break;
      }
      printf("%d ",*(temp+j));
  }
  printf("\n");
}
	return 0;
}
