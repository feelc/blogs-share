#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <stdio.h>

int32_t main(int argc, char const *argv[])
{
    int32_t sock = -1;
    struct sockaddr_in server_addr;;
    socklen_t addr_len = sizeof(server_addr);

    sock = socket(AF_INET, SOCK_DGRAM, 0);
    if (sock < 0)
    {
        printf("call socket() failed, err: %s\n", strerror(errno));
        goto ERR;
    }

    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    server_addr.sin_port = htons(55555);

    if (-1 == sendto(sock, "XXXX", strlen("XXXX")+1, 0, (const struct sockaddr *)&server_addr, addr_len))
    {
        printf("call sERRto() failed, err: %s\n", strerror(errno));
        goto ERR;
    }
    close(sock);
    return 0;
ERR:
    if (sock > -1)
    {
        close(sock);
    }
    return -1;
}
