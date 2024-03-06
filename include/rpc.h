#pragma once

#include <sys/epoll.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>

struct rpc
{
    int sfd, efd;
};

int
rpc_init(struct rpc *rpc, const char *port);

int 
rpc_listen(struct rpc *rpc);

int rpc_destroy(struct rpc *rpc);