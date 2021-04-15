#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netinet/tcp.h>

#ifndef MSG_FASTOPEN
#define MSG_FASTOPEN   0x20000000
#endif

int main()
{
    int clientSock;
    struct sockaddr_in addr;

    clientSock = socket(AF_INET, SOCK_STREAM, 0);

    if(clientSock == -1)
    {
        printf("open socket failed, err %s\n", strerror(errno));
        return 1;
    }

#ifdef DEFER_ACCEPT
    int soValue = 1;
    if(setsockopt(clientSock, IPPROTO_TCP, TCP_DEFER_ACCEPT, &soValue, sizeof(soValue))<0)
    {
        printf("set TCP_DEFER_ACCEPT failed, err %s\n", strerror(errno));
        return -1;
    }
#endif

    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(7890);
    addr.sin_addr.s_addr=inet_addr("127.0.0.1");

#ifdef FAST_OPEN

    int ret = sendto(clientSock, "Hello\n", 6, MSG_FASTOPEN,
                     (struct sockaddr *)&addr, sizeof(addr));

    if(ret<0)
    {
        printf("sendto failed, err %s\n", strerror(errno));
        return -1;
    } else {
        printf("tfo send succ\n");
    }

#else
    if(connect(clientSock, (struct sockaddr *)&addr, sizeof(addr))<0)
    {
        printf("connect failed, err %s\n", strerror(errno));
        return -1;
    }

    if(send(clientSock, "Hello\n", 6, 0)<0)
    {
        printf("send failed, err %s\n", strerror(errno));
        return -1;
    }
#endif

    close(clientSock);

    return 0;
}