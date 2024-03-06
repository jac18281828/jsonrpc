#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/epoll.h>
#include <netdb.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <ctype.h>

#include "rpc.h"

#define MAXEVENTS 64

static int set_fd_nonblock(int fd);
static int next_event(int efd, int sfd, struct epoll_event *events);
static int parse_http(int fd, char* buffer, int nRead);

int rpc_init(struct rpc *rpc, const char *port) {

    struct addrinfo *rp, *result;
    struct addrinfo hints;

    memset (&hints, 0, sizeof (hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;
    hints.ai_protocol = 0;
    hints.ai_canonname = NULL;
    hints.ai_addr = NULL;
    hints.ai_next = NULL;

    if(getaddrinfo(NULL, port, &hints, &result) != 0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(errno));
        return -1;
    }

    for(rp = result; rp != NULL; rp = rp->ai_next) {
        rpc->sfd = socket(rp->ai_family, rp->ai_socktype, rp->ai_protocol);
        if(rpc->sfd == -1) {
            continue;
        }
        if(bind(rpc->sfd, rp->ai_addr, rp->ai_addrlen) == 0) break;
        close(rpc->sfd);
    }
    if(rp == NULL) {
        fprintf(stderr, "Could not bind\n");
        return -1;
    }
    freeaddrinfo(result);
    return 0;
}

int rpc_listen(struct rpc *rpc) {

    struct epoll_event *events;
    struct epoll_event event;

    if(set_fd_nonblock(rpc->sfd) == -1) {
        perror("set_fd_nonblock");
        return -1;
    }

    if(listen(rpc->sfd, SOMAXCONN) == -1) {
        perror("listen");
        return -1;
    }
    rpc->efd = epoll_create1(0);
    if(rpc->efd == -1) {
        perror("epoll_create");
        return -1;
    }
    event.data.fd = rpc->sfd;
    event.events = EPOLLIN | EPOLLET;
    if(epoll_ctl(rpc->efd, EPOLL_CTL_ADD, rpc->sfd, &event) == -1) {
        perror("epoll_ctl");
        return -1;
    }
    events = calloc(MAXEVENTS, sizeof(event));

    while(next_event(rpc->efd, rpc->sfd, events) != -1) {
        continue;
    }
    free(events);
    return 0;
}

int rpc_destroy(struct rpc *rpc) {
    close(rpc->sfd);
    close(rpc->efd);
    return 0;
}

static int set_fd_nonblock(int fd) {
    int flags;
    if((flags = fcntl(fd, F_GETFL, 0)) == -1) {
        return -1;
    }
    if(fcntl(fd, F_SETFL, flags | O_NONBLOCK) == -1) {
        return -1;
    }
    return 0;
}

static int next_event(int efd,  
                      int sfd, 
                      struct epoll_event *events) {
  int i, n, quit=0;
  struct epoll_event event;
  n = epoll_wait (efd, events, MAXEVENTS, -1);
  for (i = 0; i < n; i++)
    {
      if ((events[i].events & EPOLLERR) ||
          (events[i].events & EPOLLHUP) ||
          (!(events[i].events & EPOLLIN))) {
        fprintf (stderr, "epoll error\n");
        close (events[i].data.fd);
        continue;
      } else if (sfd == events[i].data.fd) {
        // event on sfd
        struct sockaddr in_addr;
        socklen_t in_len;
        int fd;
        in_len = sizeof in_addr;
        fd = accept (sfd, &in_addr, &in_len);
        if (fd >= 0 && set_fd_nonblock(fd) != -1) {
          event.data.fd = fd;
          event.events = EPOLLIN | EPOLLET;
          if(epoll_ctl (efd, 
                        EPOLL_CTL_ADD, 
                        fd, &event) == 0) {
            continue;
          } 
        } else {
          perror ("accept failed");
          abort ();
        }        
} else if(events[i].events & EPOLLIN) {
        // handle read
        int done = 0;
        while(done == 0) {
          ssize_t nRead;
          char buffer[1024];
          
          nRead = read (events[i].data.fd, 
                        buffer, 
                        sizeof buffer);
          fprintf(stderr, "read %ld bytes\n", nRead);
          write(1, buffer, nRead);
          if (nRead == -1) {
            if (errno != EAGAIN) 
              done = 1;
            break;
          } else if (nRead  == 0) {
            done = 1;
            break;
          }        
          quit = parse_http(events[i].data.fd, 
                            buffer, 
                            nRead);
                          
          if (done)
            {
              close (events[i].data.fd);
            }
        }          
} else {
        fprintf (stderr, "unexpected event\n");
        abort ();
      }
    }
  return quit;
}

static int parse_http(int fd, 
                      char* buffer, 
                      int nRead) {
  int pathStart;
  int pathEnd;
  int i;
  for(i=0; i<nRead; i++)
    if(isspace(buffer[i])) { pathStart = i+1; break; }

  const char *INVALID = "NOTFOUND";
  if(i==nRead) 
    if(write(fd, 
             INVALID, 
             strlen(INVALID)) <= 0) close(fd);

  for(i=pathStart+1; i<nRead; i++) 
    if(isspace(buffer[i])) { pathEnd = i; break; }

  if(i==nRead) pathEnd = nRead;
  for(i=pathStart; i<pathEnd; i++) {
    if(buffer[i] == 'q' &&
       i <= pathEnd - 4 &&
       buffer[i+1] == 'u' &&
       buffer[i+2] == 'i' &&
       buffer[i+3] == 't') {
        const char *OKAY = "OKAY";
      if(write(fd, OKAY, strlen(OKAY)) <= 0) 
        close(fd);
      return 1;
    }
  }
  return 0;
}
                