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
#include <arpa/inet.h>

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

struct communication {
	int input;
	int pipe_server_sub;
	int pipe_sub_server;
	int client_fd;

};

struct comms2 {
	int socket;
	char IP[50];
	int pid;
	int portno;
	int writefd;
	char *status;

};

struct pipe_input {
	int fd;
	int p_number;
	pid_t client_pid;
	int msgsock;

};

int p_number;
struct process p_list[20];
struct comms2 comms2_list[20];
struct communication comms_list[20];
int global_client_id;
int comm_itr = 0;

void *funct_arth(void *ptr);
void *funct_run(void *ptr);
void *funct_kill(void *ptr);
void *funct_list(void *ptr);
void *funct_read(void *ptr);
void *funct_server_listen(void *ptr);
void *funct_pipe_listen(void*ptr);


int main()
{
	int sock, length;
	struct sockaddr_in server;
	struct sockaddr_in client;
	int msgsock;
	char buff[BUFF_SIZE];
	char buff_2[BUFF_SIZE];
	char buff_copy[BUFF_SIZE];
	int rval;
	char *tkptr;
	int accept_fork;

	int server_counter = 0;
	char *server_ptr;
	char server_buff[BUFF_SIZE];
	
	pthread_t t_arth;
	pthread_t t_run;
	pthread_t t_kill;
	pthread_t t_listen;
	pthread_t t_pipe;


	int t_rval = 0;
	char *token;
	char *ptr;
	int count;
	double sum = 0;
	int f_val;
	int dummy = 0;

	
	pthread_create(&t_listen,NULL,funct_server_listen,(void*)&dummy);

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

		int len = sizeof(client);
		msgsock = accept(sock,(struct sockaddr *) &client,
		(socklen_t*)&len);
		//msgsock = accept(sock,0,0);
		
		int pipe_parser_sub[2]; //will take input from parser and output into sub-server
		
		pipe(pipe_parser_sub);
		
		//sub_server can write from sub to server
		//server_sub can wrtie from server to sub
		accept_fork = fork();

		if(accept_fork != 0){
			//MAIN SERVER
			comms2_list[comm_itr].pid = accept_fork;
			comms2_list[comm_itr].socket = msgsock;
			sprintf(comms2_list[comm_itr].IP,"%s",inet_ntoa(client.sin_addr));
			comms2_list[comm_itr].portno = ntohs(client.sin_port);
			comms2_list[comm_itr].writefd = pipe_parser_sub[1];
			comms2_list[comm_itr].status = "active";
			comm_itr++;
			close(pipe_parser_sub[0]); // close pipe for reading from server
			
		}

		if(accept_fork == 0){

		//SUB SERVER
		close(pipe_parser_sub[1]); // close pipe for writing from sub-server

		pid_t client_pid = getpid();
		int readfd = pipe_parser_sub[0];
		struct pipe_input input;
		input.fd = readfd;
		input.p_number = p_number;
		input.client_pid = client_pid;
		input.msgsock = msgsock;
		pthread_create(&t_pipe,NULL,funct_pipe_listen,(void*)&input);


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

						

				else if(strcmp("list",token)==0) {

				int itr = 0;

				while(itr<p_number) {

					count = sprintf(buff,"\nPID: %d\nName: %s\nStatus: %s \nClient ID: %d \n", p_list[itr].PID,
					p_list[itr].Pname,p_list[itr].status,p_list[itr].client_pid);
    				write(msgsock,buff,count);
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
	

		pthread_join(t_run,NULL);
		pthread_join(t_arth,NULL);
		pthread_join(t_kill,NULL);
		pthread_join(t_listen,NULL);
		pthread_join(t_pipe,NULL);

}


void *funct_pipe_listen(void *ptr){
char buff[BUFF_SIZE];
int fd;
int p_number;
int rval;
int pid;
int t_msgsock;
int client_pid;
char *token;
char *tkptr;
char *strptr;


struct pipe_input *input;
input = (struct pipe_input *)ptr;

fd = (*input).fd;
p_number = (*input).p_number;
client_pid = (*input).client_pid;
t_msgsock = (*input).msgsock;


rval = read(fd,buff,BUFF_SIZE);
buff[rval-1] = '\0';
write(1,buff,rval);

write(1,"we're here2",sizeof("we're here2"));

token = strtok_r(buff," ",&tkptr);

if(strcmp(token,"kill")==0){

	token = strtok_r(NULL," ",&tkptr);
	if(strcmp(token,"-all")==0){
		int itr=0;
		while(itr<p_number){

			if(client_pid == p_list[itr].client_pid) {
			    
				pid = p_list[itr].PID;
				kill(pid,SIGTERM);
        		p_list[itr].status = "deactivated";
        		write(t_msgsock,"\nproccess killed\n", sizeof("\nproccess killed\n"));                   
			
       			}
     		itr++;

			}
		}
	}

}



void *funct_server_listen(void *ptr){
int rval = 0;
char buff[BUFF_SIZE];
char *token;
char *tkptr;

while(TRUE){
rval = read(STDIN_FILENO,buff,BUFF_SIZE);
buff[rval-1] = '\0';

token = strtok_r(buff," ",&tkptr);

if(strcmp("kill",token)==0){

	token = strtok_r(NULL," ",&tkptr);

	if(strcmp("-port",token)==0){
	
		token = strtok_r(NULL," ",&tkptr);
		int portnumber = atoi(token);

		int itr = 0;
		while(itr<comm_itr){
	
			if(comms2_list[itr].portno == portnumber){
		
				int msgsock = comms2_list[itr].socket;
				write(msgsock,"kill -all",sizeof("kill -all"));
				
			}
			itr++;
		}
	
	}
}

else if(strcmp("show",token) == 0) {
	write(1,"Showing all clients: \n",sizeof("Showing all clients: \n"));
	int itr = 0;

	while(itr<comm_itr){

		rval = sprintf(buff,"\nClient PID: %d\nMsgsock: %d\nIP: %s \nPort no: %d \nwritefd: %d\nstatus: %s\n", 
		comms2_list[itr].pid,comms2_list[itr].socket,comms2_list[itr].IP,comms2_list[itr].portno,
		comms2_list[itr].writefd, comms2_list[itr].status);
    	write(1,buff,rval);
    	itr++;

			}
		}

else if(strcmp("message",token) == 0){
	int itr = 0;

	token = strtok_r(NULL,"-",&tkptr);
	write(1,token,sizeof(token));
	while(itr<comm_itr){
		int msgsock = comms2_list[itr].socket;
		
		write(msgsock,token,sizeof(token));
		
		itr++;


	}


}

else if(strcmp("disconnect",token)==0){
	int itr = 0;
	token = strtok_r(NULL," ",&tkptr);

	if(strcmp("-port",token)==0){
	
		token = strtok_r(NULL," ",&tkptr);
		int portnumber = atoi(token);
		
		while(itr<comm_itr){
	
			if(comms2_list[itr].portno == portnumber){
				int writefd = comms2_list[itr].writefd;
				int pid = comms2_list[itr].pid;
				int msgsock = comms2_list[itr].socket;
				comms2_list[itr].status = "deactivated";
				comm_itr = comm_itr-1;
				//write(msgsock,"kill -all",sizeof("kill -all"));
				
				//write(1,"connection terminated",sizeof("connection terminated"));	
				write(msgsock,"connection terminated",sizeof("connection terminated"));
				//close(msgsock);
				//kill(pid,SIGTERM);
				
			}
			itr++;
		}
	

	}

	if(strcmp("-IP",token)==0){

		token = strtok_r(NULL," ",&tkptr);
		
		while(itr<comm_itr){

			if(strcmp(comms2_list[itr].IP,token)==0){
				int pid = comms2_list[itr].pid;
				int msgsock = comms2_list[itr].socket;
				comms2_list[itr].status = "deactivated";
				//write(1,"connection terminated",sizeof("connection terminated"));
				write(msgsock,"connection terminated",sizeof("connection terminated"));
				//close(msgsock);
				//kill(pid,SIGTERM);

			}
			itr++;
		}

	}


}	else {

	write(1,"Incorrect command, please try again \n",sizeof("Incorrect command, please try again \n"));
}


	}
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
			if(client_pid != p_list[itr].client_pid) {

				write(1,"\nClient does not own this process\n",
				sizeof("\nClient does not own this process\n"));
				break;
			}

			else {
        		
				kill(pid,SIGTERM);
            	p_list[itr].status = "deactivated";
           		write(t_msgsock,"\nproccess killed\n", sizeof("\nproccess killed\n"));                   
				break;
				}
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
				if(client_pid != p_list[itr].client_pid) {

					write(1,"\nClient does not own this process\n",
					sizeof("\nClient does not own this process\n"));
					break;
				}
				else{
            	int pid = p_list[itr].PID;
            	p_list[itr].status = "deactivated";

                kill(pid,SIGTERM);

                write(t_msgsock,"\nproccess killed\n", sizeof("\nproccess killed\n"));

                break; }
                	}
				
            itr++;
        
			}
        
		 }


if(strcmp(token,"-all")==0) {

	int itr = 0;

    while(itr<p_number) {
		if(p_list[itr].client_pid == client_pid){
			if(client_pid != p_list[itr].client_pid){
				itr++;
			} else {

		int pid = p_list[itr].PID;          
		p_list[itr].status = "deactivated";
        kill(pid,SIGTERM); }
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


