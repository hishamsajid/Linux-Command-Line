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
#include <pthread.h>

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

#define TRUE 1
#define BUFF_SIZE 5000

struct process {

	char *Pname;
	pid_t client_pid;
	int PID;
	char *status;
	float *s_time;
	float *e_time;

};

struct struct_input {

	int sock;
	char *buff;
	pid_t client_pid;

};

int p_number;
struct process p_list[20];


void *funct_arth(void *ptr);
void *funct_run(void *ptr);
void *funct_kill(void *ptr);
void *funct_list(void *ptr);
void *funct_read(void *ptr);


int main()
{
	int sock, length;
	struct sockaddr_in server;
	int msgsock;
	char buff[BUFF_SIZE];
	char buff_2[BUFF_SIZE];
	char buff_copy[BUFF_SIZE];
	int rval;
	char *tkptr;
	int accept_fork;
	
	pthread_t t_arth;
	pthread_t t_run;
	pthread_t t_kill;
	pthread_t t_list;

	int t_rval = 0;
	char *token;
	char *ptr;
	int count;
	double sum = 0;
	int f_val;

	

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
		
		pid_t client_pid = getpid();


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
				
				count = sprintf(buff_2,"Recieved: %s \n",buff);
				write(1,buff_2,count);

			//enter command line server code here

				buff[rval-1] = '\0';
				strcpy(buff_copy,buff);
				token = strtok_r(buff," ",&tkptr);

				//arithematic functions
				if(strcmp("add",token)==0 || strcmp("mult",token)==0
				|| strcmp("sub",token)==0) {
				
				struct struct_input input;
				input.sock = msgsock;
				input.buff = malloc(50);
				strcpy(input.buff,buff_copy);
				
				
				t_rval = pthread_create(&t_arth,NULL,funct_arth,(void*)&input);
				
                }

				//create process
                else if(strcmp("run",token)==0) {
				
			    token = strtok_r(NULL," ",&tkptr);
				
				struct struct_input input;
				input.buff = malloc(50);
				strcpy(input.buff,token);
				input.sock = msgsock;
				input.client_pid = client_pid;

				t_rval = pthread_create(&t_run,NULL,funct_run,(void*)&input);
				

                }

				
				else if(strcmp("disconnect",token)==0){
				int itr = 0;
				
 				while(itr<p_number) {
					if(p_list[itr].client_pid == client_pid){
						int pid = p_list[itr].PID;          
						p_list[itr].status = "deactivated";
        				kill(pid,SIGTERM);
						}
        			itr++;
            		}
					close(msgsock);
					raise(SIGTERM);
					break;

					
				}
		

				else if(strcmp("list",token)==0) {

				int itr = 0;

				while(itr<p_number) {

					count = sprintf(buff,"\nPID: %d\nName: %s\nStatus: %s \nClient ID: %d \n", p_list[itr].PID,
					p_list[itr].Pname,p_list[itr].status,p_list[itr].client_pid);
    				write(msgsock,buff,count);
    				/*
					count = sprintf(buff,"\nName: %s\n", p_list[itr].Pname);
    				write(msgsock,buff,count);
    				count = sprintf(buff,"\nStatus: %s\n", p_list[itr].status);
    				write(msgsock,buff,count);
					count = sprintf(buff,"\nClient ID: %d\n", p_list[itr].client_pid);
    				write(msgsock,buff,count);

    				write(msgsock,"\n",sizeof("\n")-1);

					write(msgsock,"Processes listed",sizeof("Processes listed"));
					*/
    				itr++;

					}

				}

				else if(strcasecmp("kill",token) == 0) {


				struct struct_input input;
				input.buff = malloc(50);
				strcpy(input.buff,buff_copy);
				input.sock = msgsock;

				t_rval = pthread_create(&t_kill,NULL,funct_kill,(void*)&input);



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



void *funct_kill(void *ptr) {

char *buff;
int t_msgsock;
pid_t client_pid;
double sum = 0;
struct struct_input *kill_;
kill_ = (struct struct_input *)ptr;
char *token;
char *tkptr;
char *strptr;
int count;


t_msgsock = (*kill_).sock;
buff = (*kill_).buff;
client_pid = (*kill_).client_pid;



token = strtok_r(buff," ",&tkptr);
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

            write(t_msgsock,"\nproccess killed\n", sizeof("\nproccess killed\n"));
                           
			break;
                	}
            itr++;

        	}

        }

if(strcmp(token,"-n") == 0)	{

	token = strtok_r(NULL," ",&tkptr);
    int itr =0;
	char *deactivated = (char*)malloc(50);
	strcpy(deactivated,"deactivated");
	//printf(deactivated);
    	while(itr<p_number){

            if(strcmp(token,p_list[itr].Pname) == 0){
				

            	int pid = p_list[itr].PID;
            	p_list[itr].status = "deactivated";

                kill(pid,SIGTERM);

                write(t_msgsock,"\nproccess killed\n", sizeof("\nproccess killed\n"));

                break;
                	}
				
            itr++;
        
			}
        
		 }


if(strcmp(token,"-all")==0) {

	int itr = 0;

    while(itr<p_number) {
		if(p_list[itr].client_pid == client_pid){
		int pid = p_list[itr].PID;          
		p_list[itr].status = "deactivated";
        kill(pid,SIGTERM);
		}
        itr++;
            }
        write(t_msgsock,"\nAll proccesses killed\n", sizeof("\nAll proccesses killed\n"));
    	}

else {
					
		write(t_msgsock,"\n Incorrect command, try again \n",
		sizeof("\n Incorrect command, try again \n"));

		}


}

		







void *funct_arth(void *ptr){

	char *buff;
	int t_msgsock;
	double sum = 0;
	struct struct_input *arth;
	arth = (struct struct_input *)ptr;
	char *token;
	char *tkptr;
	char *strptr;
	int count;

	t_msgsock = (*arth).sock;
	buff = (*arth).buff;
	//strcpy(buff,(*arth).buff);
	printf("Recieved into buff %s",buff);

	token = strtok_r(buff," ",&tkptr);

	if(strcmp(token,"add")==0) {

	write(1,"An addition function\n",sizeof("An addition function\n"));
	while(token!=NULL){
		double x = strtod(token,&strptr);
		sum=sum+x;
		token = strtok_r(NULL," ",&tkptr);				
		}

	}


    else if(strcmp("mult",token)==0) {
            				
	sum = 1;
	token = strtok_r(NULL," ",&tkptr);

    while(token!=NULL){            
    	sum = sum*strtod(token,&strptr);
		token = strtok_r(NULL," ",&tkptr);
		}

	}


	else if(strcmp("sub",token)==0) {

    	sum= atof(strtok_r(NULL," ",&tkptr));

        while(token!=NULL){
            sum=sum-strtod(token,&tkptr);
            token=strtok_r(NULL," ",&tkptr);
            }
		
	}

    //writing back to client
    count = sprintf(buff,"%lf",sum);
    write(1,buff,count);
    write(t_msgsock,buff,count);
				
	
}


void *funct_run(void *ptr){

	char *name;
	int t_msgsock;
	pid_t client_pid;
	struct struct_input *run;
	run = (struct struct_input *)ptr;

	t_msgsock = (*run).sock;
	name = (*run).buff;
	client_pid = (*run).client_pid;

	

	printf("we have %d and %s ",t_msgsock,name);
	
	
	int f_val = 0;
	
	f_val = fork();
		if(f_val != 0) {
			//mutex
			//pthread_mutex_lock(&mutex);
			p_list[p_number].PID = f_val;
			p_list[p_number].Pname = (char*) malloc(50);
			strcpy(p_list[p_number].Pname,name);
	        p_list[p_number].status = (char*) malloc(50);
	        p_list[p_number].status = "active";
	        p_list[p_number].s_time = (float*)clock();
			p_list[p_number].client_pid = client_pid;
	       // pthread_mutex_unlock(&mutex);
			p_number++;
			
				}

         else if(f_val == 0){
			//pthread_mutex_lock(&mutex);
			write(t_msgsock,"Successfully forked\n", sizeof("Successfully forked\n"));
        	execlp(name,name,NULL);
			//pthread_mutex_unlock(&mutex);
			write(t_msgsock,"unsuccesful execution\n", sizeof("unsuccesful execution\n"));
                }

		if(f_val != 0) {

			//write(global_msgsock,"Successfully executed", sizeof("Successfully executed"));

				}

		}


