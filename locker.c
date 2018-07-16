#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <string.h>
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

struct locker_t {
  uint16_t id;
  int write_fd;
  int read_fd;
  bool locked;
};

struct queue_t {
  struct locker_t* head;
  size_t size;
};
typedef void (*signal_handler)(int);

bool isASpace(char c){
	return c == ' ' || c == '\r' || c == '\n';
}

char * trim(char *c) {
    char * e = c + strlen(c) - 1;
    while(*c && isASpace(*c)) c++;
    while(e > c && isASpace(*e)) *e-- = '\0';
    return c;
}

bool locked;

void lock(int sig){
	locked = true;
}
void unlock(int sig){
	locked = false;
}
//child process
void child(int read_fd,int write_fd){
  //no owner
  int owner = -1;
	int lebal;
	signal(SIGUSR1,lock);
	signal(SIGUSR2,unlock);
	while (1){
		read(read_fd,&lebal,4);
		if(lebal == 0){
			read(read_fd,&owner,4);
		}
		else if(lebal == 2){
		  write(write_fd,&owner,4);
		  write(write_fd,&locked,sizeof(bool));
		}
		else if(lebal == 3){
		  write(write_fd,&owner,4);
		}else if(lebal==4){
			exit(0);
		}else if(lebal == 5){
			locked = true;
		}else if(lebal == 6){
			locked = false;
		}
		wait(&lebal);
}
}


int main(int argc, char** argv) {
	pid_t parent_pid = getpid();
  //count the id of the last inputted locker
  int count_id = 1;
  //all of the ids free to use
  int* free_id = (int*)malloc(sizeof(int)*1);
  free_id[0] = count_id;
  int size = 1;
  //declare a queue of lockers
struct queue_t* queue = (struct queue_t*)malloc(1*sizeof(struct queue_t));;
queue->head = (struct locker_t*)malloc(0*sizeof(struct locker_t));
queue->size = 0;
//the command read from stdin
char command[100];
  while(fgets(command,100,stdin)){
	  fflush(stdin);
	  fflush(stdout);
    trim(command);
    if(!strcmp(command,"CREATE")){
      //declare the filedes for the pipes
      int fd1[2];
      int fd2[2];
      pipe(fd1);
      pipe(fd2);
      pid_t pid = fork();
      //child process
      if(pid == 0){
        close(fd1[1]);
        close(fd2[0]);
        child(fd1[0],fd2[1]);
      //parent process
		  return 0;
		}else if(getpid() == parent_pid){
        close(fd1[0]);
        close(fd2[1]);
        //increase the size of the queue
        queue->size++;
        //reallocate the memory for the queue
        queue->head = (struct locker_t*)realloc(queue->head,sizeof(struct locker_t)*queue->size);
        queue->head[queue->size-1].id = free_id[size-1];
        queue->head[queue->size-1].read_fd = fd2[0];
        queue->head[queue->size-1].write_fd= fd1[1];
		int lebal = 5;
		write(queue->head[queue->size-1].write_fd,&lebal,4);
        if (free_id[size-1] == count_id){
          count_id++;
          free_id[0] = count_id;
        }else{
          size--;
          free_id = (int*)realloc(free_id,sizeof(int)*size);
        }
        printf("New Locker Created: %d\n",queue->head[queue->size-1].id);
				printf("\n");
      }else{
		  return 0;
	  }
    }else if(!strcmp(command,"QUERYALL")){

		int lebal = 2;
        for(int i = 0; i < queue->size; i++){
          int owner_id;
          bool lock;
		  write(queue->head[i].write_fd,&lebal,4);
          read(queue->head[i].read_fd,&owner_id,4);
          read(queue->head[i].read_fd,&lock,sizeof(bool));
            printf("Locker ID: %d\n", queue->head[i].id);
            if(lock == true) printf("Lock Status: locked\n");
            else printf("Lock Status: unlocked\n");
            if(owner_id != -1) printf("Owner: %u\n",owner_id);
            else printf("Owner: unowned\n");
						printf("\n");
          }
    }else if(!strcmp(command,"QUIT")){
			//kill all the fucking Locker
			int lebal = 4;
			for(int i = 0; i < queue->size; i++){
					write(queue->head[i].write_fd,&lebal,4);
			}
			//free the memory
      free(free_id);
      free(queue->head);
      free(queue);
      return 0;
    }else{
      char** words = (char**)malloc(2*sizeof(char*));
        for(int i = 0; i < 2; i++){
          words[i] = (char*)malloc(200*sizeof(char));
        }
        strcpy(words[0],strtok(command," "));
        strcpy(words[1],strtok(NULL,"\0"));
        int id = atoi(words[1]);
        free(words[1]);
          if(!strcmp(words[0],"DELETE")){
						int lebal = 4;
            for(int i = 0; i < queue->size; i++){
              if(id == queue->head[i].id){
                write(queue->head[i].write_fd,&lebal,4);
								queue->size--;
                size++;
                free_id = (int*)realloc(free_id,sizeof(int)*size);
								queue->head = (struct locker_t*)realloc(queue->head,sizeof(struct locker_t)*queue->size);
                free_id[size-1] = id;
								printf("Locker %d Removed\n",id);
              }
            }
          }else if(!strcmp(words[0],"QUERY")){
						int lebal = 2;
            for(int i = 0; i < queue->size; i++){
              if(id == queue->head[i].id){
                int owner_id;
                bool lock;
								write(queue->head[i].write_fd,&lebal,4);
                read(queue->head[i].read_fd,&owner_id,4);
                read(queue->head[i].read_fd,&lock,sizeof(bool));
                  printf("Locker ID: %d\n", queue->head[i].id);
                  if(lock == true) printf("Lock Status: locked\n");
                  else printf("Lock Status: unlocked\n");
                  if(owner_id != -1) printf("Owner: %u\n",owner_id);
                  else printf("Owner: unowned\n");
              }
            }
						printf("\n");
          }else if(!strcmp(words[0],"LOCK")){
            for(int i = 0; i < queue->size; i++){
              if(id == queue->head[i].id){
								printf("Locker %d locked\n", id);
								int lebal = 5;
								write(queue->head[i].write_fd,&lebal,4);
				  			break;
              }
            }
						printf("\n");
          }else if(!strcmp(words[0],"UNLOCK")){
						for(int i = 0; i < queue->size; i++){
							if(id == queue->head[i].id){
								printf("Locker %d Unlocked\n",id);
								int lebal = 6;
								write(queue->head[i].write_fd,&lebal,4);
                break;
              }
            }

			printf("\n");
		}else if(!strcmp(words[0],"ATTACH")){

			int lebal = 3;
			for(int i = 0; i < queue->size; i++){
				int write_fd = queue->head[i].write_fd;
				int owner_id;
					write(write_fd,&lebal,4);
					read(queue->head[i].read_fd,&owner_id,4);
				if(owner_id == -1){
						lebal = 0;
						write(write_fd,&lebal,4);
						write(write_fd,&id,4);
						printf("Locker %d Owned By %d\n",(i+1),id);
						break;
					}
			 }
			 printf("\n");
		 }else if(!strcmp(words[0],"DETACH")){
			 int noOwner = -1;
					 int lebal = 0;
			 for(int i = 0; i < queue->size; i++){
				 if(id == queue->head[i].id){
					 int write_fd = queue->head[i].write_fd;
					 write(write_fd,&lebal,4);
					 write(write_fd,&noOwner,4);
					 printf("Locker %d Unowned\n",id);
						 break;
				 }
			 }
	 printf("\n");
		 }
          free(words[0]);
        free(words);
    }

  }
}
