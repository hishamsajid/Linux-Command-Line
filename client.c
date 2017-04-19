#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>

#define DATA "Half a league, half a league . . ."
#define BUFF_SIZE 1024


int main(int argc, char *argv[])
	{
	int sock;
	int count;
	int rval;

	struct sockaddr_in server;
	struct hostent *hp;
	char buff_s[BUFF_SIZE];
    char buff_r1[BUFF_SIZE];
    char buff_r2[BUFF_SIZE];
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

		rval = recv(sock,buff_r1,1024,0);
		buff_r1[rval-1] = '/0';

		if(rval < 0){
			perror("error recieving: ");
		}

		if(rval > 0){
			count = sprintf(buff_r2,"Recieved: %s\n",buff_r1);
			write(1,buff_r2,count);
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
