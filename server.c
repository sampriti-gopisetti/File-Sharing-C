#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <stdint.h>
#include <arpa/inet.h>

int recv_all(int fd, void *buf, int len){
    char *p=buf;
    int got=0;
    while(got<len){
        int n=recv(fd, p+got, len-got, 0);
        if (n<=0){
            return -1;
        }
        got+=n;
    }
    return 0;
}

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

    int listen_status=listen(socketfd, 5); //Listen for incoming connections with a backlog of 5
    if (listen_status==-1){
        perror("listen");
    }
    else{
        printf("Listen successful\n");
    }

    int cnnfd=accept(socketfd,NULL,NULL); //Connect to a client
    if (cnnfd==-1){
        perror("accept");
    }
    else{
        printf("Connection accepted: %d\n",cnnfd);

        uint32_t namelen_net;
        recv_all(cnnfd, &namelen_net, 4);
        uint32_t namelen=ntohl(namelen_net); //Convert length of name from network byte order to host byte order

        char filename[256];
        recv_all(cnnfd, filename, namelen);
        filename[namelen]='\0'; 

        char outname[300];
        snprintf(outname, sizeof(outname), "recieved_%s", filename);
        printf("Receiving file: %s\n", outname);

        FILE *fp=fopen(outname,"wb");
        if (fp==NULL){
            perror("fopen");
            return 1;
        }    

        char buffer[1024];
        int n;
        while ((n=recv(cnnfd, buffer, sizeof(buffer), 0))>0){
            fwrite(buffer, 1, n, fp);
        }
        fclose(fp);
        printf("File received successfully\n");
    }

}