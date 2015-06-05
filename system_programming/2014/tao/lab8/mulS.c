#include "unistd.h"
#include "errno.h"
#include "sys/types.h"
#include "sys/socket.h"
#include "netinet/in.h"
#include "netdb.h"

#include "stdlib.h"
#include "stdio.h"
#include "string.h"
#include "strings.h"
#include "sys/wait.h"


//Function Prototypes
void myabort(char *);

//Some Global Variables
int serverport = 3000;
char * eptr = NULL;
int listen_socket, client_socket;
struct sockaddr_in Server_Address, Client_Address;
int result,i;
socklen_t csize;
pid_t processid;
int childcount = 0;


//main()
int
main(int argc, char **argv){

char buf[100];
char tmp[100];
char * ptr;
int n, sent, length;

//Step 0: Process Command Line
if (argc > 2){
myabort("Usage: server ");
}
if (argc == 2){
serverport = (int) strtol(argv[1], &eptr, 10);
if (*eptr != '\0') myabort("Invalid Port Number!");
}

//Step 1: Create a socket
listen_socket = socket(PF_INET, SOCK_STREAM, 0);
if (listen_socket == -1) myabort("socket()");


//Step 2: Setup Address structure
bzero(&Server_Address, sizeof(Server_Address));
Server_Address.sin_family = AF_INET;
Server_Address.sin_port = htons(serverport);
Server_Address.sin_addr.s_addr = INADDR_ANY;


//Step 3: Bind the socket to the port
result = bind(listen_socket, (struct sockaddr *) &Server_Address, sizeof(Server_Address));
if (result == -1) myabort("bind()");

//Step 4:Listen to the socket
result = listen(listen_socket, 1);
if (result == -1) myabort("listen()");



printf("\nThe forkserver :%d\n",ntohs(Server_Address.sin_port));
fflush(stdout);
//Step 5: Setup an infinite loop to make connections
while(1){


//Accept a Connection
csize = sizeof(Client_Address);
client_socket = accept( listen_socket,(struct sockaddr *) &Client_Address,&csize);
if (client_socket == -1) myabort("accept()");

printf( "\nClient Accepted!\n" );


//fork this process into a child and parent
processid = fork();

//Check the return value given by fork(), if negative then error,
//if 0 then it is the child.
if ( processid == -1){
myabort("fork()");
}else if (processid == 0){
/*Child Process*/

close(listen_socket);
//loop until client closes
while (1){



//read string from client
bzero(&buf, sizeof(buf));
do{
bzero(&tmp, sizeof(tmp));
n = read(client_socket,(char *) &tmp, 100);
//cout << "server: " << tmp;
tmp[n] = '\0';
if (n == -1) myabort("read()");
if (n == 0) break;
strncat(buf, tmp, n-1);
buf[n-1] = ' ';
} while (tmp[n-1] != '\n');

buf[ strlen(buf) ] = '\n';

printf( "From client: %s",buf);

if (n == 0) break;


//write string back to client
sent = 0;
ptr = buf;
length = strlen(buf);

//the vowels in the message are converted into upper case. 
for( i = 0; ptr[ i ]; i++)
{
if( ptr[i]=='a' || ptr[i]=='e' || ptr[i]=='i' || ptr[i]=='o' || ptr[i]=='u' )
ptr[ i ] = toupper( ptr[ i ] );
else
ptr[ i ] = ptr[ i ] ;

} 


printf( "To client: %s",ptr);
while (sent < length ){
n = write(client_socket, ptr, strlen(ptr) );
if ( n == -1) myabort("write()");
sent += n;
ptr += n;
}
}//end inner while

close(client_socket);

//Child exits
exit(0);
}


//Parent Process

printf("\nChild process spawned with id number: %d",processid );
//increment the number of children processes
childcount++;
while(childcount){
processid = waitpid( (pid_t) - 1, NULL, WNOHANG );
if (processid < 0) myabort("waitpid()");
else if (processid == 0) break;
else childcount--;
} 

}
close(listen_socket);

exit(0);

}


void myabort(char * msg){
printf("Error!: %s" , msg);
exit(1);
}
