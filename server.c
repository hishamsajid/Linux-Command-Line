#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <signal.h>

#define TRUE 1
#define BUFF_SIZE 5000

struct process {

	char *Pname;
	int PID;
	char *status;
	float *s_time;
	float *e_time;

};

int p_number;


int main()
{
	int sock, length;
	struct sockaddr_in server;
	int msgsock;
	char buff[BUFF_SIZE];
	char buff_2[BUFF_SIZE];
	int rval;
	char *tkptr;
	int accept_fork;


	char *token;
	char *ptr;
	int count;
	double sum = 0;
	int f_val;
	struct process p_list[20];
	

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
		
		accept_fork = fork();

		if(accept_fork == 0){

		if (msgsock == -1)
			perror("accept");
		
		else do {
			bzero(buff, sizeof(buff));
			if ((rval = read(msgsock, buff, 1024)) < 0){
				perror("reading stream message");}


			if (rval == 0) {
				printf("Ending connection\n");
			}

			else {
				//printf("-->%s\n", buff);
				//fflush(NULL);
				count = sprintf(buff_2,"Recieved: %s \n",buff);
				write(1,buff_2,count);

			//enter command line server code here

				buff[rval-1] = '\0';
				token = strtok_r(buff, " ",&tkptr);

				if(strcmp("add",token)==0) {
				write(1,"An addition function\n",sizeof("An addition function\n"));
				
				
				while(token!=NULL){
				double x = strtod(token,&ptr);
				sum=sum+x;
				token = strtok_r(NULL," ",&tkptr);

				}

            //writing back to client

                count = sprintf(buff,"%lf",sum);
                write(1,buff,count);
                write(msgsock,buff,count);
				//fflush(msgsock);


                }
                else if(strcmp("sub",token)==0) {

                sum= atof(strtok_r(NULL," ",&tkptr));

                while(token!=NULL){

                    sum=sum-strtod(token,&ptr);
                    token=strtok_r(NULL," ",&tkptr);

                    }

                count = sprintf(buff,"%lf",sum);
                write(1,buff,count);
                write(msgsock,buff,count);


               }


               else if(strcmp("mult",token)==0) {
            	
				
			    sum = 1;
				token = strtok_r(NULL," ",&tkptr);

                while(token!=NULL){
                    
                    sum = sum*strtod(token,&ptr);
					token = strtok_r(NULL," ",&tkptr);
					}

                count = sprintf(buff,"%lf",sum);
                write(1,buff,count);
                write(msgsock,buff,count);


                }

				

                else if(strcmp("run",token)==0) {


                token = strtok_r(NULL," ",&tkptr);

                f_val = fork();
				if(f_val != 0) {

					p_list[p_number].PID = f_val;
					p_list[p_number].Pname = (char*) malloc(50);
					strcpy(p_list[p_number].Pname,token);
	        		p_list[p_number].status = (char*) malloc(50);
	                p_list[p_number].status = "active";
	                p_list[p_number].s_time = (float*)clock();
	                p_number++;
				}

                else if(f_val == 0){

                        execlp(token,token,NULL);

                    }

				if(f_val != 0) {

					write(msgsock,"Successfully executed", sizeof("Successfully executed"));

				}


                }

				else if(strcmp("list",token)==0) {

				int itr = 0;

				while(itr<p_number) {

					count = sprintf(buff,"\nPID: %d\n", p_list[itr].PID);
    				write(msgsock,buff,count);
    				count = sprintf(buff,"\nName: %s\n", p_list[itr].Pname);
    				write(msgsock,buff,count);
    				count = sprintf(buff,"\nStatus: %s\n", p_list[itr].status);
    				write(msgsock,buff,count);

    				write(msgsock,"\n",sizeof("\n")-1);

					write(msgsock,"Processes listed",sizeof("Processes listed"));

    				itr++;

					}

				}

				else if(strcasecmp("kill",token) == 0) {

                token = strtok_r(NULL," ",&tkptr);

                    if(strcmp(token,"-id")==0) {

                    token = strtok_r(NULL," ",&tkptr);
                    int pid = atoi(token);
                    count = sprintf(buff,"%d",pid);
                    write(1,buff,count);


                    int itr = 0;

                    while(itr<p_number) {
                       
                        if(pid == p_list[itr].PID) {

                            kill(pid,SIGTERM);
                            p_list[itr].status = "deactivated";

                            write(msgsock,"\nproccess killed\n", sizeof("\nproccess killed\n"));
                           
							break;
                            }
                    itr++;

                        }

                    }

                 if(strcmp(token,"-n") == 0){

                token = strtok_r(NULL," ",&tkptr);
                int itr =0;

                while(itr<p_number){

                    if(strcmp(token,p_list[itr].Pname)==0){

                        int pid = p_list[itr].PID;
                        p_list[itr].status = "deactivated";

                        kill(pid,SIGTERM);

                        write(msgsock,"\nproccess killed\n", sizeof("\nproccess killed\n"));

                        break;
                        }
                    itr++;
                	}
                }


                 if(strcmp(token,"-all")==0) {

                int itr = 0;

                while(itr<p_number) {

                    int pid = p_list[itr].PID;
                    p_list[itr].status = "deactivated";
                    kill(pid,SIGTERM);
                    itr++;
                    }
                write(msgsock,"\nAll proccesses killed\n", sizeof("\nAll proccesses killed\n"));
                }

				else {
					
					write(msgsock,"\n Incorrect command, try again \n",
					sizeof("\n Incorrect command, try again \n"));

				}


            }

			else {
				write(msgsock,"\n Incorrect command, try again \n",
				sizeof("\n Incorrect command, try again \n"));

			}


//----------------------------

			}
		} while (rval != 0);
		close(msgsock);
		fflush(NULL);
		}
	} while (TRUE);

}
