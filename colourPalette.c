#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

	int width;
	int height;
	int magic;

int file_size(FILE *file) {
	int fp=ftell(file);
    fseek(file,0L,SEEK_END);
    int size=ftell(file);
    fseek(file,fp,SEEK_SET);
    return size;
}


typedef struct pixel_t pixel_t;

struct pixel_t {
	uint8_t red;
	uint8_t green;
	uint8_t blue;
	uint8_t empty;
	int index;
};


int main(int argc, char** argv) {
	if(argc == 1){
		printf("No Filename Specified\n");
		return 0;
	}
	FILE* file = fopen(argv[1],"r");
	if(!file){
		printf("File Does Not Exist\n");
		return 0;
	}


int size = file_size(file);
	fread(&width,4,1,file);
	fread(&height,4,1,file);
	fread(&magic,2,1,file);
	if(magic != 60535){
		printf("Invalid Image Header\n");
		return 0;
	}
	if(size != 10+width*height*sizeof(int)){
		printf("Invalid Image Data\n");
		return 0;
	}

	pixel_t** field = (pixel_t**)malloc(height*sizeof(pixel_t*));
	for(int i = 0; i < height; i++){
		field[i] = (pixel_t*)malloc(width*sizeof(pixel_t));
	}

	int num = 0;
	for(int j = 0; j < height; j++){
		for(int i = 0; i < width; i++){
			fread(&field[i][j].red,1,1,file);
			fread(&field[i][j].green,1,1,file);
			fread(&field[i][j].blue,1,1,file);
			fread(&field[i][j].empty,1,1,file);
			field[i][j].index = -1;
			//get the index
			int mark = 0;
			for(int k = 0; k < height; k++){
				for(int l = 0; l < width; l++){
					if(i == l&&j == k){
					mark = 1;
					break;
				}
				if(field[i][j].red == field[l][k].red&&field[i][j].green == field[l][k].green&&field[i][j].blue == field[l][k].blue&&field[i][j].empty == field[l][k].empty){
					field[i][j].index = field[l][k].index;
					mark = 1;
					break;
				}
			}
				if(mark == 1){
				break;}

			}
			if(field[i][j].index == -1){
				field[i][j].index = num;
				num++;
			}

		}
	}

	for(int j = 0; j < height; j++){
		printf("[");
		for(int i = 0; i < width; i++){
			if(i == width - 1){
				printf(" %d ]\n",field[i][j].index);
				continue;
			}
				printf(" %d,",field[i][j].index);
			}
		}
	for(int i = 0; i < height; i++){
		free(field[i]);
	}
	free(field);
	return 0;
}
