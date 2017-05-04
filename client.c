#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <pthread.h>

#define DATA "Half a league, half a league . . ."
#define BUFF_SIZE 1024

void *read_thread(void *ptr){



}

void *write_thread(void *ptr){



}



int main(int argc, char *argv[])
	{
	int sock;
	int count;
	int rval;
//	pthread_t thread1;
//	pthread_t thread2;

	struct sockaddr_in server;
	struct hostent *hp;
	char buff_s[BUFF_SIZE];
    char buff_r1[BUFF_SIZE];
    char buff_r2[BUFF_SIZE];
//	write(1,"Enter IP & Port no: ",sizeof("Enter IP & Port no: "));
//	read(0,)

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
	hp = gethostbyname(argv[1]);
	if (hp == 0) {
		fprintf(stderr, "%s: unknown host\n", argv[1]);
		exit(2);
	}
	bcopy(hp->h_addr, &server.sin_addr, hp->h_length);
	server.sin_port = htons(atoi(argv[2]));

	if (connect(sock,(struct sockaddr *) &server,sizeof(server)) < 0) {
		perror("connecting stream socket");
		exit(1);
	}
	while(1) {
		//sending data to server
		write(1,"Enter Command: ",sizeof("Enter Command: "));
		count = read(0,buff_s,1024);
		rval = write(sock,buff_s,count);

		if(rval < 0) {
			perror("writing failed: ");
		}

		if(rval > 0) {
			count = sprintf(buff_s,"Data sent: %d\n",rval);
			write(1,buff_s,count);

		}


		//recieve reply from server

		rval = read(sock,buff_r1,5000);

	
		buff_r1[rval-1] = '\0';

		if(rval < 0){
			perror("error recieving: ");
		}

		if(rval > 0){
			count = sprintf(buff_r2,"Recieved: %s\n",buff_r1);
			write(1,buff_r2,count);
			//fflush(NULL);
		}





/*
	if (write(sock, DATA, sizeof(DATA)) < 0)
		perror("writing on stream socket");

	else{

		getchar();

	}
*/
	}
	close(sock);
}
