#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <pthread.h>

#define BUFF_SIZE 5000

//pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIIZER;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

void *read_thread(void *ptr){

char buff_s[BUFF_SIZE];
int rval = 0;
int count = 0;
char *token;
int sock = *((int*)ptr);

while(1){
	
	write(1,"Enter Command: ",sizeof("Enter Command: "));
	count = read(0,buff_s,5000);
	//token = strtok(buff_s," ");
	
	
	rval = write(sock,buff_s,count);
	
	if(rval < 0) {
	
		perror("writing failed: ");
	
		}	

	if(rval > 0) {	
	
		count = sprintf(buff_s,"Data sent: %d\n",rval);
		write(1,buff_s,count);

		}
	
	}

}

void *write_thread(void *ptr){



int count = 0;
int rval = 0;
char buff_r1[5000];
char buff_r2[5000];
int sock = *((int*)ptr);

while(1) {

	//pthread_mutex_lock(&mutex);
	rval = read(sock,buff_r1,1024);	
	buff_r1[rval-1] = '\0';
	

	if(rval < 0){
	
		perror("error recieving: ");
	
	}

	if(rval > 0){
	
		count = sprintf(buff_r2,"Recieved: %s\n",buff_r1);
		write(1,buff_r2,count);
	
		}
	
	} close(sock);
	//pthread_mutex_unlock(&mutex);

}



int main()
	{
	int sock;
	int count;
	int rval;
	pthread_t thread_read;
	pthread_t thread_write;

	//struct thread_input input_list[2];

	struct sockaddr_in server;
	struct hostent *hp;
	char buff_s[BUFF_SIZE];
    char buff_r1[BUFF_SIZE];
    char buff_r2[BUFF_SIZE];
	char buff_ip_port[BUFF_SIZE];
	char *IP;
	char *Port;
	int t_rval = 0;
	int t_rval2 = 0;

	write(1,"Enter IP & Port no: ",sizeof("Enter IP & Port no: "));
	rval = read(0,buff_ip_port,BUFF_SIZE);
	buff_ip_port[rval -1] = '\0';
	IP = strtok(buff_ip_port," ");
	Port = strtok(NULL," ");


	write(1," Hiham Bin Sajid -09383 \n Version 0.0.1 \n All rights reserved \n \n",
	sizeof(" Hiham Bin Sajid -09383 \n Version 0.0.1 \n All rights reserved \n \n"));

	write(1,"add -> addition \n",sizeof("add -> addition \n"));
	write(1,"sub -> subtraction \n",sizeof("add -> subtraction \n"));
	write(1,"mult -> multiplication \n",sizeof("add -> multiplication \n"));
	write(1,"list -> list all processes \n \n",
	sizeof("list -> list all processes \n \n"));
	write(1,"	list -r -> list all running processes \n",
		sizeof("	list -r -> list all running processes \n"));
	write(1,"kill variations\n \n",sizeof("kill variations\n \n"));
	write(1,"	kill -id\n",sizeof("	kill -id\n"));
	write(1,"	kill -n\n",sizeof("	kill -n\n"));
	write(1,"	kill -all\n",sizeof("	kill -all\n"));
				


	/* Create socket */
	sock = socket(AF_INET, SOCK_STREAM, 0);
	if (sock < 0) {
		perror("opening stream socket");
		exit(1);
	}
	/* Connect socket using name specified by command line. */
	server.sin_family = AF_INET;
	
	hp = gethostbyname(IP);
	
	if (hp == 0) {
		fprintf(stderr, "%s: unknown host\n", IP);
		exit(2);
	}
	bcopy(hp->h_addr, &server.sin_addr, hp->h_length);
	server.sin_port = htons(atoi(Port));

	if (connect(sock,(struct sockaddr *) &server,sizeof(server)) < 0) {
		perror("connecting stream socket");
		exit(1);
	}
		//sending data to server
		//write(1,"Enter Command: ",sizeof("Enter Command: "));
		t_rval = pthread_create(&thread_read,NULL,read_thread,(void*)&sock);
		
		if(t_rval != 0) {

			perror("Error is: ");

		}

		//recieve reply from server
		
		t_rval2 = pthread_create(&thread_write,NULL,write_thread,(void*)&sock);
		
		if(t_rval2 != 0) {

			perror("Error is: ");

		}

		
		pthread_join(thread_read,NULL);
		pthread_join(thread_write,NULL);
		

}
