#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>

#define TRUE 1
#define BUFF_SIZE 1024

struct process {

	char *Pname;
	int PID;
	char *status;
	float *s_time;
	float *e_time;

};



int main()
{
	int sock, length;
	struct sockaddr_in server;
	int msgsock;
	char buff[BUFF_SIZE];
	char buff_2[BUFF_SIZE];
	int rval;


	char *token;
	char *ptr;
	int count;
	int sum = 0;
	int f_val;
	struct process p_list[20];
	int p_number;

	/* Create socket */
	sock = socket(AF_INET, SOCK_STREAM, 0);
	if (sock < 0) {
		perror("opening stream socket");
		exit(1);
	}
	/* Name socket using wildcards */
	server.sin_family = AF_INET;
	server.sin_addr.s_addr = INADDR_ANY;
	server.sin_port = 0;
	if (bind(sock, (struct sockaddr *) &server, sizeof(server))) {
		perror("binding stream socket");
		exit(1);
	}
	/* Find out assigned port number and print it out */
	length = sizeof(server);
	if (getsockname(sock, (struct sockaddr *) &server, (socklen_t*) &length)) {
		perror("getting socket name");
		exit(1);
	}
	printf("Socket has port #%d\n", ntohs(server.sin_port));
	fflush(stdout);

	/* Start accepting connections */
	listen(sock, 5);
	do {
		msgsock = accept(sock, 0, 0);
		if (msgsock == -1)
			perror("accept");
		else do {
			bzero(buff, sizeof(buff));
			if ((rval = read(msgsock, buff, 1024)) < 0)
				perror("reading stream message");


			if (rval == 0)
				printf("Ending connection\n");


			else {
				printf("-->%s\n", buff);
				count = sprintf(buff_2,"Recieved: %s \n",buff);
				write(1,buff_2,count);

			//enter command line server code here

				buff[rval-1] = '\0';
				token = strtok(buff, " ");

				if(strcmp("add",token)==0) {

				while(token!=NULL){

				sum=sum+strtod(token,&ptr);
				token = strtok(NULL," ");

				}

            //writing back to client

                count = sprintf(buff,"%d",sum);
                write(1,buff,count);
                write(msgsock,buff,count);


			}
                else if(strcmp("sub",token)==0) {

                sum= atof(strtok(NULL," "));

                while(token!=NULL){

                    sum=sum-strtod(token,&ptr);
                    token=strtok(NULL," ");

                    }

                count = sprintf(buff,"%d",sum);
                write(1,buff,count);
                write(msgsock,buff,count);


               }


               else if(strcmp("mult",token)==0) {

                sum= strtod(strtok(NULL," "),&ptr);

                while(token!=NULL){

                    sum=sum*strtod(strtok(NULL," "),&ptr);

                    }

                count = sprintf(buff,"%d",sum);
                write(1,buff,count);
                write(msgsock,buff,count);


                }

                else if(strcmp("exec",token)==0) {


                token = strtok(NULL," ");

                f_val = fork();

				p_list[p_number].PID = f_val;
				p_list[p_number].Pname = (char*) malloc(50);
				strcpy(p_list[p_number].Pname,token);
        		p_list[p_number].status = (char*) malloc(50);
                p_list[p_number].status = "active";
                p_list[p_number].s_time = (float*)clock();
                p_number++;

                    if(f_val == 0){

                        execlp(token,token,NULL);

                    }


                }




			}
		} while (rval != 0);
		close(msgsock);
	} while (TRUE);

}
