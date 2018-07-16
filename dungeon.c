#include <stdio.h>
#include<stdlib.h>
#include<string.h>
#include<stdbool.h>
typedef struct room_t room_t;

struct room_t{
  char* name;
  room_t* NORTH;
  room_t* SOUTH;
  room_t* EAST;
  room_t* WEST;
};

int countRooms;

bool isASpace(char c){
	return c == ' ' || c == '\r' || c == '\n';
}

char * trim(char *c) {
    char * e = c + strlen(c) - 1;
    while(*c && isASpace(*c)) c++;
    while(e > c && isASpace(*e)) *e-- = '\0';
    return c;
}

room_t* searchroom(room_t* dungeon,char* roomname){
  for(int i = 0; i < countRooms; i++){
    if(!strcmp((dungeon+i)->name,roomname))
    return (room_t*)(dungeon+i);
  }
  return NULL;
}

void display(room_t* room){
  printf("\n");
  printf("%s\n",room->name);
  if(room->NORTH){
    printf(" ---N--- \n");
  }else{
  printf(" ------- \n");}
  printf("|       |\n");
  printf("|       |\n");
  if(room->WEST && room -> EAST){
    printf("W       E\n");
  }else if(room->WEST){
    printf("W       |\n");
  }else if(room -> EAST){
    printf("|       E\n");
  }
  else{
    printf("|       |\n");}
  printf("|       |\n");
  printf("|       |\n");
  if(room->SOUTH){
    printf(" ---S--- \n");
  }else{
  printf(" ------- \n");
  }
  printf("\n");
}

int main(int argc, char** argv) {
  if(argc == 1){
		printf("No Level File Specified\n");
		return 0;
	}
	FILE* file = fopen(argv[1],"r");
	if(!file){
		printf("No Level File Specified\n");
		return 0;
	}

  //read from the file

  //read the firstline
  //get the number and the name of the rooms
  char rooms[100];
  fgets(rooms,100,file);
  countRooms = 1;
  for(int i = 0; i < strlen(rooms); i++){
    if(*(rooms + i) == ' '){
      countRooms++;
    }
  }
  room_t* dungeon = (room_t*)malloc(countRooms*sizeof(room_t));
  dungeon->name = trim(strtok(rooms," "));
	(dungeon)->NORTH = NULL;
   (dungeon)->SOUTH = NULL;
   (dungeon)->EAST = NULL;
   (dungeon)->WEST = NULL;
  for(int i = 1; i < countRooms; i++){
   (dungeon+i)->name = trim(strtok(NULL, " "));
   (dungeon+i)->NORTH = NULL;
   (dungeon+i)->SOUTH = NULL;
   (dungeon+i)->EAST = NULL;
   (dungeon+i)->WEST = NULL;
  }

  //read the rest lines
  char info[100];
  char* from;
  char* to;
  char* direct;
  while(!feof(file)){
    fgets(info,100,file);
	  if(feof(file))break;
    from = trim(strtok(info," > "));
    direct = strtok(NULL," > ");
    to = strtok(NULL," > ");
	  if(!from || !to){
		  continue;
	  }
	  from = trim(from);
	  to = trim(to);
	  direct = trim(direct);
    room_t* fromRoom = searchroom(dungeon,from);
    room_t* toRoom = searchroom(dungeon,to);
	  if(!fromRoom){
		  printf("%s",from);
		  free(dungeon);
		  return 0;
	  }if(!toRoom){
		  printf("%s",to);
		  free(dungeon);
		  return 0;
	  }
    if(!strcmp(direct, "NORTH")){
      fromRoom->NORTH = toRoom;

    }else if(!strcmp(direct,"SOUTH")){
      fromRoom->SOUTH = toRoom;

    }else if(!strcmp(direct, "WEST")){
      fromRoom->WEST = toRoom;

    }else if(!strcmp(direct, "EAST")){
      fromRoom->EAST = toRoom;

    }

  }

//display the start room
room_t* current = dungeon;
display(current);
char command[10];

  while(true){
    fgets(command,10,stdin);
    if(!strcmp(command,"SOUTH\n")){
      if(!current->SOUTH){
      printf("No Path This Way\n");
       display(current);
      continue;
    }
      current = current->SOUTH;
      display(current);
    }else if(!strcmp(command,"NORTH\n")){
      if(!current->NORTH){
      printf("No Path This Way\n");
       display(current);
      continue;
    }
      current = current->NORTH;
      display(current);
    }else if(!strcmp(command,"WEST\n")){
      if(!current->WEST){
      printf("No Path This Way\n");
       display(current);
      continue;
    }
      current = current->WEST;
      display(current);
    }else if(!strcmp(command,"EAST\n")){
      if(!current->EAST){
      printf("No Path This Way\n");
       display(current);
      continue;
    }
    current = current->EAST;
      display(current);
    }else if(!strcmp(command,"QUIT\n")){
		free(dungeon);
      return 0;
    }else{
      printf("What?\n");
		display(current);
		free(dungeon);
  		return 0;
    }
  }

  free(dungeon);
  return 0;
}
