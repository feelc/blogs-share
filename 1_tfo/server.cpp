#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <arpa/inet.h>

int main()
{
    int serverSock;
    struct sockaddr_in addr;

    int clientSock;
    struct sockaddr_in clientAddr;
    socklen_t addrLen;

    char buf[1024];
    int read;

    serverSock = socket(AF_INET, SOCK_STREAM, 0);
    if(serverSock == -1)
    {
        printf("open socket failed, err %s\n", strerror(errno));
        return 1;
    }

#ifdef DEFER_ACCEPT
    int soValue = 1;
    if(setsockopt(serverSock, IPPROTO_TCP, TCP_DEFER_ACCEPT, &soValue, sizeof(soValue))<0)
    {
        printf("set TCP_DEFER_ACCEPT failed, err %s\n", strerror(errno));
        return 10;
    }
#endif

    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(7890);
    addr.sin_addr.s_addr=inet_addr("127.0.0.1");

    if(bind(serverSock, (struct sockaddr *)&addr, sizeof(addr))<0)
    {
        printf("bind failed, err %s\n", strerror(errno));
        return -1;
    }

#ifdef FAST_OPEN
    printf("opent tfo\n");
    int qlen=5;
    setsockopt(serverSock, SOL_TCP, TCP_FASTOPEN, &qlen, sizeof(qlen));
#endif

    if(listen(serverSock,511)<0)
    {
        printf("listen failed, err %s\n", strerror(errno));
        return -1;
    }

    while(1)
    {
        addrLen = sizeof(clientAddr);
        clientSock = accept(serverSock, (struct sockaddr *)&clientAddr, &addrLen);
        if(clientSock < 0)
        {
            printf("accept failed, err %s\n", strerror(errno));
            return -1;
        }

        read = recv(clientSock, buf, 1024, 0);
        write(STDOUT_FILENO, buf, read);
        close(clientSock);
    }

    return 0;
}