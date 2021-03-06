#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>

#define IOCTL_MAGIC_NUMBER	'B'
#define IOCTL_BUTTON_REQ        _IO( IOCTL_MAGIC_NUMBER, 0)

#define BN 8
#define PORT 9000
#define IP  "192.168.28.41"
#define BUFSIZE 10

int main(int argc, char ** argv)
{
    int c_socket, len, n, but_value = -1;
    struct sockaddr_in c_addr;

    // Socket Connection Configure
    c_socket = socket(PF_INET,SOCK_STREAM,0);

    memset(&c_addr,0,sizeof(c_addr));
    c_addr.sin_addr.s_addr = inet_addr(IP);
    c_addr.sin_family = AF_INET;
    c_addr.sin_port = htons(PORT);

    // Device Driver Connection
    int fd_but = open("/dev/button_mod",O_RDWR);
    if( fd_but < 0 )
    {
        perror("Failed to open the device");
        return -1;
    }

    // Socket Connection
    printf("Connect to %s...\n",IP);
    if(connect(c_socket,(struct sockaddr *) &c_addr, sizeof(c_addr)) == -1)
    {
        printf("Connection Error\n");
        close(c_socket);
        return -1;
    }

    while(1)
    {
        ioctl(fd_but, IOCTL_BUTTON_REQ, &but_value);
        //printf("Button: %d\n",but_value); 
        if(but_value != -1)
        {
            char byte_value[BUFSIZE] = "";
            sprintf(byte_value, "%d", but_value);
            send(c_socket, byte_value, sizeof(byte_value), 0);
            but_value = -1;
        }

        sleep(1);    

    }

    close(fd_but);
    close(c_socket);
    return 0;
}
