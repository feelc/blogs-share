#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <errno.h>
#include <pthread.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <event.h>
#include "query_thread.h"

extern int32_t tlog[];

#define SAFE_RECVFROM(sock, buf, buf_len, src_addr) \
    do \
    { \
        socklen_t addr_len_ = sizeof(struct sockaddr); \
        socklen_t *paddr_len_ = &addr_len_; \
        if (src_addr == NULL) \
        { \
            paddr_len_ = NULL; \
        } \
        if (-1 == recvfrom(sock, buf, buf_len, 0, (struct sockaddr *)src_addr, paddr_len_)) \
        { \
            printf("call recvfrom() failed, err: %s", strerror(errno)); \
            if (errno == EINTR) \
            { \
                continue; \
            } \
            goto ERR; \
        } \
    } while (0)

int32_t create_socket()
{
    int32_t sock = -1;
    struct sockaddr_in local_addr;
    socklen_t addr_len = sizeof(local_addr);

    sock = socket(AF_INET, SOCK_DGRAM, 0);
    if (sock < 0)
    {
        printf("call socket() failed, err: %s\n", strerror(errno));
        goto ERR;
    }

    int32_t enable = 1;
    if (-1 == setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(enable)))
    {
        printf("call setsockopt() failed, set SO_REUSEADDR failed, err: %s\n", strerror(errno));
    }
    if (-1 == setsockopt(sock, SOL_SOCKET, SO_REUSEPORT, &enable, sizeof(enable)))
    {
        printf("call setsockopt() failed, set SO_REUSEPORT failed, err: %s\n", strerror(errno));
    }

    memset(&local_addr, sizeof(local_addr), 0);
    local_addr.sin_family = AF_INET;
    local_addr.sin_addr.s_addr = htonl(INADDR_ANY); //inet_addr("127.0.0.1");
    local_addr.sin_port = htons(55555);
    if (-1 == bind(sock, (const struct sockaddr *)&local_addr, addr_len))
    {
        printf("call bind() failed, err: %s\n", strerror(errno));
        goto ERR;
    }
    return sock;
ERR:
    if (sock > 0)
    {
        close(sock);
    }
    return -1;
}


void read_cb(evutil_socket_t sock, short events, void *arg)
{
    uint32_t idx = *(uint32_t *)arg;

    char buf[1024];
    struct sockaddr_in client_addr;
    SAFE_RECVFROM(sock, buf, sizeof(buf), &client_addr);
    tlog[idx] += 1;
    printf("read_cb, tid: %lu, client port: %u, buf: %s\n", pthread_self(), ntohs(client_addr.sin_port), buf);
ERR:
    return ;
}

void *query_thread_cb(void *arg)
{
    printf("tid: %lu\n", pthread_self());


    int32_t sock = create_socket();
    if (-1 == sock)
    {
        goto ERR;
    }
    struct event_base *pbase = NULL;
    struct event *pread_event = NULL;

    pbase = event_base_new();
    if (NULL == pbase)
    {
        printf("call event_base_new() failed\n");
        goto ERR;
    }
    pread_event = event_new(pbase, sock, EV_READ|EV_PERSIST, read_cb, arg);
    if (NULL == pread_event)
    {
        printf("call event_read() failed\n");
        goto ERR;
    }
    event_add(pread_event, NULL);

    printf("tid: %lu, enter event dispatch...\n", pthread_self());
    if (-1 == event_base_dispatch(pbase))
    {
        printf("call event_base_dispatch() failed\n");
        goto ERR;
    }
    event_base_free(pbase);
ERR:
    close(sock);
    return NULL;
}