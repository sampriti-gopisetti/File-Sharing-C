#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>

int main(){
    int socketfd=socket(AF_INET,SOCK_STREAM,0); //AF_INET: IPv4, SOCK_STREAM: TCP, 0: Default protocol
    if (socketfd==-1){
        perror("socket"); //Prints error message if socket creation fails
    }
    else{
        printf("Socket created successfully: %d\n",socketfd);
    }

    struct sockaddr_in addr;
    addr.sin_family=AF_INET;
    addr.sin_port=htons(5001); 
    addr.sin_addr.s_addr=INADDR_ANY; //Accept connections from any IP address

    int bind_status=bind(socketfd, (struct sockaddr *)&addr, sizeof(addr)); //Bind the socket to the specified address and port
    if (bind_status==-1){
        perror("bind");
    }
    else{
        printf("Bind successful\n");
    }
}