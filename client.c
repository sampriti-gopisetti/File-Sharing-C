#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <stdint.h>

int send_all(int fd, const void *buf, int len) {
    const char *p = buf;
    int sent = 0;
    while (sent < len) {
        int n = send(fd, p + sent, len - sent, 0);
        if (n <= 0) return -1;
        sent += n;
    }
    return 0;
}

int main(int argc, char *argv[]){
    if (argc<2){
        printf("Usage: %s <filename>\n", argv[0]);
        return 1;
    }

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
    inet_pton(AF_INET, "100.64.98.194", &addr.sin_addr); //Convert IP address to network understandable format

    int connect_status=connect(socketfd, (struct sockaddr *)&addr, sizeof(addr)); //Connect to the server
    if (connect_status==-1){
        perror("connect");
    }
    else{
        printf("Connected to server successfully\n");
    }
    
    char *filename=argv[1];
    uint32_t namelen=strlen(filename);
    uint32_t namelen_net=htonl(namelen); //Convert lenght of name to network byte order
    send_all(socketfd, &namelen_net, sizeof(namelen_net));
    send_all(socketfd, filename, namelen);

    FILE *fp=fopen(filename,"rb");
    if (fp==NULL){
        perror("fopen");
        return 1;
    }
    char buffer[1024];
    size_t bytes;
    while ((bytes=fread(buffer, 1, sizeof(buffer)-1, fp))>0){
        send_all(socketfd, buffer, bytes);
    }
    fclose(fp);
}