#include <stdio.h>
#include<stdlib.h>
#include<string.h>

int main(int argc, char* argv[]){
  if(argc == 1){
    printf("Missing Argument\n");
	  return 0;
  }
	char* inviladChar = NULL;
  long input = strtol(argv[1],&inviladChar,10);
	if(strlen(inviladChar) != 0 || input < 0){
		printf("Invalid Argument\n");
		return 0;
	}
  long** output = (long **)malloc((input+1)*sizeof(long*));
	for(int i = 0;i < input+1; i++){
		output[i] = (long *)malloc((input+1)*sizeof(long));
	}

  for(int j = 0; j <= input; j++){
    for(int i = 0; i <= j; i++){
		if(i > 0 && j > 0 && output[i-1][j-1] > 0 && output[i][j-1] > 0){
      output[i][j] = output[i-1][j-1] + output[i][j-1];
    }else{
			output[i][j] = 1;
		}
		if(i < j){
			printf("%ld ",output[i][j]);
		}else{
			printf("%ld"\n,output[i][j]);
		}

	}

  }
  for(int i = 0;i < input+1; i++){
		free(output[i]);
	}
	free(output);
  return 0;
}
